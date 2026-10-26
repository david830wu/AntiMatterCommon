# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug   # configure (tests ON when top-level)
cmake --build build -j                          # every component + tests
ctest --test-dir build --output-on-failure      # all suites
ctest --test-dir build -L <comp>                # one component's suite
ctest --test-dir build -R AmcCommonTest01       # one test
```

Sanitizer legs (CI runs both on Linux and macOS):

```bash
cmake -S . -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-fsanitize=thread -g -O1"
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-sanitize-recover=all -g"
```

Executables land in `build/bin/`; each test runs in its own directory `build/run/<test>/`
— look there for a failing test's temp files. Examples and benchmarks are OFF by
default (`-DAMC_BUILD_EXAMPLES=ON`, `-DAMC_BUILD_BENCH=ON`).

The installed package is validated by `tests/package/` (C and C++ consumers); after
touching install/export logic run:

```bash
cmake --install build --prefix /tmp/amc-prefix
cmake -S tests/package -B /tmp/amc-pkg -DCMAKE_PREFIX_PATH=/tmp/amc-prefix
cmake --build /tmp/amc-pkg && /tmp/amc-pkg/AmcCommonPackageSmoke && /tmp/amc-pkg/AmcCommonPackageSmokeCpp
```

`scripts/build_component.sh` is the interface other AntiMatter stacks (AntiMatterDagger,
AntiMatterFusion, the antimatterkit images) call: configure + build + install into
`$AM_PREFIX` (see README for its variables).

## Structure

One repository, one CMake package (`AntiMatterCommon`), one version. Components live in
`components/<comp>/` and are listed in `AMC_COMPONENTS` in the top-level
`CMakeLists.txt`, in dependency order. A component is a plain part of Common: it has
no `project()`, Unity copy, install/package logic, build script or CI of its own —
those exist once, at the root. `cmake/AmcCommon.cmake` holds the shared helpers a
component's CMakeLists uses: `amc_add_library`, `amc_add_test`, `amc_add_bench`,
`amc_add_example`.

```
components/<comp>/
├── CMakeLists.txt          amc_add_library(<comp> …) + tests/ bench/ examples/
├── include/Amc<Comp>.h     public header(s), installed flat into include/
├── src/                    Amc<Comp>*.c, Amc<Comp>Internal.h
├── tests/                  Amc<Comp>Test<NN>_<Topic>.c — the user manual
├── bench/  examples/
├── docs/                   the component's canonical design record
└── CLAUDE.md               component-specific invariants
```

Root-level code is the pseudo-component `common`: the umbrella header `AmcCommon.h`,
the generated `AmcCommonVersion.h`, cross-component tests in `tests/`, examples in
`examples/`. The umbrella target `amc_common` links every component and is exported as
`AntiMatter::AntiMatterCommon` — named after the package, the only name outside the
convention below.

## Naming convention — strict, for every component

`<comp>` is the lowercase directory name (`logger`), `<Comp>` its capitalized form
(`Logger`), `<COMP>` the uppercase form (`LOGGER`).

| What | Pattern | Example |
| --- | --- | --- |
| Public functions and types | `amc_<comp>_*`, `struct amc_<comp>_*` | `amc_logger_init`, `struct amc_logger_stats` |
| Public macros | `AMC_<COMP>_*` | `AMC_LOGGER_INFO`, `AMC_LOGGER_JSON` |
| Every other name in the component's namespace — internal functions, globals, types, enum constants and macros, header plumbing, macro-local names | `amc_<comp>_internal_*`, `AMC_<COMP>_INTERNAL_*` | `amc_logger_internal_emit`, `AMC_LOGGER_INTERNAL_MSG_SIZE_MAX` |
| Include guards | `AMC_<COMP>[_<PART>]_H_` | `AMC_COMMON_VERSION_H_` |
| Files | `Amc<Comp>*.{h,c}` | `AmcLoggerQueue.c` |
| Library target | `amc_<comp>`, exported as `AntiMatter::Amc<Comp>` | `amc_logger` → `AntiMatter::AmcLogger` |
| Test-seam build of the library | `amc_<comp>_testing`, compiled with `AMC_<COMP>_TESTING` | `amc_logger_testing` |
| Package component | `<Comp>` | `find_package(AntiMatterCommon 0.5 COMPONENTS Logger)` |
| Tests — one name for source file, target and ctest test | `Amc<Comp>Test<NN>_<Topic>`, ctest label `<comp>` | `AmcLoggerTest07_AsyncAndReliability` |
| Test helpers | `Amc<Comp>Test<Role>.{h,c}` | `AmcLoggerTestSupport.h` |
| Benchmarks, examples, package smoke tests | `Amc<Comp>Bench<Name>`, `Amc<Comp>Example[<Name>]`, `Amc<Comp>PackageSmoke[Cpp]` | `AmcLoggerBenchEnqueue`, `AmcLoggerExample` |
| Threads a component starts | `amc-<comp>[-<role>]` (≤ 15 characters) | `amc-logger` |
| CMake options | `AMC_BUILD_{TESTS,EXAMPLES,BENCH}` for all, `AMC_<COMP>_*` for one component | |

So an `amc_<comp>_` / `AMC_<COMP>_` name without `_internal_` is always API, declared in
`include/`, and nothing uses the bare `amc_` / `AMC_` namespace. File-local (`static`)
names are free and carry no prefix (test helpers such as `assert_log_line`). Example
programs model user code, so their sources keep user-style names; only the target follows
the pattern. `amc_add_test()`, `amc_add_bench()` and `amc_add_example()` reject names that
break the pattern at configure time.

## Rules for every component

- **Packaging: STATIC + `POSITION_INDEPENDENT_CODE` + default symbol visibility.**
  AntiMatterFusion embeds Common's static libraries PRIVATE into several shared
  libraries and relies on ELF symbol interposition to end up with exactly one instance
  per process (one logger, one worker thread). Never add `-fvisibility=hidden`,
  `-Bsymbolic`, version scripts or a SHARED build for a component without redesigning
  that. It is also why the naming convention is strict: every external-linkage symbol is
  global across the whole process.
- **Layering**: a component depends only on libc, pthreads and components listed before
  it in `AMC_COMPONENTS`. No cycles.
- **Language**: pure C, `-std=gnu11` (C11 + GNU extensions). Public headers are usable
  from C and C++; the C++ package smoke test keeps that true.
- **Warnings**: libraries build with `-Wall -Wextra -Wformat=2 -Werror`; tests and
  examples with `-Wall -Wextra`.
- **Fail fast**: mistakes are errors with a precise diagnostic, never silent defaults —
  in the code (config errors fail init) and in the build (a misnamed test or an unknown
  `COMPONENTS` name fails configure).
- **Tests are the user manual**: each component's tests are chapters meant to be read
  in order — a narrative header comment per file, one behavior per test.
- **Versioning**: `project(VERSION)` in the top-level `CMakeLists.txt` is the only copy;
  `AmcCommonVersion.h` is generated from it. 0.x with `SameMinorVersion`: a breaking
  change in any component bumps the minor version.

## Workflow

Propose and agree before moving to a new phase; questions are numbered so David can
answer tersely. Changes land through PRs with CI green on all six legs ({ubuntu gcc-13,
macos-14} × {Release, ASan/UBSan, TSan}).

## Current state

Components: `logger` (AmcLogger, `components/logger/`) — migrated from the standalone
AntiMatterLogger repository (agreed 2026-10-26; clean structure over legacy
compatibility). Its history is imported with tags `logger-v0.1.0`…`logger-v0.4.0`, and it
follows this convention in full (`components/logger/docs/Architecture.md` §14.19).

Remaining migration steps:

1. Switch downstream over: AntiMatterFusion (`find_package(AntiMatterCommon 0.5
   COMPONENTS Logger)`, `scripts/build.sh`, `docker/Dockerfile.{kit,model}`, the
   `AMC_LOGGER_JSON` / `AMC_LOGGER_KV_*` call sites) and AntiMatterDagger
   (`scripts/1_envbuild/build.sh`).
2. Archive the AntiMatterLogger repository.
