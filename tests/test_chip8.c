#include "chip8.h"
#include "munit.h"
#include "test_helpers.h"
#include <stdio.h>

static MunitResult test_init(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  munit_assert_uint16(chip.pc, ==, PROGRAM_START);
  munit_assert_uint8(chip.dt, ==, 0);
  munit_assert_uint8(chip.st, ==, 0);
  munit_assert_false(chip.waiting_for_key);

  return MUNIT_OK;
}

static MunitResult test_cls(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  // Fill entire display buffer with active pixels
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    chip.display[i] = 1;
  }

  // Write Clear Screen instruction
  write_clear_screen_instruction(&chip);
  chip8_step(&chip);

  // Verify all pixels are reset to 0
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    munit_assert_uint32(chip.display[i], ==, 0);
  }

  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_ret(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  // Set up stack state as if a subroutine call was active
  chip.sc = 1;
  chip.stack[0] = 0x300;

  // Write Return instruction
  write_return_instruction(&chip);
  chip8_step(&chip);

  // Verify stack pointer decrements and PC is restored 
  munit_assert_uint8(chip.sc, ==, 0);
  munit_assert_uint16(chip.pc, ==, 0x300);

  return MUNIT_OK;
}

static MunitResult test_group_jump(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  // Write Jump instruction
  write_jump_instruction(&chip, 0x400);
  chip8_step(&chip);

  munit_assert_uint16(chip.pc, ==, 0x400);

  return MUNIT_OK;
}

