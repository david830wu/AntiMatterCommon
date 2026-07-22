cmake_minimum_required(VERSION 3.27)

project(AntiMatterCommonLib
        VERSION 0.1
        DESCRIPTION "AntiMatterCommon"
        LANGUAGES C
)

add_library(AntiMatterCommon)
target_sources(
    AntiMatterCommon
PRIVATE
    AntiMatterCommon.c
)
set_target_properties(AntiMatterCommon 
PROPERTIES PUBLIC_HEADER 
    "AntiMatterCommon.h;AntiMatterCommonVersion.h"
)
target_include_directories(
    AntiMatterCommon
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    AntiMatterCommon
PUBLIC
    Threads::Threads
)
add_library(AntiMatterCommon::AntiMatterCommon ALIAS AntiMatterCommon)

install(TARGETS AntiMatterCommon 
    DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

install(TARGETS AntiMatterCommon
    EXPORT AntiMatterCommonTargets
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)

set(ConfigPackageLocation lib/cmake/AntiMatterCommon)
install(EXPORT AntiMatterCommonTargets
    FILE AntiMatterCommonTargets.cmake
    NAMESPACE AntiMatterCommon::
    DESTINATION ${ConfigPackageLocation}
)
install(FILES ${CMAKE_SOURCE_DIR}/cmake/AntiMatterCommonConfig.cmake
    DESTINATION ${ConfigPackageLocation}
)
