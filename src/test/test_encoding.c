#include "unity.h"

#include "../encoding.h"

void setUp(void) {}
void tearDown(void) {}

void test_encode_label(void) {
    struct Instruction i = {.type = INSTRUCTION_TYPE_LABEL,
                            .lbl_fields = {.name = "osef"}};
    uint16_t encoded;
    struct SymbolTable st;
    symbol_table_init(&st);
    TEST_ASSERT_NOT_EQUAL(0, encode(&i, &st, &encoded));
    symbol_table_destroy(&st);
}

void test_encode_a(void) {
    struct Instruction i;
    struct SymbolTable st;
    symbol_table_init(&st);

    // raw address
    i = (struct Instruction){.type = INSTRUCTION_TYPE_A,
                             .a_fields = {.resolved = true, .address = 4224}};
    TEST_ASSERT_EQUAL(4224, encode_a(&i, &st));

    // variables
    i = (struct Instruction){.type = INSTRUCTION_TYPE_A,
                             .a_fields = {.resolved = false, .symbol = "var1"}};
    TEST_ASSERT_EQUAL(16, encode_a(&i, &st));

    i = (struct Instruction){.type = INSTRUCTION_TYPE_A,
                             .a_fields = {.resolved = false, .symbol = "var2"}};
    TEST_ASSERT_EQUAL(17, encode_a(&i, &st));

    i = (struct Instruction){.type = INSTRUCTION_TYPE_A,
                             .a_fields = {.resolved = false, .symbol = "var1"}};
    TEST_ASSERT_EQUAL(16, encode_a(&i, &st));

    // builin
    i = (struct Instruction){.type = INSTRUCTION_TYPE_A,
                             .a_fields = {.resolved = false, .symbol = "R10"}};
    TEST_ASSERT_EQUAL(10, encode_a(&i, &st));

    i = (struct Instruction){
        .type = INSTRUCTION_TYPE_A,
        .a_fields = {.resolved = false, .symbol = "SCREEN"}};
    TEST_ASSERT_EQUAL(16384, encode_a(&i, &st));

    // label
    symbol_table_add(&st, "symbol1", 1234);
    i = (struct Instruction){
        .type = INSTRUCTION_TYPE_A,
        .a_fields = {.resolved = false, .symbol = "symbol1"}};
    TEST_ASSERT_EQUAL(1234, encode_a(&i, &st));

    symbol_table_destroy(&st);
}

void test_encode_c(void) {
    struct Instruction i;

    i = (struct Instruction){.type = INSTRUCTION_TYPE_C,
                             .c_fields = {.comp = COMP_0}};
    TEST_ASSERT_EQUAL(0b1110101010000000, encode_c(&i));

    i = (struct Instruction){.type = INSTRUCTION_TYPE_C,
                             .c_fields = {.comp = COMP_0, .dest = {.A = 1}}};
    TEST_ASSERT_EQUAL(0b1110101010100000, encode_c(&i));

    i = (struct Instruction){.type = INSTRUCTION_TYPE_C,
                             .c_fields = {.comp = COMP_0, .jump = JUMP_JMP}};
    TEST_ASSERT_EQUAL(0b1110101010000111, encode_c(&i));

    i = (struct Instruction){
        .type = INSTRUCTION_TYPE_C,
        .c_fields = {.comp = COMP_0, .dest = {.A = 1}, .jump = JUMP_JMP}};
    TEST_ASSERT_EQUAL(0b1110101010100111, encode_c(&i));

    i = (struct Instruction){.type = INSTRUCTION_TYPE_C,
                             .c_fields = {.comp = COMP_D_AND_A,
                                          .dest = {.A = 1, .D = 1},
                                          .jump = JUMP_JGE}};
    TEST_ASSERT_EQUAL(0b1110000000110011, encode_c(&i));

    i = (struct Instruction){.type = INSTRUCTION_TYPE_C,
                             .c_fields = {.comp = COMP_M_MINUS_1,
                                          .dest = {.M = 1, .D = 1},
                                          .jump = JUMP_NULL}};
    TEST_ASSERT_EQUAL(0b1111110010011000, encode_c(&i));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_encode_label);
    RUN_TEST(test_encode_a);
    RUN_TEST(test_encode_c);
    return UNITY_END();
}
