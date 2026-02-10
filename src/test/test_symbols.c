#include "unity.h"

#include "../symbols.h"

void setUp(void) {}

void tearDown(void) {}

void test_get_existing_symbol(void) {
    struct SymbolTable st;
    symbol_table_init(&st);
    enum SymbolTableError result;

    result = symbol_table_add(&st, "super", 0x42);
    TEST_ASSERT_EQUAL(ST_OK, result);

    const struct Symbol *s = symbol_table_get(&st, "super");
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("super", s->name);
    TEST_ASSERT_EQUAL(0x42, s->address);

    symbol_table_destroy(&st);
}

void test_get_non_existing_symbol(void) {
    struct SymbolTable st;
    symbol_table_init(&st);

    const struct Symbol *s = symbol_table_get(&st, "nope");
    TEST_ASSERT_NULL(s);

    symbol_table_destroy(&st);
}

void test_add_symbol_twice(void) {
    struct SymbolTable st;
    symbol_table_init(&st);
    enum SymbolTableError result;

    result = symbol_table_add(&st, "dup", 0x42);
    TEST_ASSERT_EQUAL(ST_OK, result);

    result = symbol_table_add(&st, "dup", 0x43);
    TEST_ASSERT_EQUAL(ST_SYMBOL_ALREADY_EXISTS, result);

    const struct Symbol *s = symbol_table_get(&st, "dup");
    TEST_ASSERT_NOT_NULL(s);
    TEST_ASSERT_EQUAL_STRING("dup", s->name);
    TEST_ASSERT_EQUAL(0x42, s->address);

    symbol_table_destroy(&st);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_get_existing_symbol);
    RUN_TEST(test_get_non_existing_symbol);
    RUN_TEST(test_add_symbol_twice);
    return UNITY_END();
}
