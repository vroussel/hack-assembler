#include "translate.h"

#include <stdio.h>

#include "IR.h"
#include "encoding.h"
#include "parsing/parsing.h"
#include "symbols.h"
#include "utils.h"

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
    enum OutputFormat format;
};

void encode_cb(struct Instruction *instr, int line_number, void *data) {
    struct EncodeCbData *_data = data;
    uint16_t encoded;
    char buf[16];
    encode(instr, _data->st, &encoded);
    if (_data->format == OUTPUT_FORMAT_BINARY) {
        fwrite(&encoded, sizeof(encoded), 1, _data->out_stream);
    } else {
        to_bin(encoded, buf);
        fprintf(_data->out_stream, "%s\n", buf);
    }
}

int translate(FILE *input, FILE *output, enum OutputFormat format) {
    int ret = 0;
    struct SymbolTable st;
    symbol_table_init(&st);

    ret = process_file(input, (instruction_handler_cb)(&fill_symbol_table_cb),
                       &st);
    if (ret != 0) {
        goto cleanup;
    }

    rewind(input);
    struct EncodeCbData data = {
        .st = &st, .out_stream = output, .format = format};
    ret = process_file(input, (instruction_handler_cb)(&encode_cb), &data);
    if (ret != 0) {
        goto cleanup;
    }
    fflush(output);

cleanup:
    symbol_table_destroy(&st);
    return ret;
}
