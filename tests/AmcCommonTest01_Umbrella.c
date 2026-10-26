/*
 * ============================================================================
 *  CHAPTER 01 — THE UMBRELLA
 * ============================================================================
 *  AntiMatterCommon ships every component in one CMake package. A consumer
 *  that wants all of them writes
 *
 *      find_package(AntiMatterCommon 0.5 REQUIRED)
 *      target_link_libraries(app PRIVATE AntiMatter::AntiMatterCommon)
 *
 *  and includes "AmcCommon.h": every component's public API plus the version
 *  of the package it was built against. Linking a single component,
 *  AntiMatter::Amc<Comp>, and including its header is the narrower form.
 * ============================================================================
 */

#include "AmcCommon.h"

#include <stdio.h>
#include <unity.h>

void setUp(void)    {}
void tearDown(void) {}

/* AMC_COMMON_VERSION is generated from project(VERSION) in CMakeLists.txt, so
 * there is no second copy of the version to forget when bumping it. */
static void test_VersionMatchesTheBuild(void)
{
    TEST_ASSERT_EQUAL_STRING(AMC_COMMON_EXPECTED_VERSION, AMC_COMMON_VERSION);
}

/* The numeric parts exist for compile-time checks (#if AMC_COMMON_VERSION_MINOR
 * >= ...) and always agree with the string. */
static void test_NumericPartsAgreeWithTheString(void)
{
    char composed[32];
    snprintf(composed, sizeof(composed), "%d.%d.%d", AMC_COMMON_VERSION_MAJOR,
             AMC_COMMON_VERSION_MINOR, AMC_COMMON_VERSION_PATCH);
    TEST_ASSERT_EQUAL_STRING(AMC_COMMON_VERSION, composed);
}

/* One include and one link bring every component: here the logger, whose
 * statistics are readable at any time — before init they are all zero. */
static void test_UmbrellaBringsEveryComponent(void)
{
    struct amc_logger_stats stats;
    amc_logger_get_stats(&stats);
    TEST_ASSERT_EQUAL_UINT64(0, stats.enqueued);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_VersionMatchesTheBuild);
    RUN_TEST(test_NumericPartsAgreeWithTheString);
    RUN_TEST(test_UmbrellaBringsEveryComponent);
    return UNITY_END();
}
