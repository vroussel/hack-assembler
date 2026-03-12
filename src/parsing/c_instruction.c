#include "internal.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define DEST_A 0x1
#define DEST_D 0x2
#define DEST_M 0x4

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
    unsigned long n = sizeof(comp_values) / sizeof(typeof(*comp_values));
    for (unsigned long i = 0; i < n; i++) {
        struct CompValue cv = comp_values[i];
        if (strcmp(cv.str, comp) == 0) {
            *out = cv.value;
            return 0;
        }
    }
    return 1;
}

int parse_jump(const char *jump, enum Jump *out) {
    unsigned long n = sizeof(jump_values) / sizeof(typeof(*jump_values));
    for (unsigned long i = 0; i < n; i++) {
        struct JumpValue jv = jump_values[i];
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

    const char *equal_idx = strchr(line, '=');
    if (equal_idx) {
        dest_begin = line;
        dest_len = equal_idx - dest_begin;
        comp_begin = equal_idx + 1;
    } else {
        comp_begin = line;
    }

    const char *semicolon_idx = strchr(line, ';');
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
