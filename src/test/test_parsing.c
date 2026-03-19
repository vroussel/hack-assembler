#include "unity.h"

#include "../parsing/parsing.h"

#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static FILE *str_to_stream(char *str) {
    return fmemopen(str, strlen(str), "r");
}

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
    TEST_ASSERT_EQUAL_STRING("good", instr.lbl_fields.name);

    ret = parse_label("(good)with_trailing_stuff", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(7, err.column);
    TEST_ASSERT_EQUAL_STRING("good", instr.lbl_fields.name);

    ret = parse_label(
        "(toooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
        "oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooolong)",
        &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(MAX_LABEL_LENGTH + 2, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));
}

void test_parse_a_instruction(void) {
    struct Instruction instr;
    struct ParseLineError err;
    int ret;

    ret = parse_a_instruction("@", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(2, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));

    ret = parse_a_instruction("@bad-label", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(5, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));

    ret = parse_a_instruction("@good_label", &instr, &err);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(INSTRUCTION_TYPE_A, instr.type);
    TEST_ASSERT_FALSE(instr.a_fields.resolved);
    TEST_ASSERT_EQUAL_STRING("good_label", instr.a_fields.symbol);

    ret = parse_a_instruction("@123", &instr, &err);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(INSTRUCTION_TYPE_A, instr.type);
    TEST_ASSERT_TRUE(instr.a_fields.resolved);
    TEST_ASSERT_EQUAL(123, instr.a_fields.address);

    ret = parse_a_instruction(
        "@toooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
        "oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooolong",
        &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(MAX_ADDR_LENGTH + 2, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));
}

void test_parse_c_instruction(void) {
    struct Instruction instr;
    struct ParseLineError err;
    int ret;

    // comp
    ret = parse_c_instruction("0", &instr, &err);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(INSTRUCTION_TYPE_C, instr.type);
    TEST_ASSERT_EQUAL(COMP_0, instr.c_fields.comp);
    TEST_ASSERT_EQUAL(0, instr.c_fields.dest.A);
    TEST_ASSERT_EQUAL(0, instr.c_fields.dest.D);
    TEST_ASSERT_EQUAL(0, instr.c_fields.dest.M);
    TEST_ASSERT_EQUAL(JUMP_NULL, instr.c_fields.jump);

    // dest + comp
    ret = parse_c_instruction("AM=!D", &instr, &err);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(INSTRUCTION_TYPE_C, instr.type);
    TEST_ASSERT_EQUAL(COMP_NOT_D, instr.c_fields.comp);
    TEST_ASSERT_EQUAL(1, instr.c_fields.dest.A);
    TEST_ASSERT_EQUAL(0, instr.c_fields.dest.D);
    TEST_ASSERT_EQUAL(1, instr.c_fields.dest.M);
    TEST_ASSERT_EQUAL(JUMP_NULL, instr.c_fields.jump);

    // dest + comp + jump
    ret = parse_c_instruction("MD=D&A;JMP", &instr, &err);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(INSTRUCTION_TYPE_C, instr.type);
    TEST_ASSERT_EQUAL(COMP_D_AND_A, instr.c_fields.comp);
    TEST_ASSERT_EQUAL(0, instr.c_fields.dest.A);
    TEST_ASSERT_EQUAL(1, instr.c_fields.dest.D);
    TEST_ASSERT_EQUAL(1, instr.c_fields.dest.M);
    TEST_ASSERT_EQUAL(JUMP_JMP, instr.c_fields.jump);

    // comp + jump
    ret = parse_c_instruction("A+1;JGE", &instr, &err);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(INSTRUCTION_TYPE_C, instr.type);
    TEST_ASSERT_EQUAL(COMP_A_PLUS_1, instr.c_fields.comp);
    TEST_ASSERT_EQUAL(0, instr.c_fields.dest.A);
    TEST_ASSERT_EQUAL(0, instr.c_fields.dest.D);
    TEST_ASSERT_EQUAL(0, instr.c_fields.dest.M);
    TEST_ASSERT_EQUAL(JUMP_JGE, instr.c_fields.jump);

    // extra ; at end end
    ret = parse_c_instruction("MD=D&A;JMP;", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(11, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));

    // Invalid dest
    ret = parse_c_instruction("OMD=D&A;JMP", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(1, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));

    // Invalid comp
    ret = parse_c_instruction("MD=D&D;JMP", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(4, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));

    // Invalid jump
    ret = parse_c_instruction("MD=D&A;JNP", &instr, &err);
    TEST_ASSERT_EQUAL(1, ret);
    TEST_ASSERT_EQUAL(8, err.column);
    TEST_ASSERT_GREATER_THAN(0, strlen(err.error_msg));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_line_empty);
    RUN_TEST(test_parse_line_with_comments);
    RUN_TEST(test_fgets2);
    RUN_TEST(test_parse_label);
    RUN_TEST(test_parse_a_instruction);
    RUN_TEST(test_parse_c_instruction);
    return UNITY_END();
}
