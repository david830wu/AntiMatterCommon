cmake_minimum_required(VERSION 3.27)

project(AntiMatterCommonApps
        VERSION 0.1
        DESCRIPTION "AntiMatterCommon apps"
        LANGUAGES C
)

# list and compile executable
list(APPEND target_apps "demo")

if ("demo" IN_LIST target_apps)
    add_subdirectory(demo)
endif()
