# Cross-component example programs (each component keeps its own under
# components/<comp>/examples/). Built with -DAMC_BUILD_EXAMPLES=ON; not installed.

add_executable(AmcCommonExample AmcCommonExample.c)
target_link_libraries(AmcCommonExample PRIVATE AntiMatter::AntiMatterCommon)
set_target_properties(AmcCommonExample PROPERTIES C_STANDARD 11 C_EXTENSIONS ON)
target_compile_options(AmcCommonExample PRIVATE -Wall -Wextra)
