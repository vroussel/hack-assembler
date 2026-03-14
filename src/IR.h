#include <stdint.h>

#define MAX_LABEL_LENGTH 128
#define MAX_ADDR_LENGTH MAX_LABEL_LENGTH
#define MAX_DEST_LENGTH 3
#define MAX_COMP_LENGTH 3
#define MAX_JUMP_LENGTH 3

struct InstructionLabelFields {
    char name[MAX_LABEL_LENGTH + 1];
};

enum AddressType {
    ADDRESS_TYPE_RAW,
    ADDRESS_TYPE_LABEL,
};

struct InstructionAFields {
    enum AddressType type;
    union {
        char label[MAX_LABEL_LENGTH + 1];
        uint16_t raw;
    };
};

struct Dest {
    uint8_t A : 1;
    uint8_t D : 1;
    uint8_t M : 1;
};

enum Jump {
    JUMP_NULL,
    JUMP_JGT,
    JUMP_JEQ,
    JUMP_JGE,
    JUMP_JLT,
    JUMP_JNE,
    JUMP_JLE,
    JUMP_JMP
};

enum Comp {
    COMP_0,
    COMP_1,
    COMP_MINUS_1,
    COMP_D,
    COMP_A,
    COMP_M,
    COMP_NOT_D,
    COMP_NOT_A,
    COMP_NOT_M,
    COMP_MINUS_D,
    COMP_MINUS_A,
    COMP_MINUS_M,
    COMP_D_PLUS_1,
    COMP_A_PLUS_1,
    COMP_M_PLUS_1,
    COMP_D_MINUS_1,
    COMP_A_MINUS_1,
    COMP_M_MINUS_1,
    COMP_D_PLUS_A,
    COMP_D_PLUS_M,
    COMP_D_MINUS_A,
    COMP_D_MINUS_M,
    COMP_A_MINUS_D,
    COMP_M_MINUS_D,
    COMP_D_AND_A,
    COMP_D_AND_M,
    COMP_D_OR_A,
    COMP_D_OR_M
};

struct InstructionCFields {
    enum Jump jump;
    enum Comp comp;
    struct Dest dest;
};

enum InstructionType {
    INSTRUCTION_TYPE_A,
    INSTRUCTION_TYPE_C,
    INSTRUCTION_TYPE_LABEL,
};

struct Instruction {
    enum InstructionType type;
    union {
        struct InstructionLabelFields lbl_fields;
        struct InstructionAFields a_fields;
        struct InstructionCFields c_fields;
    };
};
