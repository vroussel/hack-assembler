#include "unity.h"

#include "../symbols.h"

void setUp(void) {}

void tearDown(void) {}

void test_get_existing_symbol(void) {
    struct SymbolTable st;
    symbol_table_init(&st);
    enum SymbolTableError result;

    result = symbol_table_add(&st, "super", 42);
    TEST_ASSERT_EQUAL(ST_OK, result);

    TEST_ASSERT_EQUAL(42, symbol_table_get_or_create(&st, "super"));

    symbol_table_destroy(&st);
}

void test_get_non_existing_symbol(void) {
    struct SymbolTable st;
    symbol_table_init(&st);

    TEST_ASSERT_EQUAL(16, symbol_table_get_or_create(&st, "var1"));
    TEST_ASSERT_EQUAL(17, symbol_table_get_or_create(&st, "var2"));
    TEST_ASSERT_EQUAL(18, symbol_table_get_or_create(&st, "var3"));

    symbol_table_destroy(&st);
}

void test_add_symbol_twice(void) {
    struct SymbolTable st;
    symbol_table_init(&st);
    enum SymbolTableError result;

    result = symbol_table_add(&st, "dup", 42);
    TEST_ASSERT_EQUAL(ST_OK, result);

    result = symbol_table_add(&st, "dup", 0x43);
    TEST_ASSERT_EQUAL(ST_SYMBOL_ALREADY_EXISTS, result);

    TEST_ASSERT_EQUAL(42, symbol_table_get_or_create(&st, "dup"));

    symbol_table_destroy(&st);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_get_existing_symbol);
    RUN_TEST(test_get_non_existing_symbol);
    RUN_TEST(test_add_symbol_twice);
    return UNITY_END();
}
