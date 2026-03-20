#include <stdio.h>

enum OutputFormat { OUTPUT_FORMAT_BINARY, OUTPUT_FORMAT_TEXT };

int translate(FILE *input, FILE *output, enum OutputFormat format);
