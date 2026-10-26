# The executable user manual: chapters 01..10, meant to be read in order. Each
# links amc_logger_testing, the library built with the test seam
# (amc_logger_internal_test_reset and a clock hook).

foreach(chapter
        AmcLoggerTest01_QuickStart
        AmcLoggerTest02_LogFormat
        AmcLoggerTest03_Levels
        AmcLoggerTest04_Configuration
        AmcLoggerTest05_FileSink
        AmcLoggerTest06_Lifecycle
        AmcLoggerTest07_AsyncAndReliability
        AmcLoggerTest08_CompileTimeStrip
        AmcLoggerTest09_MultiThreaded
        AmcLoggerTest10_JsonPayload)
    amc_add_test(logger ${chapter}
                 SOURCES AmcLoggerTestHelperModule.c
                 LIBRARIES amc_logger_testing)
endforeach()

# chapter 08 demonstrates compile-time stripping: same sources, extra flag
target_compile_definitions(AmcLoggerTest08_CompileTimeStrip
    PRIVATE AMC_LOGGER_ACTIVE_LEVEL=AMC_LOGGER_LEVEL_WARN)
