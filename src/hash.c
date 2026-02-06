#include <stdint.h>
#include <string.h>

int hash(const char* key, int mod) {
    unsigned long h = 0;
    unsigned long len = strlen(key);
    for (unsigned long i = 0; i < len; i++) {
        h = h * 31 + key[i];
    }
    return h % mod;
}
