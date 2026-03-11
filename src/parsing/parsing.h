#ifndef PARSING_PARSING_H
#define PARSING_PARSING_H

#include <stdio.h>

struct Instruction;

typedef void (*instruction_handler_cb)(void *data,
                                       const struct Instruction *instr,
                                       int line_number);

int process_file(FILE *input, instruction_handler_cb instruction_handler,
                 void *data);

enum InstructionType {
    INSTRUCTION_TYPE_A,
    INSTRUCTION_TYPE_C,
    INSTRUCTION_TYPE_LABEL,
};

enum InstructionType instruction_type(const struct Instruction *instr);
const char *instruction_label_name(const struct Instruction *instr);

#endif
