#include "utils.h"

void to_bin(uint16_t n, char *out) {
    int i;
    for (i = 0; i < 16; i++) {
        out[i] = n & (1 << (15 - i)) ? '1' : '0';
    }
    out[i] = '\0';
}
