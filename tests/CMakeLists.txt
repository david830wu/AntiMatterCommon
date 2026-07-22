cmake_minimum_required(VERSION 3.27)

project({{ProjectName}}Test
        VERSION {{Version}}
        DESCRIPTION "{{ProjectName}} test cases"
        LANGUAGES C
)

enable_testing()

find_package(unity REQUIRED)

# list and compile executable
list(APPEND target_tests "{{ProjectName}}Test")

if ("{{ProjectName}}Test" IN_LIST target_tests)
    set(test_name "{{ProjectName}}Test")
    add_executable(${test_name})
    target_sources(${test_name} PUBLIC ${test_name}.c)
    target_link_libraries(${test_name} PUBLIC unity::framework {{ProjectName}}::{{ProjectName}})
    add_test("${test_name}" ${test_name})
endif()
