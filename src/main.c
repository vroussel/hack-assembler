#include <stdio.h>
#include <unistd.h>

#include "parsing/parsing.h"
#include "symbols.h"

void fill_symbol_table(void *data, const struct Instruction *instr,
                       int line_number) {
    struct SymbolTable *st = data;
    if (instruction_type(instr) != INSTRUCTION_TYPE_LABEL) {
        return;
    }
    symbol_table_add(st, instruction_label_name(instr), line_number + 1);
}

int translate(FILE *input, FILE *output) {
    int ret = 0;
    struct SymbolTable st;
    symbol_table_init(&st);

    ret =
        process_file(input, (instruction_handler_cb)(&fill_symbol_table), &st);
    if (ret != 0) {
        goto cleanup;
    }

cleanup:
    symbol_table_destroy(&st);
    return ret;
}

int main() {
    int ret = translate(stdin, stdout);
    return ret;
}
