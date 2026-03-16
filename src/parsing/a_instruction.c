#include "parsing.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_a_instruction(const char *line, struct Instruction *instr_out,
                        struct ParseLineError *error_out) {
    assert(instr_out);
    assert(error_out);

    const char *c = line;
    assert(*c == '@');

    int address_len = 0;

    int col = 1;

    char buf[MAX_ADDR_LENGTH];
    bool resolved = true;
    while (true) {
        c++;
        col++;
        if (eol(*c)) {
            break;
        } else if (address_len == MAX_ADDR_LENGTH) {
            snprintf(error_out->error_msg, sizeof(error_out->error_msg),
                     "Address exceeds max length of %d", MAX_LABEL_LENGTH);
            error_out->column = col;
            return 1;
        } else if (!isalnum(*c) && *c != '_') {
            strncpy(error_out->error_msg, "Invalid address character",
                    sizeof(error_out->error_msg));
            error_out->column = col;
            return 1;
        }

        if (!isdigit(*c)) {
            resolved = false;
        }

        buf[address_len++] = *c;
    }

    buf[address_len] = '\0';

    if (address_len == 0) {
        strncpy(error_out->error_msg, "Empty address",
                sizeof(error_out->error_msg));
        error_out->column = col;
        return 1;
    }

    instr_out->type = INSTRUCTION_TYPE_A;
    instr_out->a_fields.resolved = resolved;
    if (resolved) {
        instr_out->a_fields.address = (uint16_t)atoi(buf);
    } else {
        memcpy(instr_out->a_fields.symbol, buf, address_len + 1);
    }

    return 0;
}
