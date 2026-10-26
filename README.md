# AntiMatterCommon

[![CI](https://github.com/david830wu/AntiMatterCommon/actions/workflows/ci.yml/badge.svg)](https://github.com/david830wu/AntiMatterCommon/actions/workflows/ci.yml)

Common C components for the AntiMatter project, built, tested and shipped together as
one CMake package with one version.

## TL;DR

```bash
./scripts/build_component.sh
ctest --test-dir build --output-on-failure
```

This builds every component and its tests, then installs the headers, libraries and
CMake package into `.local/`. Consumers link everything through one target:

```cmake
find_package(AntiMatterCommon 0.5 REQUIRED)
target_link_libraries(app PRIVATE AntiMatter::AntiMatterCommon)
```

```c
#include "AmcCommon.h"   /* every component's API + AMC_COMMON_VERSION */
```

## Components

| Component | Target | Header | Purpose |
| --- | --- | --- | --- |
| *(none yet — AntiMatterLogger moves in as `components/logger`)* | | | |

Each component lives in `components/<comp>/` with its own headers, sources, tests
(written as a user manual) and design docs. The naming convention and the rules every
component follows are in [CLAUDE.md](CLAUDE.md).

## Detailed build and use

Requires CMake ≥ 3.27 and gcc-13 (Linux) or AppleClang ≥ 14 (macOS); POSIX threads.
The test framework, [Unity](https://github.com/ThrowTheSwitch/Unity), is vendored in
`third_party/unity` (MIT) — nothing to install.

### Repository build

`scripts/build_component.sh` is the build interface for standalone use and for parent
stacks. Its defaults are `build/` for CMake state, `.local/` for the install, a
`Release` build, and tests enabled.

| Variable | Default | Purpose |
| --- | --- | --- |
| `AM_PREFIX` | `.local` | Install prefix |
| `AM_BUILD_DIR` | `build` | CMake build directory |
| `AM_BUILD_TESTS` | `ON` | Build the test suites |
| `AM_CMAKE_PREFIX_PATH` | empty | Additional semicolon-separated CMake package prefixes |
| `BUILD_TYPE` | `Release` | CMake build type |
| `NUM_THREADS` | host CPU count | Parallel build jobs |
| `AM_CLEAN_BUILD` | `false` | Set to `true` to configure with `cmake --fresh` |

For example, a parent stack selects one shared prefix with
`AM_PREFIX=/path/to/prefix ./scripts/build_component.sh`.

The equivalent manual developer build, without installing:

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure   # or -L <comp> for one component
```

| Option | Default | Purpose |
| --- | --- | --- |
| `AMC_BUILD_TESTS` | `ON` when top-level | Build every component's tests |
| `AMC_BUILD_EXAMPLES` | `OFF` | Build the example programs |
| `AMC_BUILD_BENCH` | `OFF` | Build the benchmark harnesses |

Executables land in `build/bin/`, e.g. `./build/bin/AmcCommonExample`.

### Consume from CMake

```cmake
find_package(AntiMatterCommon 0.5 REQUIRED)                    # everything
find_package(AntiMatterCommon 0.5 REQUIRED COMPONENTS <Comp>)  # also checks <Comp> exists

target_link_libraries(app PRIVATE AntiMatter::AntiMatterCommon)  # all components
target_link_libraries(app PRIVATE AntiMatter::Amc<Comp>)         # or just one
```

Configure an installed consumer with `-DCMAKE_PREFIX_PATH=/path/to/prefix`;
`add_subdirectory(AntiMatterCommon)` provides the same targets. Versions follow 0.x
`SameMinorVersion` rules: only the same minor version satisfies a request.
`tests/package/` is the C and C++ consumer CI builds against a fresh install.

## License

MIT — see [LICENSE](LICENSE).
