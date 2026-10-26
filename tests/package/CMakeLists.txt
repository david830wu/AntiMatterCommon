# Package smoke test: consumes an INSTALLED AntiMatterCommon via find_package,
# proving the export set, package config, version file, and header install all
# work for a downstream project, from both C and C++. Not part of the main
# build — CI (and you) run:
#
#     cmake --install build --prefix /tmp/amc-prefix
#     cmake -S tests/package -B build-pkg -DCMAKE_PREFIX_PATH=/tmp/amc-prefix
#     cmake --build build-pkg
#     ./build-pkg/AmcCommonPackageSmoke && ./build-pkg/AmcCommonPackageSmokeCpp
#     ./build-pkg/AmcLoggerPackageSmoke && ./build-pkg/AmcLoggerPackageSmokeCpp

cmake_minimum_required(VERSION 3.27)
project(AmcPackageSmoke LANGUAGES C CXX)

find_package(AntiMatterCommon 0.5 REQUIRED COMPONENTS Logger)

add_executable(AmcCommonPackageSmoke AmcCommonPackageSmoke.c)
set_target_properties(AmcCommonPackageSmoke PROPERTIES C_STANDARD 11 C_EXTENSIONS ON)
target_compile_options(AmcCommonPackageSmoke PRIVATE -Wall -Wextra -Werror)
target_link_libraries(AmcCommonPackageSmoke PRIVATE AntiMatter::AntiMatterCommon)

add_executable(AmcCommonPackageSmokeCpp AmcCommonPackageSmokeCpp.cpp)
set_target_properties(AmcCommonPackageSmokeCpp PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_compile_options(AmcCommonPackageSmokeCpp PRIVATE -Wall -Wextra -Werror)
target_link_libraries(AmcCommonPackageSmokeCpp PRIVATE AntiMatter::AntiMatterCommon)

# ---- component: logger (AntiMatter::AmcLogger) ----
add_executable(AmcLoggerPackageSmoke AmcLoggerPackageSmoke.c)
set_target_properties(AmcLoggerPackageSmoke PROPERTIES C_STANDARD 11 C_EXTENSIONS ON)
target_compile_options(AmcLoggerPackageSmoke PRIVATE -Wall -Wextra -Werror)
target_link_libraries(AmcLoggerPackageSmoke PRIVATE AntiMatter::AmcLogger)

add_executable(AmcLoggerPackageSmokeCpp AmcLoggerPackageSmokeCpp.cpp)
set_target_properties(AmcLoggerPackageSmokeCpp PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)
target_compile_options(AmcLoggerPackageSmokeCpp PRIVATE -Wall -Wextra -Werror)
target_link_libraries(AmcLoggerPackageSmokeCpp PRIVATE AntiMatter::AmcLogger)
