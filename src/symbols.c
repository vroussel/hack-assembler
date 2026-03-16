#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "symbols.h"

void symbol_table_init(struct SymbolTable *st) {
    assert(st);

    memset(st->buckets, 0, BUCKET_COUNT * sizeof(struct Bucket *));
    st->next_var_address = 16;
}

void symbol_table_destroy(struct SymbolTable *st) {
    assert(st);

    for (int i = 0; i < 1024; i++) {
        struct Bucket *b = st->buckets[i];
        while (b) {
            struct Bucket *current = b;
            b = current->next;
            free(current);
        }
        b = NULL;
    }
}

enum SymbolTableError symbol_table_add(struct SymbolTable *st,
                                       const char *symbol_name,
                                       uint16_t address) {
    assert(st);
    assert(symbol_name);

    if (strlen(symbol_name) > SYMBOL_MAX_LEN) {
        return ST_SYMBOL_TOO_LONG;
    }

    int h = hash(symbol_name, BUCKET_COUNT);
    struct Bucket *b = st->buckets[h];
    while (b) {
        if (strcmp(b->symbol.name, symbol_name) == 0) {
            return ST_SYMBOL_ALREADY_EXISTS;
        }
        b = b->next;
    }

    struct Bucket *new_bucket = malloc(sizeof(struct Bucket));
    if (!new_bucket) {
        return ST_OUT_OF_MEMORY;
    }
    strncpy(new_bucket->symbol.name, symbol_name, SYMBOL_MAX_LEN);
    new_bucket->symbol.name[SYMBOL_MAX_LEN] = '\0';
    new_bucket->symbol.address = address;
    new_bucket->next = st->buckets[h];

    st->buckets[h] = new_bucket;

    return ST_OK;
}

enum SymbolTableError symbol_table_delete(struct SymbolTable *st,
                                          const char *symbol_name) {
    assert(st);
    assert(symbol_name);

    if (strlen(symbol_name) > SYMBOL_MAX_LEN) {
        return ST_SYMBOL_TOO_LONG;
    }

    int h = hash(symbol_name, BUCKET_COUNT);
    struct Bucket **pp = &(st->buckets[h]);
    while (*pp) {
        struct Bucket *b = *pp;
        if (strcmp(b->symbol.name, symbol_name) == 0) {
            *pp = b->next;
            free(b);
            break;
        }
        pp = &(b->next);
    }
    return ST_OK;
}

uint16_t symbol_table_get_or_create(struct SymbolTable *st,
                                    const char *symbol_name) {
    assert(st);
    assert(symbol_name);

    int h = hash(symbol_name, BUCKET_COUNT);
    struct Bucket *b = st->buckets[h];
    while (b) {
        if (strcmp(b->symbol.name, symbol_name) == 0) {
            return b->symbol.address;
        }
        b = b->next;
    }

    const uint16_t addr = st->next_var_address++;
    symbol_table_add(st, symbol_name, addr);
    return addr;
}
