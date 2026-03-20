#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "translate.h"

void print_usage(const char *arg0, FILE *f) {
    fprintf(f, "%s [-t|--text]", arg0);
}

FILE *input_file() {
    if (fseek(stdin, 0, SEEK_CUR) == 0) {
        return stdin;
    }

    FILE *tmp = tmpfile();
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), stdin))) {
        fwrite(buf, 1, n, tmp);
    }
    rewind(tmp);
    return tmp;
}

int main(int argc, char *argv[]) {
    enum OutputFormat format = OUTPUT_FORMAT_BINARY;
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (strcmp("-t", arg) == 0 || strcmp("--text", arg) == 0) {
            format = OUTPUT_FORMAT_TEXT;
        } else {
            print_usage(argv[0], stderr);
            return 1;
        }
    }

    FILE *input = input_file();
    int ret = translate(input, stdout, format);
    fclose(input);
    return ret;
}
