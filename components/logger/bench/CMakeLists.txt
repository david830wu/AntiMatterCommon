# Benchmark harness (docs/Architecture.md §13). Pin two cores when running — the
# worker needs its own: taskset -c 2,3 ./build/bin/AmcLoggerBenchEnqueue

foreach(bench AmcLoggerBenchDisabled AmcLoggerBenchEnqueue AmcLoggerBenchThroughput)
    amc_add_bench(logger ${bench} LIBRARIES amc_logger)
endforeach()

option(AMC_LOGGER_BENCH_SPDLOG
       "Build the spdlog comparison benchmark (FetchContent, needs network)" OFF)
if(AMC_LOGGER_BENCH_SPDLOG)
    # C++ is confined to this one benchmark target; the library stays pure C
    enable_language(CXX)
    include(FetchContent)
    FetchContent_Declare(spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.tar.gz)
    FetchContent_MakeAvailable(spdlog)
    amc_add_bench(logger AmcLoggerBenchSpdlog LIBRARIES spdlog::spdlog)
    set_target_properties(AmcLoggerBenchSpdlog PROPERTIES CXX_STANDARD 17)
endif()
