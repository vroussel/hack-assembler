#include "encoding.h"

#include <assert.h>

// clang-format off
static uint16_t comp_encoding[] = {
    [COMP_0]         = 0b1110101010,
    [COMP_1]         = 0b1110111111,
    [COMP_MINUS_1]   = 0b1110111010,
    [COMP_D]         = 0b1110001100,
    [COMP_A]         = 0b1110110000,
    [COMP_M]         = 0b1111110000,
    [COMP_NOT_D]     = 0b1110001101,
    [COMP_NOT_A]     = 0b1110110001,
    [COMP_NOT_M]     = 0b1111110001,
    [COMP_MINUS_D]   = 0b1110001111,
    [COMP_MINUS_A]   = 0b1110110011,
    [COMP_MINUS_M]   = 0b1111110011,
    [COMP_D_PLUS_1]  = 0b1110011111,
    [COMP_A_PLUS_1]  = 0b1110110111,
    [COMP_M_PLUS_1]  = 0b1111110111,
    [COMP_D_MINUS_1] = 0b1110001110,
    [COMP_A_MINUS_1] = 0b1110110010,
    [COMP_M_MINUS_1] = 0b1111110010,
    [COMP_D_PLUS_A]  = 0b1110000010,
    [COMP_D_PLUS_M]  = 0b1111000010,
    [COMP_D_MINUS_A] = 0b1110010011,
    [COMP_D_MINUS_M] = 0b1111010011,
    [COMP_A_MINUS_D] = 0b1110000111,
    [COMP_M_MINUS_D] = 0b1111000111,
    [COMP_D_AND_A]   = 0b1110000000,
    [COMP_D_AND_M]   = 0b1111000000,
    [COMP_D_OR_A]    = 0b1110010101,
    [COMP_D_OR_M]    = 0b1111010101,
};

static uint16_t jump_encoding[] = {
    [JUMP_NULL] = 0b000,
    [JUMP_JGT]  = 0b001,
    [JUMP_JEQ]  = 0b010,
    [JUMP_JGE]  = 0b011,
    [JUMP_JLT]  = 0b100,
    [JUMP_JNE]  = 0b101,
    [JUMP_JLE]  = 0b110,
    [JUMP_JMP]  = 0b111,
};
// clang-format on

uint16_t encode_a(struct Instruction *instr, struct SymbolTable *st) {
    assert(instr->type == INSTRUCTION_TYPE_A);

    if (!instr->a_fields.resolved) {
        instr->a_fields.address =
            symbol_table_get_or_create(st, instr->a_fields.symbol);
        instr->a_fields.resolved = true;
    }
    return instr->a_fields.address;
}

uint16_t encode_c(const struct Instruction *instr) {
    assert(instr->type == INSTRUCTION_TYPE_C);

    const struct Dest d = instr->c_fields.dest;
    uint16_t comp = comp_encoding[instr->c_fields.comp];
    uint16_t jump = jump_encoding[instr->c_fields.jump];
    uint16_t dest = d.A << 2 | d.D << 1 | d.M;

    return 0b111 << 13 | comp << 6 | dest << 3 | jump;
}

int encode(struct Instruction *instr, struct SymbolTable *st, uint16_t *out) {
    assert(instr);
    assert(out);

    if (is_pseudo_instruction(instr)) {
        return 1;
    }

    switch (instr->type) {
    case INSTRUCTION_TYPE_A:
        *out = encode_a(instr, st);
        break;
    case INSTRUCTION_TYPE_C:
        *out = encode_c(instr);
        break;
    case INSTRUCTION_TYPE_LABEL:
        break;
    }

    return 0;
}
