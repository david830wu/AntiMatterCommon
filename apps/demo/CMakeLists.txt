cmake_minimum_required(VERSION 3.27)

project(AntiMatterCommonApps
        VERSION 0.1
        DESCRIPTION "AntiMatterCommon apps"
        LANGUAGES C
)

add_executable(demo)
target_sources(
    demo
PUBLIC
    main.c
)
target_include_directories(
    demo
PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/apps/demo>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    demo
PRIVATE
    AntiMatterCommon::AntiMatterCommon
    Threads::Threads
)
install(TARGETS demo DESTINATION bin)
