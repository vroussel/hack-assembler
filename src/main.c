#include <stdio.h>
#include <unistd.h>

#include "translate.h"

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

int main() {
    FILE *input = input_file();
    int ret = translate(input, stdout);
    fclose(input);
    return ret;
}
