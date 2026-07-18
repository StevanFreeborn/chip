#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "chip8.h"

static inline void write_opcode_to_memory(chip_eight_t *chip, uint16_t opcode) {
  chip->memory[chip->pc] = (opcode >> 8) & 0xFF;
  chip->memory[chip->pc + 1] = opcode & 0xFF;
}

static inline void write_clear_screen_instruction(chip_eight_t *chip) {
  write_opcode_to_memory(chip, OPCODE_GROUP_SYS_OR_RET | OPCODE_CLS);
}

static inline void write_return_instruction(chip_eight_t *chip) {
  write_opcode_to_memory(chip, OPCODE_GROUP_SYS_OR_RET | OPCODE_RET);
}

static inline void write_jump_instruction(chip_eight_t *chip, uint16_t addr) {
  write_opcode_to_memory(chip, OPCODE_GROUP_JUMP | (addr & 0x0FFF));
}

static inline void write_call_instruction(chip_eight_t *chip, uint16_t addr) {
  write_opcode_to_memory(chip, OPCODE_GROUP_CALL | (addr & 0x0FFF));
}

static inline void write_skip_if_register_equal_value_instruction(chip_eight_t *chip, uint8_t x, uint8_t val) {
  write_opcode_to_memory(chip, OPCODE_GROUP_SE_VX_BYTE | ((x & 0x0F) << 8) | val);
}

static inline void write_skip_if_register_not_equal_value_instruction(chip_eight_t *chip, uint8_t x, uint8_t val) {
  write_opcode_to_memory(chip, OPCODE_GROUP_SNE_VX_BYTE | ((x & 0x0F) << 8) | val);
}

static inline void write_skip_if_registers_equal_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_SE_VX_VY | ((x & 0x0F) << 8) | ((y & 0x0F) << 4));
}

static inline void write_skip_if_registers_not_equal_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_SNE_VX_VY | ((x & 0x0F) << 8) | ((y & 0x0F) << 4));
}

static inline void write_set_register_value_instruction(chip_eight_t *chip, uint8_t x, uint8_t val) {
  write_opcode_to_memory(chip, OPCODE_GROUP_LD_VX_BYTE | ((x & 0x0F) << 8) | val);
}

static inline void write_add_value_to_register_instruction(chip_eight_t *chip, uint8_t x, uint8_t val) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ADD_VX_BYTE | ((x & 0x0F) << 8) | val);
}

static inline void write_copy_register_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_LD);
}

static inline void write_or_registers_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_OR);
}

static inline void write_and_registers_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_AND);
}

static inline void write_xor_registers_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_XOR);
}

static inline void write_add_registers_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_ADD);
}

static inline void write_subtract_registers_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_SUB);
}

static inline void write_shift_right_register_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_SHR);
}

static inline void write_subtract_registers_reverse_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_SUBN);
}

static inline void write_shift_left_register_instruction(chip_eight_t *chip, uint8_t x, uint8_t y) {
  write_opcode_to_memory(chip, OPCODE_GROUP_ARITHMETIC | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | SUB_OP_SHL);
}

static inline void write_set_index_register_instruction(chip_eight_t *chip, uint16_t addr) {
  write_opcode_to_memory(chip, OPCODE_GROUP_LD_I_ADDR | (addr & 0x0FFF));
}

static inline void write_jump_with_offset_instruction(chip_eight_t *chip, uint16_t addr) {
  write_opcode_to_memory(chip, OPCODE_GROUP_JP_V0_ADDR | (addr & 0x0FFF));
}

static inline void write_random_byte_instruction(chip_eight_t *chip, uint8_t x, uint8_t mask) {
  write_opcode_to_memory(chip, OPCODE_GROUP_RND_VX_BYTE | ((x & 0x0F) << 8) | mask);
}

static inline void write_draw_sprite_instruction(chip_eight_t *chip, uint8_t x, uint8_t y, uint8_t height) {
  write_opcode_to_memory(chip, OPCODE_GROUP_DRW_VX_VY_N | ((x & 0x0F) << 8) | ((y & 0x0F) << 4) | (height & 0x0F));
}

static inline void write_skip_if_key_pressed_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_KEY_SKIP | ((x & 0x0F) << 8) | OPCODE_SKP);
}

static inline void write_skip_if_key_not_pressed_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_KEY_SKIP | ((x & 0x0F) << 8) | OPCODE_SKNP);
}

static inline void write_get_delay_timer_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_LD_VX_DT);
}

static inline void write_wait_for_key_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_LD_VX_K);
}

static inline void write_set_delay_timer_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_LD_DT_VX);
}

static inline void write_set_sound_timer_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_LD_ST_VX);
}

static inline void write_add_register_to_index_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_ADD_I_VX);
}

static inline void write_set_index_to_font_character_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_LD_F_VX);
}

static inline void write_store_bcd_representation_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_LD_B_VX);
}

static inline void write_dump_registers_to_memory_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_LD_I_REG);
}

static inline void write_load_registers_from_memory_instruction(chip_eight_t *chip, uint8_t x) {
  write_opcode_to_memory(chip, OPCODE_GROUP_MISC | ((x & 0x0F) << 8) | SUB_OP_LD_REG_I);
}

#endif
