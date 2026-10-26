# Cross-component example programs (each component keeps its own under
# components/<comp>/examples/). Built with -DAMC_BUILD_EXAMPLES=ON; not installed.

amc_add_example(common AmcCommonExample LIBRARIES AntiMatter::AntiMatterCommon)
