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

void test_parse_label(void) {
    struct Instruction instr;
    struct ParseLineError err;
    int ret;

    ret = parse_label("()", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(2, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));

    ret = parse_label("(missingclose", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(14, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));

    ret = parse_label("(invalich@r", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(10, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));

    ret = parse_label("(good)", &instr, &err);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(INSTRUCTION_TYPE_LABEL, instr.type);
    TEST_ASSERT_EQUAL_STRING("good", instr.fields.lbl_fields.name);

    ret = parse_label(
        "(toooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
        "oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooolong)",
        &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(MAX_LABEL_LENGTH + 2, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_line_empty);
    RUN_TEST(test_parse_line_with_comments);
    RUN_TEST(test_fgets2);
    RUN_TEST(test_parse_label);
    return UNITY_END();
}
