#include "parsing.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

bool eol(const char c) { return c == '\0' || c == '\n'; }

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

char *fgets2(char *s, int size, FILE *stream, bool *truncated) {
    if (truncated) {
        *truncated = false;
    }
    char *ret = fgets(s, size, stream);
    if (truncated && ret) {
        *truncated = ((int)strlen(s) == size - 1 && s[size - 2] != '\n');
    }
    return ret;
}

int process_file(FILE *input, instruction_handler_cb instruction_handler,
                 void *data) {
    int visual_line = 1;      // 1 based, used for error reporting
    int instruction_line = 0; // 0 based, used for "real" instructions
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
            if (!is_pseudo_instruction(&instr)) {
                instruction_line++;
            }
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
