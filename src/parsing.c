#include "parsing.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LABEL_LENGTH 128

enum InstructionType {
    INSTRUCTION_TYPE_LABEL,
    INSTRUCTION_TYPE_NONE,
};

struct InstructionLabelFields {
    char name[MAX_LABEL_LENGTH + 1];
};

union InstructionFields {
    struct InstructionLabelFields lbl_fields;
};

struct Instruction {
    enum InstructionType type;
    union InstructionFields fields;
};

struct ParseLineError {
    int column;
    char error_msg[1024];
};

enum ParseLineResult {
    PLR_INSTRUCTION,
    PLR_EMPTY,
    PLR_ERROR,
};

int parse_label(const char *line, struct Instruction *instr_out,
                struct ParseLineError *error_out) {
    assert(instr_out);
    assert(error_out);

    const char *c = line;
    assert(*c == '(');

    instr_out->type = INSTRUCTION_TYPE_LABEL;
    int label_len = 0;

    int ret = 0;
    int col = 1;
    while (*++c != ')') {
        col++;

        if (*c == '\0') {
            ret = 1;
            strncpy(error_out->error_msg, "Missing ')' in symbol declaration",
                    sizeof(error_out->error_msg));
            error_out->column = col;
            break;
        } else if (label_len == MAX_LABEL_LENGTH) {
            ret = 1;
            snprintf(error_out->error_msg, sizeof(error_out->error_msg),
                     "Label exceeds max length of %d", MAX_LABEL_LENGTH);
            error_out->column = col;
            break;
        } else if (!isalnum(*c) && *c != '_') {
            ret = 1;
            strncpy(error_out->error_msg, "Invalid label character",
                    sizeof(error_out->error_msg));
            error_out->column = col;
            break;
        }

        instr_out->fields.lbl_fields.name[label_len++] = *c;
    }
    if (label_len == 0) {
        ret = 1;
        strncpy(error_out->error_msg, "Empty label",
                sizeof(error_out->error_msg));
        error_out->column = 2;
    }

    instr_out->fields.lbl_fields.name[label_len] = '\0';
    return ret;
}

enum ParseLineResult parse_line(const char *line, struct Instruction *instr_out,
                                struct ParseLineError *error_out) {
    assert(line);
    assert(instr_out);
    assert(error_out);

    // Ignore spaces
    const char *c = line;
    while (isspace((unsigned char)*c)) {
        c++;
    }

    if (*c == '\0') {
        return PLR_EMPTY;
    } else if (strncmp(c, "//", 2) == 0) {
        return PLR_EMPTY;
    }

    int instruction_parse_result;
    switch (*c) {
    case '(':
        instruction_parse_result = parse_label(c, instr_out, error_out);
        break;
    default:
        break;
    };

    if (instruction_parse_result == 0) {
        return PLR_INSTRUCTION;
    } else {
        return PLR_ERROR;
    }
}

static char *fgets2(char *s, int size, FILE *stream, bool *truncated) {
    if (truncated) {
        *truncated = false;
    }
    char *ret = fgets(s, size, stream);
    if (truncated && ret) {
        *truncated = (strlen(s) == size - 1 && s[size - 2] != '\n');
    }
    return ret;
}

typedef void (*instruction_handler_cb)(void *data,
                                       const struct Instruction *instr,
                                       int line_number);

int process_file(FILE *input, instruction_handler_cb instruction_handler,
                 void *data) {
    int vline = 0;
    int line = 0;
    struct Instruction instr;
    struct ParseLineError err;
    char buffer[MAX_LINE_LENGTH + 2]; // +2 for \n and \0
    bool truncated;

    while (fgets2(buffer, sizeof(buffer), input, &truncated)) {
        vline++;
        if (truncated) {
            fprintf(stderr, "Line %d exceeds max line length of %d", vline,
                    MAX_LINE_LENGTH);
            return 1;
        }
        enum ParseLineResult ret = parse_line(buffer, &instr, &err);
        switch (ret) {
        case PLR_INSTRUCTION:
            line++;
            instruction_handler(data, &instr, line);
            break;
        case PLR_EMPTY:
            break;
        case PLR_ERROR:
            // TODO print error
            return 1;
        }
    }
    if (feof(input)) {
        fputs("EOF has been reached\n", stderr);
    }
    if (ferror(input)) {
        fputs("Error while reading input\n", stderr);
    }

    return 0;
}

void fill_symbol_table(struct SymbolTable *st, const struct Instruction *instr,
                       int line_number) {
    ;
}

int translate(FILE *input, FILE *output) {
    struct SymbolTable st;
    symbol_table_init(&st);

    int ret =
        process_file(input, (instruction_handler_cb)(&fill_symbol_table), &st);
    if (ret != 0) {
        return ret;
        symbol_table_destroy(&st);
    }

    symbol_table_destroy(&st);
    return 0;
}
