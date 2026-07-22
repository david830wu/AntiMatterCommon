cmake_minimum_required(VERSION 3.27)

project({{ProjectName}}Lib
        VERSION {{Version}}
        DESCRIPTION "{{ProjectName}}"
        LANGUAGES C
)

add_library({{ProjectName}})
target_sources(
    {{ProjectName}}
PRIVATE
    {{ProjectName}}.c
)
set_target_properties({{ProjectName}} 
PROPERTIES PUBLIC_HEADER 
    "{{ProjectName}}.h;{{ProjectName}}Version.h"
)
target_include_directories(
    {{ProjectName}}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    {{ProjectName}}
PUBLIC
    Threads::Threads
)
add_library({{ProjectName}}::{{ProjectName}} ALIAS {{ProjectName}})

install(TARGETS {{ProjectName}} 
    DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

install(TARGETS {{ProjectName}}
    EXPORT {{ProjectName}}Targets
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)

set(ConfigPackageLocation lib/cmake/{{ProjectName}})
install(EXPORT {{ProjectName}}Targets
    FILE {{ProjectName}}Targets.cmake
    NAMESPACE {{ProjectName}}::
    DESTINATION ${ConfigPackageLocation}
)
install(FILES ${CMAKE_SOURCE_DIR}/cmake/{{ProjectName}}Config.cmake
    DESTINATION ${ConfigPackageLocation}
)
