cmake_minimum_required(VERSION 3.27)

project({{ProjectName}}
        VERSION {{Version}}
        DESCRIPTION "{{Description}}"
        LANGUAGES C
)

enable_testing()

# find and configure required package
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(apps)
