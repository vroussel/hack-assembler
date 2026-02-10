#include "unity.h"

#include "../hash.h"
#include <limits.h>
#include <stdlib.h>
#include <time.h>

void setUp(void) { srand(time(0)); }

void tearDown(void) {
    // clean stuff up here
}

void random_var_name(int buf_size, char *out) {
    static const char valid_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz"
                                      "1234567890"
                                      "_";
    static const int valid_chars_count = sizeof(valid_chars) - 1;

    int len = (rand() % (buf_size - 1)) + 1;
    for (int i = 0; i < len; i++) {
        char c = valid_chars[rand() % valid_chars_count];
        out[i] = c;
    }

    out[len] = '\0';
}

void test_hash_function_distribution(void) {
    int buckets[1024] = {0};
    const int bucket_count = sizeof(buckets) / sizeof(buckets[0]);
    char tmp[128];
    static const int iterations = 100000;
    for (int i = 0; i < iterations; i++) {
        random_var_name(sizeof(tmp), tmp);
        int h = hash(tmp, bucket_count);
        buckets[h]++;
    }

    double expected_avg_bucket_size = (double)iterations / bucket_count;
    double tolerance = 0.5;
    for (int j = 0; j < bucket_count; j++) {
        int bucket_count = buckets[j];
        TEST_ASSERT_LESS_THAN(expected_avg_bucket_size * (1 + tolerance),
                              bucket_count);
        TEST_ASSERT_GREATER_THAN(expected_avg_bucket_size * (1 - tolerance),
                                 bucket_count);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hash_function_distribution);
    return UNITY_END();
}
