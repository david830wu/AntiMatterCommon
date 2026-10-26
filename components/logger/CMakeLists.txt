# AmcLogger — fast, small, pure-C logging (docs/Design.md, docs/Architecture.md).
# Built, tested and packaged by the AntiMatterCommon root build.

amc_add_library(logger TEST_SEAM SOURCES
    src/AmcLoggerCore.c
    src/AmcLoggerConfig.c
    src/AmcLoggerQueue.c
    src/AmcLoggerRender.c
    src/AmcLoggerSink.c)

if(AMC_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if(AMC_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(AMC_BUILD_BENCH)
    add_subdirectory(bench)
endif()
