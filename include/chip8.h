#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MEMORY_SIZE 4096
#define PROGRAM_START 512
#define FONT_START 80
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

enum OpcodeGroups {
  OPCODE_GROUP_SYS_OR_RET = 0,
  OPCODE_GROUP_JUMP = 4096,
  OPCODE_GROUP_CALL = 8192,
  OPCODE_GROUP_SE_VX_BYTE = 12288,
  OPCODE_GROUP_SNE_VX_BYTE = 16384,
  OPCODE_GROUP_SE_VX_VY = 20480,
  OPCODE_GROUP_LD_VX_BYTE = 24576,
  OPCODE_GROUP_ADD_VX_BYTE = 28672,
  OPCODE_GROUP_ARITHMETIC = 32768,
  OPCODE_GROUP_SNE_VX_VY = 36864,
  OPCODE_GROUP_LD_I_ADDR = 40960,
  OPCODE_GROUP_JP_V0_ADDR = 45056,
  OPCODE_GROUP_RND_VX_BYTE = 49152,
  OPCODE_GROUP_DRW_VX_VY_N = 53248,
  OPCODE_GROUP_KEY_SKIP = 57344,
  OPCODE_GROUP_MISC = 61440
};

enum SysOpcodes {
  OPCODE_CLS = 224,
  OPCODE_RET = 238
};

enum ArithmeticSubOpcodes {
  SUB_OP_LD = 0,
  SUB_OP_OR = 1,
  SUB_OP_AND = 2,
  SUB_OP_XOR = 3,
  SUB_OP_ADD = 4,
  SUB_OP_SUB = 5,
  SUB_OP_SHR = 6,
  SUB_OP_SUBN = 7,
  SUB_OP_SHL = 14
};

enum KeySkipOpcodes {
  OPCODE_SKP = 158,
  OPCODE_SKNP = 161
};

enum MiscSubOpcodes {
  SUB_OP_LD_VX_DT = 7,
  SUB_OP_LD_VX_K = 10,
  SUB_OP_LD_DT_VX = 21,
  SUB_OP_LD_ST_VX = 24,
  SUB_OP_ADD_I_VX = 30,
  SUB_OP_LD_F_VX = 41,
  SUB_OP_LD_B_VX = 51,
  SUB_OP_LD_I_REG = 85,
  SUB_OP_LD_REG_I = 101
};

enum ExitCodes {
  COMPLETED_SUCCESSFULLY,
  FILE_NOT_PROVIDED,
  FILE_MISSING,
  PROGRAM_TOO_LARGE,
  MEMORY_ALLOCATION_FAILURE,
};

typedef struct ChipEight {
  uint8_t memory[MEMORY_SIZE];
  uint16_t pc;
  uint16_t I;
  uint8_t V[16];
  uint8_t dt;
  uint8_t st;
  uint16_t stack[16];
  uint8_t sc;
  uint32_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
  uint8_t keys[16];
  uint8_t key_register;
  bool waiting_for_key;
} chip_eight_t;

void chip8_init(chip_eight_t *chip);
int chip8_load_rom(chip_eight_t *chip, const char *file_path);
void chip8_step(chip_eight_t *chip);
void chip8_tick_timers(chip_eight_t *chip);

#endif // CHIP8_H
