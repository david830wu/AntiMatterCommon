#ifndef AMC_LOGGER_INTERNAL_H_
#define AMC_LOGGER_INTERNAL_H_

/* Internal contracts shared by the AmcLogger implementation files.
 * Nothing here is public API; tests may declare amc_logger_internal_test_reset().
 * Every name carries amc_logger_internal_ / AMC_LOGGER_INTERNAL_ (CLAUDE.md). */

#include "AmcLogger.h"

#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

/* ---- limits (see docs/Architecture.md §11) ---- */
#define AMC_LOGGER_INTERNAL_MODULE_MAX        128
#define AMC_LOGGER_INTERNAL_MSG_SIZE_MIN      256
#define AMC_LOGGER_INTERNAL_MSG_SIZE_MAX      8192
#define AMC_LOGGER_INTERNAL_QUEUE_SIZE_MIN    16
#define AMC_LOGGER_INTERNAL_QUEUE_SIZE_MAX    1048576
#define AMC_LOGGER_INTERNAL_FLUSH_MS_MAX      3600000
#define AMC_LOGGER_INTERNAL_WORKER_BATCH      64
/* ~5 us of polling before parking: long enough that a saturated stream never parks
 * the worker, short enough that sparse traffic (~10k msg/s) costs only ~5% of one
 * core in idle spinning */
#define AMC_LOGGER_INTERNAL_WORKER_SPIN_ITERS 512

#if defined(__x86_64__)
#define AMC_LOGGER_INTERNAL_CPU_RELAX() __builtin_ia32_pause()
#elif defined(__aarch64__)
#define AMC_LOGGER_INTERNAL_CPU_RELAX() __asm__ volatile("yield")
#else
#define AMC_LOGGER_INTERNAL_CPU_RELAX() ((void)0)
#endif
#define AMC_LOGGER_INTERNAL_FILE_IOBUF_SIZE   65536
#define AMC_LOGGER_INTERNAL_CONFIG_SIZE_MAX   (1024 * 1024)
#define AMC_LOGGER_INTERNAL_PATH_MAX          4096
#define AMC_LOGGER_INTERNAL_TRUNC_MARKER      "...(truncated)"
/* escaped-string ring: one slot per possible AMC_LOGGER_JSON pair, each holding one
 * escaped value incl. "..." + NUL */
#define AMC_LOGGER_INTERNAL_JSON_ESC_SLOTS    16
#define AMC_LOGGER_INTERNAL_JSON_ESC_BUF      1024
/* fixed cpu_set_t width; CPU_SET beyond it is undefined behavior */
#define AMC_LOGGER_INTERNAL_WORKER_CPU_MAX    1023

#define AMC_LOGGER_INTERNAL_ALIGN64(x) (((x) + 63u) & ~((size_t)63u))

enum amc_logger_internal_state {
    AMC_LOGGER_INTERNAL_UNINIT = 0,
    AMC_LOGGER_INTERNAL_READY,
    AMC_LOGGER_INTERNAL_SHUTDOWN
};
enum amc_logger_internal_policy {
    AMC_LOGGER_INTERNAL_POLICY_BLOCK = 0,
    AMC_LOGGER_INTERNAL_POLICY_OVERRUN_OLDEST,
    AMC_LOGGER_INTERNAL_POLICY_DISCARD_NEW
};

/* ---- logger: public prefix + internal extension, one allocation ---- */
struct amc_logger_internal_module {
    struct amc_logger_internal_handle  pub;    /* MUST stay first (_Static_assert'ed) */
    char                              *module; /* owned copy, stripped basename       */
    struct amc_logger_internal_module *next;   /* registry chain                      */
};

/* ---- message slot header; payload bytes follow contiguously ---- */
struct amc_logger_internal_msg {
    struct timespec ts;
    const char     *module;           /* -> registry module name (permanent)        */
    const char     *function;         /* __func__, static storage                    */
    const char     *event;            /* string literal (Design D3 contract)         */
    int32_t         trader_id;
    uint8_t         level;
    uint8_t         has_id;
    uint8_t         truncated;        /* payload hit its cap at capture              */
    uint16_t        payload_len;
};

/* ---- bounded MPSC ring, mutex + condvars ---- */
struct amc_logger_internal_queue {
    pthread_mutex_t mtx;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;
    pthread_cond_t  flush_done;
    char           *slots;            /* capacity * slot_stride                      */
    char           *batch;            /* copy-out buffer; overrun_oldest only        */
    uint32_t        capacity;
    uint32_t        slot_stride;
    uint32_t        head, count;      /* ring state, under mtx                       */
    uint32_t        inflight;         /* slots checked out zero-copy by the worker;  *
                                       * producers treat them as occupied (under mtx)*/
    uint64_t        seq_enq;          /* total accepted, under mtx                   */
    _Atomic uint64_t enq_hint;        /* mirrors seq_enq; lets the worker spin-poll  *
                                       * for new messages without touching the mutex */
    uint64_t        seq_flushed;      /* written + fflushed watermark, under mtx     */
    uint64_t        flush_req;        /* highest requested flush target, under mtx   */
    int             worker_waiting;   /* worker parked on not_empty (under mtx);
                                       * producers skip the wake syscall otherwise   */
    int             stop;
    int             inited;
};

