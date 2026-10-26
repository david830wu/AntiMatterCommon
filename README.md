# AntiMatterCommon

Common C utilities for the AntiMatter project.

## TL;DR

AntiMatterCommon is not part of the current Dagger native build chain. To build
it directly, provide a CMake installation of Unity for its test target:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/unity-prefix
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build --prefix .local
./build/apps/demo/demo
```

## Detailed build and use

### Requirements

- CMake 3.27 or newer and a C11 compiler.
- POSIX threads, resolved through CMake's `Threads` package.
- The Unity C test framework, exporting `unity::framework` through a CMake
  package. Add its prefix to `CMAKE_PREFIX_PATH` if it is not installed in a
  standard location.

### Configure, build, and test

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/path/to/unity-prefix
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The build produces the `AntiMatterCommon` library, the
`AntiMatterCommonTest` CTest target, and a small `demo` executable:

```bash
./build/apps/demo/demo
```

Install it into a local or shared prefix:

```bash
cmake --install build --prefix /path/to/prefix
```

The install contains `AntiMatterCommon.h`, `AntiMatterCommonVersion.h`, the
library, the demo under `bin/`, and the CMake target export under
`lib/cmake/AntiMatterCommon/`.

### Consume from CMake

```cmake
find_package(Threads REQUIRED)
find_package(AntiMatterCommon REQUIRED)
target_link_libraries(my_target PRIVATE AntiMatterCommon::AntiMatterCommon)
```

Configure the consumer with `-DCMAKE_PREFIX_PATH=/path/to/prefix` when using a
non-system install. The current exported package metadata also resolves HDF5,
so HDF5 must be discoverable by CMake consumers. Including
`AntiMatterCommon.h` exposes the library version through
`ANTIMATTERCOMMON_VERSION`.
