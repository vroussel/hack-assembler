#include <stdbool.h>
#include <stdio.h>

struct Instruction {};
struct ParseLineError {
    int column;
    const char *error_msg;
};

enum ParseLineResult {
    PLR_INSTRUCTION,
    PLR_EMPTY,
    PLR_ERROR,
};
enum ParseLineResult parse_line(const char *line,
                                struct Instruction *instruction_out,
                                struct ParseLineError *error_out);
int translate(FILE *input, FILE *output);
