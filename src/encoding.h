#include <stdint.h>

#include "IR.h"
#include "symbols.h"

uint16_t encode_a(struct Instruction *instr, struct SymbolTable *st);
uint16_t encode_c(const struct Instruction *instr);
int encode(struct Instruction *instr, struct SymbolTable *st, uint16_t *out);
