#include "parsing.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbols.h"

#define MAX_LINE_LENGTH 1024
#define MAX_LABEL_LENGTH 128
#define MAX_ADDR_LENGTH MAX_LABEL_LENGTH

enum InstructionType {
    INSTRUCTION_TYPE_A,
    INSTRUCTION_TYPE_LABEL,
    INSTRUCTION_TYPE_NONE,
};

struct InstructionLabelFields {
    char name[MAX_LABEL_LENGTH + 1];
};

enum AddressType {
    ADDRESS_TYPE_RAW,
    ADDRESS_TYPE_LABEL,
};

struct InstructionAFields {
    enum AddressType type;
    union {
        char label[MAX_LABEL_LENGTH + 1];
        uint16_t raw;
    };
};

struct Instruction {
    enum InstructionType type;
    union {
        struct InstructionLabelFields lbl_fields;
        struct InstructionAFields a_fields;
    };
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

bool eol(const char c) { return c == '\0' || c == '\n'; }

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

int parse_a_instruction(const char *line, struct Instruction *instr_out,
                        struct ParseLineError *error_out) {
    assert(instr_out);
    assert(error_out);

    const char *c = line;
    assert(*c == '@');

    instr_out->type = INSTRUCTION_TYPE_A;
    int address_len = 0;

    int col = 1;

    char buf[MAX_ADDR_LENGTH];
    enum AddressType addr_type = ADDRESS_TYPE_RAW;
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
            addr_type = ADDRESS_TYPE_LABEL;
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

    instr_out->a_fields.type = addr_type;
    switch (addr_type) {
    case ADDRESS_TYPE_RAW:
        instr_out->a_fields.raw = (uint16_t)atoi(buf);
        break;
    case ADDRESS_TYPE_LABEL:
        memcpy(instr_out->a_fields.label, buf, address_len + 1);
        break;
    }
    return 0;
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
    case '@':
        instruction_parse_result = parse_a_instruction(c, instr_out, error_out);
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
    int visual_line = 1;      // 1 based, used for error reporting
    int instruction_line = 0; // 0 based, used for labels and such
    struct Instruction instr;
    struct ParseLineError err;
    char buffer[MAX_LINE_LENGTH + 2]; // +2 for \n and \0
    bool truncated;

    while (fgets2(buffer, sizeof(buffer), input, &truncated)) {
        if (truncated) {
            fprintf(stderr, "Line %d exceeds max line length of %d",
                    visual_line, MAX_LINE_LENGTH);
            return 1;
        }
        enum ParseLineResult ret = parse_line(buffer, &instr, &err);
        switch (ret) {
        case PLR_INSTRUCTION:
            instruction_handler(data, &instr, instruction_line);
            instruction_line++;
            break;
        case PLR_EMPTY:
            break;
        case PLR_ERROR:
            // TODO print error
            return 1;
        }
        visual_line++;
    }
    if (feof(input)) {
        fputs("EOF has been reached\n", stderr);
    }
    if (ferror(input)) {
        fputs("Error while reading input\n", stderr);
    }

    return 0;
}

void fill_symbol_table(void *data, const struct Instruction *instr,
                       int line_number) {
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

    ret =
        process_file(input, (instruction_handler_cb)(&fill_symbol_table), &st);
    if (ret != 0) {
        goto cleanup;
    }

cleanup:
    symbol_table_destroy(&st);
    return ret;
}
