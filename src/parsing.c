#include "parsing.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

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

    return PLR_ERROR;
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

int translate(FILE *input, FILE *output) {
    int vline = 0;
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
        parse_line(buffer, &instr, &err);
    }
    if (feof(input)) {
        fputs("EOF has been reached\n", stderr);
    }
    if (ferror(input)) {
        fputs("Error while reading input\n", stderr);
    }

    return 0;
}
