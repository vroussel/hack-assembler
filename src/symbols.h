#include <stdbool.h>
#include <stdint.h>

#define BUCKET_COUNT 1024
#define SYMBOL_MAX_LEN 128

struct SymbolTable;

struct Symbol {
    char name[SYMBOL_MAX_LEN + 1];
    uint16_t address;
};

struct Bucket {
    struct Symbol symbol;
    struct Bucket *next;
};

struct SymbolTable {
    struct Bucket *buckets[BUCKET_COUNT];
};

enum SymbolTableError {
    ST_OK,
    ST_SYMBOL_TOO_LONG,
    ST_SYMBOL_ALREADY_EXISTS,
    ST_OUT_OF_MEMORY,
};

void symbol_table_init(struct SymbolTable *st);
void symbol_table_destroy(struct SymbolTable *st);
enum SymbolTableError symbol_table_add(struct SymbolTable *st,
                                       const char *symbol_name,
                                       uint16_t address);
enum SymbolTableError symbol_table_delete(struct SymbolTable *st,
                                          const char *symbol_name);
const struct Symbol *symbol_table_get(const struct SymbolTable *st,
                                      const char *symbol_name);
