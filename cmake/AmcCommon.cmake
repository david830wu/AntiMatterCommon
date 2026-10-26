# Build helpers shared by every component. They enforce the naming convention in
# CLAUDE.md: component <comp> (lowercase directory name, e.g. "logger") is named
# Amc<Comp> everywhere ("AmcLogger"); its library is amc_<comp>, exported as
# AntiMatter::Amc<Comp>; its executables are Amc<Comp>Test<NN>_<Topic>,
# Amc<Comp>Bench<Name> and Amc<Comp>Example[<Name>]. A name that breaks the
# convention is a configure error.

# amc_component_name(<comp> <out_var>) — "logger" -> "Logger"
function(amc_component_name comp out_var)
    string(SUBSTRING "${comp}" 0 1 head)
    string(SUBSTRING "${comp}" 1 -1 tail)
    string(TOUPPER "${head}" head)
    set(${out_var} "${head}${tail}" PARENT_SCOPE)
endfunction()

# The language level, warnings and linkage of every component library.
# PIC + default visibility: AntiMatterFusion embeds Common's static libraries PRIVATE
# into several shared libraries and relies on ELF interposition for exactly one
# instance per process (CLAUDE.md, "Packaging").
function(_amc_library_defaults target)
    set_target_properties(${target} PROPERTIES C_STANDARD 11 C_EXTENSIONS ON
                                               POSITION_INDEPENDENT_CODE ON)
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wformat=2 -Werror)
    target_link_libraries(${target} PUBLIC Threads::Threads)
endfunction()

# amc_add_library(<comp> SOURCES <src>... [TEST_SEAM])
#   The component library amc_<comp> with its public headers in include/, exported
#   as AntiMatter::Amc<Comp> and installed with the package. TEST_SEAM also builds
#   amc_<comp>_testing — the same sources with AMC_<COMP>_TESTING defined — when
#   tests are enabled.
function(amc_add_library comp)
    cmake_parse_arguments(PARSE_ARGV 1 arg "TEST_SEAM" "" "SOURCES")
    amc_component_name(${comp} comp_name)
    string(TOUPPER "${comp}" comp_upper)
    set(target amc_${comp})
    set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)

    add_library(${target} STATIC ${arg_SOURCES})
    target_include_directories(${target} PUBLIC
        $<BUILD_INTERFACE:${include_dir}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    _amc_library_defaults(${target})
    set_target_properties(${target} PROPERTIES EXPORT_NAME Amc${comp_name})
    add_library(AntiMatter::Amc${comp_name} ALIAS ${target})

    install(TARGETS ${target} EXPORT AntiMatterCommonTargets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY ${include_dir}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    if(arg_TEST_SEAM AND AMC_BUILD_TESTS)
        add_library(${target}_testing STATIC ${arg_SOURCES})
        target_include_directories(${target}_testing PUBLIC ${include_dir})
        target_compile_definitions(${target}_testing PUBLIC AMC_${comp_upper}_TESTING)
        _amc_library_defaults(${target}_testing)
    endif()
endfunction()

# _amc_add_executable(<comp> <Test|Bench|Example> <name> [SOURCES ...] [LIBRARIES ...])
#   Checks <name> against the convention for its kind, then builds <name>.c or
#   <name>.cpp (when present in the current directory) plus SOURCES.
function(_amc_add_executable comp kind name)
    cmake_parse_arguments(PARSE_ARGV 3 arg "" "" "SOURCES;LIBRARIES")
    amc_component_name(${comp} comp_name)
    set(stem "Amc${comp_name}${kind}")
    if(kind STREQUAL "Test")
        set(pattern "^${stem}[0-9][0-9]_[A-Za-z0-9]+$")
        set(shape "${stem}<NN>_<Topic>")
    elseif(kind STREQUAL "Bench")
        set(pattern "^${stem}[A-Z][A-Za-z0-9]*$")
        set(shape "${stem}<Name>")
    else()
        set(pattern "^${stem}([A-Z][A-Za-z0-9]*)?$")
        set(shape "${stem}[<Name>]")
    endif()
    if(NOT name MATCHES "${pattern}")
        message(FATAL_ERROR "'${name}' of component '${comp}' must be named ${shape}")
    endif()

    set(sources ${arg_SOURCES})
    foreach(ext c cpp)
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${name}.${ext})
            list(PREPEND sources ${name}.${ext})
        endif()
    endforeach()
    if(NOT sources)
        message(FATAL_ERROR "'${name}': no ${name}.c / ${name}.cpp and no SOURCES")
    endif()

    add_executable(${name} ${sources})
    target_link_libraries(${name} PRIVATE ${arg_LIBRARIES})
    set_target_properties(${name} PROPERTIES C_STANDARD 11 C_EXTENSIONS ON)
    target_compile_options(${name} PRIVATE -Wall -Wextra)
endfunction()

# amc_add_test(<comp> <name> [SOURCES <extra>...] [LIBRARIES <lib>...])
#   A Unity test registered with ctest under the label <comp>, run in its own
#   directory build/run/<name>/ so temporary files never collide.
function(amc_add_test comp name)
    _amc_add_executable(${comp} Test ${name} ${ARGN})
    target_link_libraries(${name} PRIVATE amc_unity)

    set(run_dir ${PROJECT_BINARY_DIR}/run/${name})
    file(MAKE_DIRECTORY ${run_dir})
    add_test(NAME ${name} COMMAND ${name} WORKING_DIRECTORY ${run_dir})
    set_tests_properties(${name} PROPERTIES LABELS ${comp})
endfunction()

# amc_add_bench(<comp> <name> [SOURCES <extra>...] [LIBRARIES <lib>...])
#   A benchmark, always compiled -O2 whatever the build type.
function(amc_add_bench comp name)
    _amc_add_executable(${comp} Bench ${name} ${ARGN})
    target_compile_options(${name} PRIVATE -O2)
endfunction()

# amc_add_example(<comp> <name> [SOURCES <src>...] [LIBRARIES <lib>...])
#   An example program. Not installed.
function(amc_add_example comp name)
    _amc_add_executable(${comp} Example ${name} ${ARGN})
endfunction()
