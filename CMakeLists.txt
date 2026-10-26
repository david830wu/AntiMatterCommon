cmake_minimum_required(VERSION 3.27)
project(AntiMatterCommon
        VERSION 0.5.0
        DESCRIPTION "Common C components for the AntiMatter project"
        LANGUAGES C)

option(AMC_BUILD_TESTS "Build every component's test suite" ${PROJECT_IS_TOP_LEVEL})
option(AMC_BUILD_EXAMPLES "Build the example programs" OFF)
option(AMC_BUILD_BENCH "Build the benchmark harnesses" OFF)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/AmcCommon.cmake)
find_package(Threads REQUIRED)

# every executable lands in build/bin/; the naming convention keeps them unique
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if(AMC_BUILD_TESTS)
    enable_testing()
    add_library(amc_unity STATIC third_party/unity/unity.c)
    target_include_directories(amc_unity PUBLIC third_party/unity)
endif()

# ---- components: components/<comp>/, built, tested and packaged together ----
# Lowercase directory names in dependency order. Component <comp> provides the
# library target amc_<comp> (AntiMatter::Amc<Comp>), see CLAUDE.md.
set(AMC_COMPONENTS)
foreach(comp IN LISTS AMC_COMPONENTS)
    add_subdirectory(components/${comp})
endforeach()

# ---- the umbrella: AntiMatter::AntiMatterCommon = AmcCommon.h + every component ----
configure_file(cmake/AmcCommonVersion.h.in include/AmcCommonVersion.h @ONLY)

add_library(amc_common INTERFACE)
target_include_directories(amc_common INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
set_target_properties(amc_common PROPERTIES EXPORT_NAME AntiMatterCommon)
add_library(AntiMatter::AntiMatterCommon ALIAS amc_common)

set(AMC_COMPONENT_NAMES)   # the names find_package(... COMPONENTS) accepts
foreach(comp IN LISTS AMC_COMPONENTS)
    amc_component_name(${comp} name)
    list(APPEND AMC_COMPONENT_NAMES ${name})
    target_link_libraries(amc_common INTERFACE amc_${comp})
endforeach()

# ---- installation and CMake package export ----
# Downstream usage, see tests/package (run by CI against a fresh install tree):
#     find_package(AntiMatterCommon 0.5 REQUIRED [COMPONENTS <Comp>...])
#     target_link_libraries(app PRIVATE AntiMatter::AntiMatterCommon)  # or AntiMatter::Amc<Comp>
install(TARGETS amc_common EXPORT AntiMatterCommonTargets)
install(FILES include/AmcCommon.h ${PROJECT_BINARY_DIR}/include/AmcCommonVersion.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(AMC_PACKAGE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/AntiMatterCommon)
install(EXPORT AntiMatterCommonTargets
        FILE AntiMatterCommonTargets.cmake
        NAMESPACE AntiMatter::
        DESTINATION ${AMC_PACKAGE_DIR})

configure_package_config_file(cmake/AntiMatterCommonConfig.cmake.in
    ${PROJECT_BINARY_DIR}/AntiMatterCommonConfig.cmake
    INSTALL_DESTINATION ${AMC_PACKAGE_DIR})
# 0.x versioning: minor bumps may break API, so only same-minor satisfies
write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/AntiMatterCommonConfigVersion.cmake
    COMPATIBILITY SameMinorVersion)
install(FILES
    ${PROJECT_BINARY_DIR}/AntiMatterCommonConfig.cmake
    ${PROJECT_BINARY_DIR}/AntiMatterCommonConfigVersion.cmake
    DESTINATION ${AMC_PACKAGE_DIR})

# ---- cross-component tests and examples ----
if(AMC_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if(AMC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
