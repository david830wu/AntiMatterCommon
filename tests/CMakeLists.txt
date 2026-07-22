cmake_minimum_required(VERSION 3.27)

project(AntiMatterCommonTest
        VERSION 0.1
        DESCRIPTION "AntiMatterCommon test cases"
        LANGUAGES C
)

enable_testing()

find_package(unity REQUIRED)

# list and compile executable
list(APPEND target_tests "AntiMatterCommonTest")

if ("AntiMatterCommonTest" IN_LIST target_tests)
    set(test_name "AntiMatterCommonTest")
    add_executable(${test_name})
    target_sources(${test_name} PUBLIC ${test_name}.c)
    target_link_libraries(${test_name} PUBLIC unity::framework AntiMatterCommon::AntiMatterCommon)
    add_test("${test_name}" ${test_name})
endif()
