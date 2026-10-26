# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with the logger
component. The repository-wide rules — naming convention, packaging, layering, build
helpers — are in the AntiMatterCommon root `CLAUDE.md`; this file adds what is specific
to the logger.

## Commands

Everything builds from the AntiMatterCommon root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug   # configure (tests ON when top-level)
cmake --build build -j                          # every component + tests
ctest --test-dir build -L logger --output-on-failure   # the ten logger chapters
ctest --test-dir build -R AmcLoggerTest07       # a single chapter
```

Sanitizer legs (both must stay green; run before claiming concurrency work done):

```bash
cmake -S . -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-fsanitize=thread -g -O1"
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-sanitize-recover=all -g"
```

Each chapter runs in its own working directory (`build/run/AmcLoggerTestNN_<Topic>/`) —
look there for a failing test's config/log files. The library builds with `-Werror`;
tests build without it.

Benchmarks and the example (both OFF by default):

```bash
cmake -S . -B build-rel -DCMAKE_BUILD_TYPE=Release -DAMC_BUILD_BENCH=ON -DAMC_BUILD_EXAMPLES=ON
taskset -c 2,3 ./build-rel/bin/AmcLoggerBenchEnqueue   # pin TWO cores: worker needs its own
cd components/logger && ../../build-rel/bin/AmcLoggerExample   # config path is relative
# spdlog comparison: add -DAMC_LOGGER_BENCH_SPDLOG=ON (FetchContent, needs network)
```

The installed package is validated by the root `tests/package/`
(`AmcLoggerPackageSmoke[Cpp]`, see the root `CLAUDE.md` for the commands).

Before accepting any queue/worker change: all three suites, the TSan stress loop
(`ctest -R "AmcLoggerTest07|AmcLoggerTest09"` ×25), and
AmcLoggerBenchEnqueue/AmcLoggerBenchThroughput — the queue's tuning history and why each
piece exists is Architecture §14.13; don't re-tune without reading it.

## Where truth lives

The logger is developed in agreed phases; documents are canonical and kept current:

- `docs/Design.md` — FINAL agreed requirements (with dated amendments). Do not change
  behavior away from it without discussing with David first.
- `docs/Architecture.md` — the implementation contract; §14 records every refinement
  (§14.19: the move into AntiMatterCommon and the naming convention). When implementation
  forces a deviation, add it there.
- `docs/InitDesign.md` — superseded draft, historical only. `Question1.md`/`Answer1.md`
  are the design-review Q&A the decisions trace back to; they keep the original names.

## Big picture

Data flow (async, the default):

```
AMC_LOGGER_*() macro                    AmcLoggerCore.c
  → call-site static cached handle (acquire/release atomic)
  → relaxed level check (struct amc_logger_internal_handle.level is in the header for this)
  → amc_logger_internal_log(): capture timestamp + vsnprintf payload on the stack
  → AmcLoggerQueue.c: copy into MPSC ring slot under queue mutex
  → single worker thread "amc-logger": batched dequeue, then per-line
  → AmcLoggerSink.c amc_logger_internal_emit(): render (AmcLoggerRender.c) + fwrite
```

Sync mode and `critical_sync` CRITICALs skip the queue and call
`amc_logger_internal_emit()` directly from the calling thread — one shared emission path
for all modes.

Two library targets: `amc_logger` (production) and `amc_logger_testing`
(`-DAMC_LOGGER_TESTING`: adds `amc_logger_internal_test_reset()` and a clock hook). Tests
must link the testing variant; production has no reset and init/shutdown happen once per
process, ever.

## Invariants you must not break

- **No thread ever holds two locks.** The worker emits line-at-a-time; the queue mutex
  is always released before any sink mutex is taken. Deadlock-freedom rests on this.
- **Shutdown never frees** logger structs, the registry, or queue slot memory — that is
  what makes post-shutdown log calls safe without refcounting. Only OS resources
  (worker thread, `FILE*`) are released. Same reason registry loggers survive
  `amc_logger_internal_test_reset()`: call-site static caches point at them, so
  `amc_logger_init` re-sweeps all existing loggers' levels before applying overrides.
- **The `"\1" __VA_ARGS__` sentinel** in the macros: enforces literal format strings,
  makes empty payloads work, and avoids `-Wformat-zero-length` in user builds.
  `amc_logger_internal_log` skips the first byte. EVENT strings are stored by pointer —
  the literal-only contract is documented API.
- **`AMC_LOGGER_JSON` must stay a pure compile-time literal composer** (Architecture
  §14.14): it expands to exactly the raw form's format literal + args — chapter 10
  asserts byte equality. Never turn it into anything with runtime cost. The single
  deliberate runtime piece is `AMC_LOGGER_KV_STR_ESC` → `amc_logger_json_escape()`
  (per-thread static TLS ring, 16 × 1 KB — §14.15); keep its slot count ≥ the
  composer's pair cap.
- **Fail fast** (philosophy 00): config mistakes are init failures with a
  `file:line: message` stderr diagnostic — never a silent default. Every new
  diagnostic needs a chapter-04 test asserting its exact text. Runtime loss (drops,
  truncation, write errors) is counted in stats and announced via `[MessageLoss]`
  summary lines, never silent.
- The config parser (`AmcLoggerConfig.c`) is a pure function over a string — keep it
  free of globals so it stays directly unit-testable. It accepts a strict YAML subset
  and must reject everything outside it loudly (no sequences, anchors, multiline).
- **One logger per process, also when embedded**: AntiMatterFusion links the static
  library PRIVATE into several shared libraries and relies on symbol interposition, so
  everything with external linkage keeps default visibility (root `CLAUDE.md`).

## Tests are the user manual

`tests/AmcLoggerTest01…AmcLoggerTest10` are documentation chapters written from the user's
point of view, meant to be read in order — narrative header comment per file, one behavior
per test. New user-visible behavior belongs in the fitting chapter, written in the same
style. Test discipline: always release captured stdout/stderr (`release_stdout()`)
*before* asserting, or Unity's failure output lands in the capture file.
`AmcLoggerTestSupport.h`'s `assert_log_line()` is the executable line-format spec; the
MODULE a chapter asserts is its own file name (`"AmcLoggerTest02_LogFormat"`).
AmcLoggerTest08 is compiled with `-DAMC_LOGGER_ACTIVE_LEVEL=AMC_LOGGER_LEVEL_WARN` to
prove compile-time stripping.

## Conventions

- Pure C, `-std=gnu11` (C11 + `##__VA_ARGS__`, `_Thread_local`, GNU attributes). The
  public header is usable from C and C++ (Architecture §14.16–17); the C++ package smoke
  test keeps that true. Linux gcc-13 and macOS AppleClang are the supported toolchains;
  POSIX APIs only.
- Naming follows the root convention: API `amc_logger_*` / `AMC_LOGGER_*`, everything else
  `amc_logger_internal_*` / `AMC_LOGGER_INTERNAL_*`; file-local helpers are unprefixed.
- Zero runtime dependencies beyond libc/pthreads. Unity (test-only) is vendored at the
  root; the spdlog benchmark dependency is allowed for bench targets only.

## Current state / roadmap

v1 complete and released standalone up to v0.4.0; now a component of AntiMatterCommon
0.5.0. Common retains a single import snapshot; standalone history and release tags
remain in [AntiMatterLogger](https://github.com/david830wu/AntiMatterLogger).
Ten-chapter suite green (incl. TSan/ASan/UBSan), `bench/`
harness with measured numbers (README table; investigation in Architecture §14.13),
`examples/` target. Remaining ideas live in Design.md §13 future-work parking lot.