static MunitResult test_group_call(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  // Simulate existing stack depth of 2
  chip.sc = 2; 

  // Write Call instruction
  write_call_instruction(&chip, 0x400);
  chip8_step(&chip);

  // PC jumps to subroutine
  munit_assert_uint16(chip.pc, ==, 0x400);

  // Stack pointer is incremented
  munit_assert_uint8(chip.sc, ==, 3);

  // Pushed return address points to instruction following call
  munit_assert_uint16(chip.stack[2], ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_se_vx_byte(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Skip next instruction
  chip8_init(&chip);
  chip.V[0] = 0x42;
  write_skip_if_register_equal_value_instruction(&chip, 0, 0x42);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 4);

  // Do not skip
  chip8_init(&chip);
  chip.V[0] = 0x10;
  write_skip_if_register_equal_value_instruction(&chip, 0, 0x42);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_sne_vx_byte(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Skip next instruction
  chip8_init(&chip);
  chip.V[0] = 0x10;
  write_skip_if_register_not_equal_value_instruction(&chip, 0, 0x42);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 4);

  // Do not skip
  chip8_init(&chip);
  chip.V[0] = 0x42;
  write_skip_if_register_not_equal_value_instruction(&chip, 0, 0x42);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_se_vx_vy(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Skip
  chip8_init(&chip);
  chip.V[0] = 0x20;
  chip.V[1] = 0x20;
  write_skip_if_registers_equal_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 4);

  // No skip
  chip8_init(&chip);
  chip.V[0] = 0x20;
  chip.V[1] = 0x30;
  write_skip_if_registers_equal_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_sne_vx_vy(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Skip
  chip8_init(&chip);
  chip.V[0] = 0x20;
  chip.V[1] = 0x30;
  write_skip_if_registers_not_equal_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 4);

  // No skip
  chip8_init(&chip);
  chip.V[0] = 0x20;
  chip.V[1] = 0x20;
  write_skip_if_registers_not_equal_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_ld_vx_byte(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  write_set_register_value_instruction(&chip, 2, 0xAB);
  chip8_step(&chip);

  munit_assert_uint8(chip.V[2], ==, 0xAB);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_add_vx_byte(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  chip.V[3] = 10;
  write_add_value_to_register_instruction(&chip, 3, 20);
  chip8_step(&chip);

  munit_assert_uint8(chip.V[3], ==, 30);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_arithmetic(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Copy Vy into Vx
  chip8_init(&chip);
  chip.V[0] = 5;
  chip.V[1] = 10;
  write_copy_register_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 10);

  // OR Vx, Vy
  chip8_init(&chip);
  chip.V[0] = 0x0F;
  chip.V[1] = 0xF0;
  write_or_registers_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 0xFF);

  // AND Vx, Vy
  chip8_init(&chip);
  chip.V[0] = 0x0F;
  chip.V[1] = 0x3F;
  write_and_registers_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 0x0F);

  // XOR Vx, Vy
  chip8_init(&chip);
  chip.V[0] = 0xAA;
  chip.V[1] = 0xFF;
  write_xor_registers_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 0x55);

  // ADD Vx, Vy
  // Carry overflow (200 + 100 = 300 -> 300 % 256 = 44, VF = 1)
  chip8_init(&chip);
  chip.V[0] = 200;
  chip.V[1] = 100;
  write_add_registers_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 44);
  munit_assert_uint8(chip.V[15], ==, 1);

  // No carry overflow (50 + 100 = 150, VF = 0)
  chip8_init(&chip);
  chip.V[0] = 50;
  chip.V[1] = 100;
  write_add_registers_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 150);
  munit_assert_uint8(chip.V[15], ==, 0);

  // SUB Vx, Vy
  // No borrow (100 >= 50 -> VF = 1)
  chip8_init(&chip);
  chip.V[0] = 100;
  chip.V[1] = 50;
  write_subtract_registers_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 50);
  munit_assert_uint8(chip.V[15], ==, 1);

  // Borrow occurs (50 < 100 -> VF = 0)
  chip8_init(&chip);
  chip.V[0] = 50;
  chip.V[1] = 100;
  write_subtract_registers_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 206);
  munit_assert_uint8(chip.V[15], ==, 0);

  // Shift right by 1, VF stores shifted LSB
  // LSB is 1 (5 >> 1 = 2, VF = 1)
  chip8_init(&chip);
  chip.V[0] = 0b00000101;
  write_shift_right_register_instruction(&chip, 0, 0);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 2);
  munit_assert_uint8(chip.V[15], ==, 1);

  // LSB is 0 (4 >> 1 = 2, VF = 0)
  chip8_init(&chip);
  chip.V[0] = 0b00000100;
  write_shift_right_register_instruction(&chip, 0, 0);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 2);
  munit_assert_uint8(chip.V[15], ==, 0);

  // SUBN Vx, Vy
  // No borrow (Vy 100 >= Vx 50 -> VF = 1)
  chip8_init(&chip);
  chip.V[0] = 50;
  chip.V[1] = 100;
  write_subtract_registers_reverse_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 50);
  munit_assert_uint8(chip.V[15], ==, 1);

  // Borrow occurs (Vy 50 < Vx 100 -> VF = 0)
  chip8_init(&chip);
  chip.V[0] = 100;
  chip.V[1] = 50;
  write_subtract_registers_reverse_instruction(&chip, 0, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 206);
  munit_assert_uint8(chip.V[15], ==, 0);

  // Shift left by 1, VF stores shifted MSB
  // MSB is 1 (0b10000001 << 1 = 2, VF = 1)
  chip8_init(&chip);
  chip.V[0] = 0b10000001;
  write_shift_left_register_instruction(&chip, 0, 0);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 2);
  munit_assert_uint8(chip.V[15], ==, 1);

  // MSB is 0 (0b00000001 << 1 = 2, VF = 0)
  chip8_init(&chip);
  chip.V[0] = 0b00000001;
  write_shift_left_register_instruction(&chip, 0, 0);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 2);
  munit_assert_uint8(chip.V[15], ==, 0);

  return MUNIT_OK;
}

