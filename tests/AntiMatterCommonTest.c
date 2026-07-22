/* AntiMatterCommonTest.cpp
* 
* Author: Wentao Wu
* Created: 20260722 
* Version: 0.1
*/

#include "AntiMatterCommon.h"

#include <unity.h>
#include <stdint.h>

#define REWARD_TOL 1e-6

void setUp(void) {
}

void tearDown(void) {
}

void test_commander_int64(void) {
    int64_t reward, expected_reward;
    expected_reward = 0;
    reward = 0;
    TEST_ASSERT_EQUAL_INT64(expected_reward, reward);
}

void test_commander_float(void) {
    double reward, expected_reward;
    expected_reward = 0.0;
    reward = 0.0;
    TEST_ASSERT_FLOAT_WITHIN(REWARD_TOL, expected_reward, reward);
}

void test_commander_bool(void) {
    int reward, threshold;
    threshold = 0;
    reward = 1;
    TEST_ASSERT_TRUE(reward > threshold);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_commander_int64);
    RUN_TEST(test_commander_float);
    RUN_TEST(test_commander_bool);
    return UNITY_END();
}
