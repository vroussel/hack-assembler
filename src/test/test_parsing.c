#include "unity.h"

#include "../parsing.c"
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

void test_parse_line_empty(void) {
    enum ParseLineResult ret;
    struct Instruction instr;
    struct ParseLineError err;

    // really empty
    ret = parse_line("", &instr, &err);
    TEST_ASSERT_EQUAL(PLR_EMPTY, ret);

    // spaces
    ret = parse_line("   ", &instr, &err);
    TEST_ASSERT_EQUAL(PLR_EMPTY, ret);

    // spaces + tabs
    ret = parse_line("		 ", &instr, &err);
    TEST_ASSERT_EQUAL(PLR_EMPTY, ret);
}

void test_parse_line_with_comments(void) {
    enum ParseLineResult ret;
    struct Instruction instr;
    struct ParseLineError err;

    // comment
    ret = parse_line(" // cool", &instr, &err);
    TEST_ASSERT_EQUAL(PLR_EMPTY, ret);

    // comment with stuff before
    ret = parse_line("stuff // cool", &instr, &err);
    TEST_ASSERT_NOT_EQUAL(PLR_EMPTY, ret);
}

static FILE *str_to_stream(char *str) {
    return fmemopen(str, strlen(str), "r");
}

void test_fgets2(void) {
    // Simulate a max line length of 10
    char buf[10 + 2];
    bool truncated;
    FILE *f;

    // less than 10 chars
    f = str_to_stream("01234");
    fgets2(buf, sizeof(buf), f, &truncated);
    TEST_ASSERT_EQUAL_STRING("01234", buf);
    TEST_ASSERT_FALSE(truncated);
    fclose(f);

    // 10 chars then EOF
    f = str_to_stream("0123456789");
    fgets2(buf, sizeof(buf), f, &truncated);
    TEST_ASSERT_EQUAL_STRING("0123456789", buf);
    TEST_ASSERT_FALSE(truncated);
    fclose(f);

    // 10 chars then \n
    f = str_to_stream("0123456789\n");
    fgets2(buf, sizeof(buf), f, &truncated);
    TEST_ASSERT_EQUAL_STRING("0123456789\n", buf);
    TEST_ASSERT_FALSE(truncated);
    fclose(f);

    // 11 chars then EOF
    f = str_to_stream("01234567891");
    fgets2(buf, sizeof(buf), f, &truncated);
    TEST_ASSERT_EQUAL_STRING("01234567891", buf);
    TEST_ASSERT_TRUE(truncated);
    fclose(f);

    // 11 chars then \n
    f = str_to_stream("01234567891\n");
    fgets2(buf, sizeof(buf), f, &truncated);
    TEST_ASSERT_EQUAL_STRING("01234567891", buf);
    TEST_ASSERT_TRUE(truncated);
    fclose(f);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_line_empty);
    RUN_TEST(test_parse_line_with_comments);
    RUN_TEST(test_fgets2);
    return UNITY_END();
}
