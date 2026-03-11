#ifndef PARSING_INTERNAL_H
#define PARSING_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>

#include "parsing.h"

#define MAX_LINE_LENGTH 1024
#define MAX_LABEL_LENGTH 128
#define MAX_ADDR_LENGTH MAX_LABEL_LENGTH
#define MAX_DEST_LENGTH 3
#define MAX_COMP_LENGTH 3
#define MAX_JUMP_LENGTH 3

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

bool eol(const char c);
int parse_label(const char *line, struct Instruction *instr_out,
                struct ParseLineError *error_out);
int parse_a_instruction(const char *line, struct Instruction *instr_out,
                        struct ParseLineError *error_out);
int parse_c_instruction(const char *line, struct Instruction *instr_out,
                        struct ParseLineError *error_out);

enum ParseLineResult parse_line(const char *line, struct Instruction *instr_out,
                                struct ParseLineError *error_out);

char *fgets2(char *s, int size, FILE *stream, bool *truncated);

#endif