static MunitResult test_ld_i_addr(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  write_set_index_register_instruction(&chip, 0x123);
  chip8_step(&chip);

  munit_assert_uint16(chip.I, ==, 0x123);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_jp_v0_addr(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  // Set V0 offset to 5, target base address to 0x300 -> Expect PC = 0x305
  chip.V[0] = 0x05;
  write_jump_with_offset_instruction(&chip, 0x300);
  chip8_step(&chip);

  munit_assert_uint16(chip.pc, ==, 0x305);

  return MUNIT_OK;
}

static MunitResult test_rnd_vx_byte(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  // Mask of 0x00 guarantees result is 0 regardless of random output
  write_random_byte_instruction(&chip, 0, 0x00);
  chip8_step(&chip);

  munit_assert_uint8(chip.V[0], ==, 0x00);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_drw_vx_vy_n(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Draw sprite without collision (VF = 0)
  chip8_init(&chip);
  chip.V[0] = 63;
  chip.V[1] = 31;
  chip.I = 0x300;
  chip.memory[0x300] = 0b11000000;
  chip.memory[0x301] = 0b11000000;

  write_draw_sprite_instruction(&chip, 0, 1, 2);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[15], ==, 0);

  // Draw sprite over an active pixel (VF = 1)
  chip8_init(&chip);
  chip.V[0] = 0;
  chip.V[1] = 0;
  chip.I = 0x300;
  chip.memory[0x300] = 0xFF;
  chip.display[0] = 1;

  write_draw_sprite_instruction(&chip, 0, 1, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[15], ==, 1);

  return MUNIT_OK;
}

static MunitResult test_key_skip(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Key is pressed
  chip8_init(&chip);
  chip.V[0] = 5;
  chip.keys[5] = 1;
  write_skip_if_key_pressed_instruction(&chip, 0);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 4);

  // Key is NOT pressed
  chip8_init(&chip);
  chip.V[0] = 5;
  chip.keys[5] = 0;
  write_skip_if_key_pressed_instruction(&chip, 0);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  // Key is NOT pressed
  chip8_init(&chip);
  chip.V[0] = 5;
  chip.keys[5] = 0;
  write_skip_if_key_not_pressed_instruction(&chip, 0);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 4);

  // Key is pressed
  chip8_init(&chip);
  chip.V[0] = 5;
  chip.keys[5] = 1;
  write_skip_if_key_not_pressed_instruction(&chip, 0);
  chip8_step(&chip);
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_misc_instructions(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Read Delay Timer into Vx
  chip8_init(&chip);
  chip.dt = 42;
  write_get_delay_timer_instruction(&chip, 1);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[1], ==, 42);

  // Wait for key press
  chip8_init(&chip);
  write_wait_for_key_instruction(&chip, 2);
  chip8_step(&chip);
  munit_assert_true(chip.waiting_for_key);
  munit_assert_uint8(chip.key_register, ==, 2);
  chip8_step(&chip); // Stepping while waiting should keep PC unchanged
  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  // Set Delay Timer = Vx
  chip8_init(&chip);
  chip.V[3] = 99;
  write_set_delay_timer_instruction(&chip, 3);
  chip8_step(&chip);
  munit_assert_uint8(chip.dt, ==, 99);

  // Set Sound Timer = Vx
  chip8_init(&chip);
  chip.V[4] = 88;
  write_set_sound_timer_instruction(&chip, 4);
  chip8_step(&chip);
  munit_assert_uint8(chip.st, ==, 88);

  // Set I = I + Vx
  chip8_init(&chip);
  chip.I = 0x200;
  chip.V[5] = 0x50;
  write_add_register_to_index_instruction(&chip, 5);
  chip8_step(&chip);
  munit_assert_uint16(chip.I, ==, 0x250);

  // Set I = Font Character location for digit Vx
  chip8_init(&chip);
  chip.V[0] = 0x0A;
  write_set_index_to_font_character_instruction(&chip, 0);
  chip8_step(&chip);
  munit_assert_uint16(chip.I, ==, FONT_START + (0x0A * 5));

  // Store BCD representation of Vx in memory starting at I
  // 234 -> memory[I] = 2, memory[I+1] = 3, memory[I+2] = 4
  chip8_init(&chip);
  chip.I = 0x300;
  chip.V[0] = 234;
  write_store_bcd_representation_instruction(&chip, 0);
  chip8_step(&chip);
  munit_assert_uint8(chip.memory[0x300], ==, 2);
  munit_assert_uint8(chip.memory[0x301], ==, 3);
  munit_assert_uint8(chip.memory[0x302], ==, 4);

  // Dump registers V0 through Vx into memory starting at address I
  chip8_init(&chip);
  chip.I = 0x400;
  chip.V[0] = 10;
  chip.V[1] = 20;
  chip.V[2] = 30;
  write_dump_registers_to_memory_instruction(&chip, 2);
  chip8_step(&chip);
  munit_assert_uint8(chip.memory[0x400], ==, 10);
  munit_assert_uint8(chip.memory[0x401], ==, 20);
  munit_assert_uint8(chip.memory[0x402], ==, 30);

  // Load registers V0 through Vx from memory starting at address I
  chip8_init(&chip);
  chip.I = 0x400;
  chip.memory[0x400] = 11;
  chip.memory[0x401] = 22;
  chip.memory[0x402] = 33;
  write_load_registers_from_memory_instruction(&chip, 2);
  chip8_step(&chip);
  munit_assert_uint8(chip.V[0], ==, 11);
  munit_assert_uint8(chip.V[1], ==, 22);
  munit_assert_uint8(chip.V[2], ==, 33);

  return MUNIT_OK;
}

