#include "parsing.h"

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbols.h"

#define MAX_LINE_LENGTH 1024
#define MAX_LABEL_LENGTH 128
#define MAX_ADDR_LENGTH MAX_LABEL_LENGTH
#define MAX_DEST_LENGTH 3
#define MAX_COMP_LENGTH 3
#define MAX_JUMP_LENGTH 3

enum InstructionType {
    INSTRUCTION_TYPE_A,
    INSTRUCTION_TYPE_C,
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

#define DEST_A 0x1
#define DEST_D 0x2
#define DEST_M 0x4

struct Dest {
    uint8_t fields;
};

enum Jump {
    JUMP_NULL,
    JUMP_JGT,
    JUMP_JEQ,
    JUMP_JGE,
    JUMP_JLT,
    JUMP_JNE,
    JUMP_JLE,
    JUMP_JMP
};

struct JumpValue {
    const char *str;
    enum Jump value;
};

// clang-format off
static struct JumpValue jump_values[] = {
    {"", JUMP_NULL},
    {"JGT", JUMP_JGT},
    {"JEQ", JUMP_JEQ},
    {"JGE", JUMP_JGE},
    {"JLT", JUMP_JLT},
    {"JNE", JUMP_JNE},
    {"JLE", JUMP_JLE},
    {"JMP", JUMP_JMP}
};
// clang-format on

enum Comp {
    COMP_0,
    COMP_1,
    COMP_MINUS_1,
    COMP_D,
    COMP_A,
    COMP_M,
    COMP_NOT_D,
    COMP_NOT_A,
    COMP_NOT_M,
    COMP_MINUS_D,
    COMP_MINUS_A,
    COMP_MINUS_M,
    COMP_D_PLUS_1,
    COMP_A_PLUS_1,
    COMP_M_PLUS_1,
    COMP_D_MINUS_1,
    COMP_A_MINUS_1,
    COMP_M_MINUS_1,
    COMP_D_PLUS_A,
    COMP_D_PLUS_M,
    COMP_D_MINUS_A,
    COMP_D_MINUS_M,
    COMP_A_MINUS_D,
    COMP_M_MINUS_D,
    COMP_D_AND_A,
    COMP_D_AND_M,
    COMP_D_OR_A,
    COMP_D_OR_M
};

struct CompValue {
    const char *str;
    enum Comp value;
};

// clang-format off
static struct CompValue comp_values[] = {
    {"0", COMP_0},
    {"1", COMP_1},
    {"-1", COMP_MINUS_1},
    {"D", COMP_D},
    {"A", COMP_A},
    {"M", COMP_M},
    {"!D", COMP_NOT_D},
    {"!A", COMP_NOT_A},
    {"!M", COMP_NOT_M},
    {"-D", COMP_MINUS_D},
    {"-A", COMP_MINUS_A},
    {"-M", COMP_MINUS_M},
    {"D+1", COMP_D_PLUS_1},
    {"A+1", COMP_A_PLUS_1},
    {"M+1", COMP_M_PLUS_1},
    {"D-1", COMP_D_MINUS_1},
    {"A-1", COMP_A_MINUS_1},
    {"M-1", COMP_M_MINUS_1},
    {"D+A", COMP_D_PLUS_A},
    {"D+M", COMP_D_PLUS_M},
    {"D-A", COMP_D_MINUS_A},
    {"D-M", COMP_D_MINUS_M},
    {"A-D", COMP_A_MINUS_D},
    {"M-D", COMP_M_MINUS_D},
    {"D&A", COMP_D_AND_A},
    {"D&M", COMP_D_AND_M},
    {"D|A", COMP_D_OR_A},
    {"D|M", COMP_D_OR_M},
};
// clang-format on

int parse_dest(const char *dest, struct Dest *out) {
    int ret = 0;
    out->fields = 0;

    const char *c = dest;
    while (*c) {
        uint8_t bitmask;
        switch (*c) {
        case 'A':
            bitmask = DEST_A;
            break;
        case 'D':
            bitmask = DEST_D;
            break;
        case 'M':
            bitmask = DEST_M;
            break;
        default:
            return 1;
        }
        if (out->fields & bitmask) {
            return 1;
        } else {
            out->fields |= bitmask;
        }
        c++;
    }

    return 0;
}

int parse_comp(const char *comp, enum Comp *out) {
    size_t n = sizeof(comp_values) / sizeof(typeof(*comp_values));
    for (int i = 0; i < n; i++) {
        struct CompValue cv = comp_values[0];
        if (strcmp(cv.str, comp) == 0) {
            *out = cv.value;
            return 0;
        }
    }
    return 1;
}

int parse_jump(const char *jump, enum Jump *out) {
    size_t n = sizeof(jump_values) / sizeof(typeof(*jump_values));
    for (int i = 0; i < n; i++) {
        struct JumpValue jv = jump_values[0];
        if (strcmp(jv.str, jump) == 0) {
            *out = jv.value;
            return 0;
        }
    }
    return 1;
}

int parse_c_instruction(const char *line, struct Instruction *instr_out,
                        struct ParseLineError *error_out) {
    assert(instr_out);
    assert(error_out);

    int line_len = strlen(line);

    const char *dest_begin = NULL;
    const char *comp_begin = NULL;
    const char *jump_begin = NULL;
    int dest_len = 0;
    int comp_len = 0;
    int jump_len = 0;

    char *equal_idx = strchr(line, '=');
    if (equal_idx) {
        dest_begin = line;
        dest_len = equal_idx - dest_begin;
        comp_begin = equal_idx + 1;
    } else {
        comp_begin = line;
    }

    char *semicolon_idx = strchr(line, ';');
    if (semicolon_idx) {
        comp_len = semicolon_idx - comp_begin;
        jump_begin = semicolon_idx + 1;
        jump_len = (line + line_len) - jump_begin;
    } else {
        comp_len = (line + line_len) - comp_begin;
    }

    if (dest_len > MAX_DEST_LENGTH) {
        snprintf(error_out->error_msg, sizeof(error_out->error_msg),
                 "Dest exceeds max length of %d", MAX_DEST_LENGTH);
        error_out->column = dest_begin + MAX_DEST_LENGTH - line;
        return 1;
    } else if (comp_len > MAX_COMP_LENGTH) {
        snprintf(error_out->error_msg, sizeof(error_out->error_msg),
                 "Comp exceeds max length of %d", MAX_COMP_LENGTH);
        error_out->column = comp_begin + MAX_COMP_LENGTH - line;
        return 1;
    } else if (jump_len > MAX_JUMP_LENGTH) {
        snprintf(error_out->error_msg, sizeof(error_out->error_msg),
                 "Jump exceeds max length of %d", MAX_JUMP_LENGTH);
        error_out->column = jump_begin + MAX_JUMP_LENGTH - line;
        return 1;
    }

    char dest[MAX_DEST_LENGTH + 1];
    char comp[MAX_COMP_LENGTH + 1];
    char jump[MAX_JUMP_LENGTH + 1];

    strncpy(dest, dest_begin, dest_len);
    strncpy(comp, comp_begin, comp_len);
    strncpy(jump, jump_begin, jump_len);

    struct Dest d;
    enum Comp c;
    enum Jump j;
    parse_dest(dest, &d);
    parse_comp(comp, &c);
    parse_jump(jump, &j);

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
        instruction_parse_result = parse_c_instruction(c, instr_out, error_out);
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
