#include "translate.h"

#include <stdio.h>

#include "IR.h"
#include "encoding.h"
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

struct EncodeCbData {
    struct SymbolTable *st;
    FILE *out_stream;
};

void encode_cb(struct Instruction *instr, int line_number, void *data) {
    struct EncodeCbData *_data = data;
    uint16_t encoded;
    encode(instr, _data->st, &encoded);
    fwrite(&encoded, sizeof(encoded), 1, _data->out_stream);
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

    rewind(input);
    struct EncodeCbData data = {.st = &st, .out_stream = output};
    ret = process_file(input, (instruction_handler_cb)(&encode_cb), &data);
    if (ret != 0) {
        goto cleanup;
    }
    fflush(output);

cleanup:
    symbol_table_destroy(&st);
    return ret;
}
