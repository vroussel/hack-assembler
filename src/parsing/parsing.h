#ifndef PARSING_PARSING_H
#define PARSING_PARSING_H

#include <stdbool.h>
#include <stdio.h>

#include "../IR.h"

struct ParseLineError {
    int column;
    char error_msg[1024];
};

enum ParseLineResult {
    PLR_INSTRUCTION,
    PLR_EMPTY,
    PLR_ERROR,
};

typedef void (*instruction_handler_cb)(void *data,
                                       const struct Instruction *instr,
                                       int line_number);

bool eol(const char c);

char *fgets2(char *s, int size, FILE *stream, bool *truncated);

enum ParseLineResult parse_line(const char *line, struct Instruction *instr_out,
                                struct ParseLineError *error_out);

int parse_label(const char *line, struct Instruction *instr_out,
                struct ParseLineError *error_out);
int parse_a_instruction(const char *line, struct Instruction *instr_out,
                        struct ParseLineError *error_out);
int parse_c_instruction(const char *line, struct Instruction *instr_out,
                        struct ParseLineError *error_out);

int process_file(FILE *input, instruction_handler_cb instruction_handler,
                 void *data);

#endif
