# The docs/Design.md §9 walkthrough. Its sources model a user's program, so they keep
# user-style names (the module names main and LongTermExtractor are what
# config/logger.yaml configures). The config path is relative — run it from here:
#     cd components/logger && ../../build/bin/AmcLoggerExample

amc_add_example(logger AmcLoggerExample
                SOURCES main.c LongTermExtractor.c
                LIBRARIES amc_logger)