static MunitResult test_timers_and_rom(const MunitParameter params[], void *data) {
  (void)params;
  (void)data;

  chip_eight_t chip;

  // Verify timer tick countdown logic
  chip8_init(&chip);
  chip.dt = 2;
  chip.st = 1;
  chip8_tick_timers(&chip);
  munit_assert_uint8(chip.dt, ==, 1);
  munit_assert_uint8(chip.st, ==, 0);
  chip8_tick_timers(&chip);
  munit_assert_uint8(chip.dt, ==, 0);
  munit_assert_uint8(chip.st, ==, 0);

  // Verify chip8_load_rom error when file does not exist
  chip8_init(&chip);
  munit_assert_int(chip8_load_rom(&chip, "non_existent_rom.ch8"), ==, FILE_MISSING);

  // Verify chip8_load_rom success with valid ROM file
  munit_assert_int(chip8_load_rom(&chip, "test_files/ibm_logo.ch8"), ==, COMPLETED_SUCCESSFULLY);

  // Verify chip8_load_rom error when ROM exceeds maximum allowable memory
  FILE *f = fopen("test_files/too_large.ch8", "wb");

  if (f != NULL) {
    uint8_t dummy[3600] = {0};

    fwrite(dummy, 1, sizeof(dummy), f);
    fclose(f);

    munit_assert_int(chip8_load_rom(&chip, "test_files/too_large.ch8"), ==, PROGRAM_TOO_LARGE);

    remove("test_files/too_large.ch8");
  }

  // Unrecognized opcode step branch
  write_opcode_to_memory(&chip, 0x0000);
  chip8_step(&chip);

  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/init", test_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/cls", test_cls, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_ret", test_ret, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_group_jump", test_group_jump, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_group_call", test_group_call, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_se_vx_byte", test_se_vx_byte, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_sne_vx_byte", test_sne_vx_byte, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_se_vx_vy", test_se_vx_vy, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_sne_vx_vy", test_sne_vx_vy, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_ld_vx_byte", test_ld_vx_byte, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_add_vx_byte", test_add_vx_byte, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_arithmetic", test_arithmetic, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_ld_i_addr", test_ld_i_addr, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_jp_v0_addr", test_jp_v0_addr, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_rnd_vx_byte", test_rnd_vx_byte, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_drw_vx_vy_n", test_drw_vx_vy_n, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_key_skip", test_key_skip, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_misc_instructions", test_misc_instructions, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_timers_and_rom", test_timers_and_rom, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/chip8", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
