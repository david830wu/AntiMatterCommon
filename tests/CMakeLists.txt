# Cross-component tests: the umbrella header, target and package as a whole.
# Each component's own suite lives in components/<comp>/tests/.

amc_add_test(common AmcCommonTest01_Umbrella LIBRARIES AntiMatter::AntiMatterCommon)
target_compile_definitions(AmcCommonTest01_Umbrella
    PRIVATE AMC_COMMON_EXPECTED_VERSION="${PROJECT_VERSION}")
