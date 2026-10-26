# Build helpers shared by every component. They enforce the naming convention in
# CLAUDE.md: component <comp> (lowercase directory name, e.g. "logger") is named
# Amc<Comp> everywhere ("AmcLogger"), and its tests are Amc<Comp>Test<NN>_<Topic>,
# one name for the source file, the CMake target and the ctest test.

# amc_component_name(<comp> <out_var>) — "logger" -> "Logger"
function(amc_component_name comp out_var)
    string(SUBSTRING "${comp}" 0 1 head)
    string(SUBSTRING "${comp}" 1 -1 tail)
    string(TOUPPER "${head}" head)
    set(${out_var} "${head}${tail}" PARENT_SCOPE)
endfunction()

# amc_add_test(<comp> <name> [SOURCES <extra>...] [LIBRARIES <lib>...])
#   Builds <name>.c (plus any extra SOURCES) against Unity, registers it with
#   ctest under the label <comp>, and runs it in its own directory
#   build/run/<name>/ so temporary files never collide. A name that breaks the
#   convention is a configure error.
function(amc_add_test comp name)
    cmake_parse_arguments(PARSE_ARGV 2 arg "" "" "SOURCES;LIBRARIES")
    amc_component_name(${comp} comp_name)
    if(NOT name MATCHES "^Amc${comp_name}Test[0-9][0-9]_[A-Za-z0-9]+$")
        message(FATAL_ERROR "test '${name}' of component '${comp}' must be named "
                            "Amc${comp_name}Test<NN>_<Topic>")
    endif()

    add_executable(${name} ${name}.c ${arg_SOURCES})
    target_link_libraries(${name} PRIVATE ${arg_LIBRARIES} amc_unity)
    set_target_properties(${name} PROPERTIES C_STANDARD 11 C_EXTENSIONS ON)
    target_compile_options(${name} PRIVATE -Wall -Wextra)

    set(run_dir ${PROJECT_BINARY_DIR}/run/${name})
    file(MAKE_DIRECTORY ${run_dir})
    add_test(NAME ${name} COMMAND ${name} WORKING_DIRECTORY ${run_dir})
    set_tests_properties(${name} PROPERTIES LABELS ${comp})
endfunction()
