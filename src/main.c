#include <stdio.h>
#include <unistd.h>

#include "parsing/parsing.h"
#include "symbols.h"

void fill_symbol_table_cb(struct Instruction *instr, int line_number,
                          void *data) {
    struct SymbolTable *st = data;
    if (instr->type != INSTRUCTION_TYPE_LABEL) {
        return;
    }
    symbol_table_add(st, instr->lbl_fields.name, line_number + 1);
}

int translate(FILE *input, FILE *output) {
    int ret = 0;
    struct SymbolTable st;
    symbol_table_init(&st);

    ret = process_file(input, (instruction_handler_cb)(&fill_symbol_table_cb),
                       &st);
    if (ret != 0) {
        goto cleanup;
    }

cleanup:
    symbol_table_destroy(&st);
    return ret;
}

FILE *input_file() {
    if (fseek(stdin, 0, SEEK_CUR) == 0) {
        return stdin;
    }

    FILE *tmp = tmpfile();
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), stdin))) {
        fwrite(buf, 1, n, tmp);
    }
    rewind(tmp);
    return tmp;
}

int main() {
    FILE *input = input_file();
    int ret = translate(input, stdout);
    fclose(input);
    return ret;
}
