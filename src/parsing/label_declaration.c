#include "parsing.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int parse_label(const char *line, struct Instruction *instr_out,
                struct ParseLineError *error_out) {
    assert(instr_out);
    assert(error_out);

    const char *c = line;
    assert(*c == '(');

    int label_len = 0;

    int col = 1;
    char buf[MAX_LABEL_LENGTH];
    while (true) {
        c++;
        col++;

        if (*c == ')') {
            break;
        }
        if (*c == '\0' || *c == '\n') {
            strncpy(error_out->error_msg, "Missing ')' in symbol declaration",
                    sizeof(error_out->error_msg));
            error_out->column = col;
            return 1;
        } else if (label_len == MAX_LABEL_LENGTH) {
            snprintf(error_out->error_msg, sizeof(error_out->error_msg),
                     "Label exceeds max length of %d", MAX_LABEL_LENGTH);
            error_out->column = col;
            return 1;
        } else if (!isalnum(*c) && *c != '_') {
            strncpy(error_out->error_msg, "Invalid label character",
                    sizeof(error_out->error_msg));
            error_out->column = col;
            return 1;
        }

        buf[label_len++] = *c;
    }
    buf[label_len] = '\0';

    if (label_len == 0) {
        strncpy(error_out->error_msg, "Empty label",
                sizeof(error_out->error_msg));
        error_out->column = col;
        return 1;
    } else if (!eol(*++c)) {
        col++;
        strncpy(error_out->error_msg,
                "Unwxpected character after end of symbol ')'",
                sizeof(error_out->error_msg));
        error_out->column = col;
        return 1;
    }

    instr_out->type = INSTRUCTION_TYPE_LABEL;
    memcpy(instr_out->lbl_fields.name, buf, label_len + 1);
    return 0;
}
