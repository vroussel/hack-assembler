#include "unity.h"

#include "../utils.h"

void setUp(void) {}

void tearDown(void) {}

void test_to_bin() {
    char buf[16];
    to_bin(16, buf);
    TEST_ASSERT_EQUAL_STRING("0000000000010000", buf);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_to_bin);
    return UNITY_END();
}
