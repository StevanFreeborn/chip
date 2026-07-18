#include "munit.h"
#include "chip8.h"
#include "test_helpers.h"

static MunitResult test_init(const MunitParameter params[], void* data) {
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

static MunitResult test_cls(const MunitParameter params[], void* data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    chip.display[i] = 1;
  }

  write_clear_screen_instruction(&chip);

  chip8_step(&chip);

  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
    munit_assert_uint32(chip.display[i], ==, 0);
  }

  munit_assert_uint16(chip.pc, ==, PROGRAM_START + 2);

  return MUNIT_OK;
}

static MunitResult test_ret(const MunitParameter params[], void* data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  chip.sc = 1;
  chip.stack[0] = 1;

  write_return_instruction(&chip);

  chip8_step(&chip);

  munit_assert_uint8(chip.sc, ==, 0);
  munit_assert_uint16(chip.pc, ==, 1);

  return MUNIT_OK;
}

static MunitResult test_group_jump(const MunitParameter params[], void* data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  write_jump_instruction(&chip, 1);

  chip8_step(&chip);

  munit_assert_uint16(chip.pc, ==, 1);

  return MUNIT_OK;
}

static MunitResult test_add_carry(const MunitParameter params[], void* data) {
  (void)params;
  (void)data;

  chip_eight_t chip;
  chip8_init(&chip);

  chip.V[0] = 200;
  chip.V[1] = 100;

  write_add_registers_instruction(&chip, 0, 1);

  chip8_step(&chip);

  munit_assert_uint8(chip.V[15], ==, 1);

  return MUNIT_OK;
}

static MunitTest tests[] = {
  { "/init", test_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/cls", test_cls, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/test_ret", test_ret, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/test_group_jump", test_group_jump, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/test_add_carry", test_add_carry, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
  "/chip8",
  tests,
  NULL,
  1,
  MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