struct amc_logger_internal_sink {
    pthread_mutex_t mtx;
    FILE           *fp;               /* stdout, or fopen'd file                     */
    char           *iobuf;            /* file sink only                              */
    int             is_file;
    int             closed;           /* checked under mtx; post-close writes no-op  */
    int             dirty;
    int             mtx_inited;
};

struct amc_logger_internal_cfg_logger {
    char                                  *module;
    int                                    level;
    struct amc_logger_internal_cfg_logger *next;
};

struct amc_logger_internal_config {
    int      default_level;
    int      async;
    int      critical_sync;
    int      worker_cpu;              /* -1 = unpinned (Linux only; ignored in
                                       * sync mode; macOS warns + unpinned)   */
    int      utc_offset_hours;
    enum amc_logger_internal_policy policy;
    uint32_t queue_size;
    uint32_t max_message_size;
    uint32_t flush_every_ms;
    int      has_stdout;
    int      has_file;
    char    *file_path_template;      /* owned                                       */
    struct amc_logger_internal_cfg_logger *loggers;  /* owned                        */
};

struct amc_logger_internal_global {
    _Atomic int       state;          /* enum amc_logger_internal_state              */
    struct amc_logger_internal_config cfg;
    struct amc_logger_internal_sink   sinks[2];  /* [0] = stdout, [1] = file         */
    struct amc_logger_internal_queue  queue;
    pthread_t         worker;
    int               worker_started;

    _Atomic uint64_t  st_enqueued, st_dropped_new, st_overwritten, st_blocks,
                      st_truncated, st_write_errors, st_high_water;
    /* last values reported in a MessageLoss summary (worker/shutdown only) */
    uint64_t          rep_dropped, rep_overwritten, rep_truncated, rep_werr;

    _Atomic int       warned_before_init;
};
extern struct amc_logger_internal_global amc_logger_internal_global;

#define AMC_LOGGER_INTERNAL_STAT_INC(field)                                         \
    atomic_fetch_add_explicit(&amc_logger_internal_global.field, 1,                 \
                              memory_order_relaxed)

/* ---- Core ---- */
void amc_logger_internal_now(struct timespec *out);
struct amc_logger_internal_module *amc_logger_internal_logger_get(const char *module);
void amc_logger_internal_maybe_emit_loss_summary(void);

/* ---- Render ---- */
size_t amc_logger_internal_render_line(char *dst, size_t cap,
                                       const struct amc_logger_internal_msg *m,
                                       const char *payload, int *out_truncated);

/* ---- Sink ---- */
int  amc_logger_internal_sinks_setup(char *err, size_t errsz);
void amc_logger_internal_emit(const struct amc_logger_internal_msg *m,
                              const char *payload);
void amc_logger_internal_sinks_flush(void);
void amc_logger_internal_sinks_close(void);
int  amc_logger_internal_path_expand(const char *tmpl, char *out, size_t outsz,
                                     char *err, size_t errsz);

/* ---- Config (pure; no globals) ---- */
void amc_logger_internal_config_defaults(struct amc_logger_internal_config *out);
int  amc_logger_internal_config_parse(const char *text, size_t len,
                                      const char *diag_name,
                                      struct amc_logger_internal_config *out,
                                      char *err, size_t errsz);
void amc_logger_internal_config_free(struct amc_logger_internal_config *cfg);

/* ---- Queue / worker ---- */
int  amc_logger_internal_async_start(char *err, size_t errsz);
void amc_logger_internal_async_stop(void);
void amc_logger_internal_async_enqueue(const struct amc_logger_internal_msg *m,
                                       const char *payload);
int  amc_logger_internal_async_flush(void);

#ifdef AMC_LOGGER_TESTING
/* Test-only: tear everything down to AMC_LOGGER_INTERNAL_UNINIT so a test binary can
 * simulate many independent process lifetimes. Registry loggers are kept alive
 * (call-site caches point at them); init re-applies levels to them. */
void amc_logger_internal_test_reset(void);
void amc_logger_internal_async_destroy(void);
void amc_logger_internal_sinks_destroy(void);
typedef void (*amc_logger_internal_clock_fn)(struct timespec *out);
void amc_logger_internal_set_clock(amc_logger_internal_clock_fn fn);
#endif

#endif /* AMC_LOGGER_INTERNAL_H_ */
