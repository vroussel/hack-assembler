#include <unistd.h>

#include "parsing.h"

int main() {
    int ret = translate(stdin, stdout);
    return ret;
}
