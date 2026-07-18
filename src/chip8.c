#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const uint8_t fontset[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(chip_eight_t *chip) {
  memset(chip, 0, sizeof(chip_eight_t));
  chip->pc = PROGRAM_START;
  chip->dt = 0;
  chip->st = 0;
  chip->waiting_for_key = false;

  for (int i = 0; i < 80; i++) {
    chip->memory[FONT_START + i] = fontset[i];
  }
}

int chip8_load_rom(chip_eight_t *chip, const char *file_path) {
  FILE *file_ptr = fopen(file_path, "rb");

  if (file_ptr == NULL) {
    return FILE_MISSING;
  }

  fseek(file_ptr, 0, SEEK_END);
  int num_of_bytes = ftell(file_ptr);

  int max_program_size = MEMORY_SIZE - PROGRAM_START;

  if (num_of_bytes > max_program_size) {
    fclose(file_ptr);
    return PROGRAM_TOO_LARGE;
  }

  fseek(file_ptr, 0, SEEK_SET);

  size_t bytes_read = fread(&chip->memory[PROGRAM_START], sizeof(uint8_t), num_of_bytes, file_ptr);
  (void)bytes_read; // Ignore unused warning
  
  fclose(file_ptr);

  return COMPLETED_SUCCESSFULLY;
}

static void draw_sprite(chip_eight_t *chip, uint16_t n, uint16_t x, uint16_t y) {
  uint8_t x_coordinate = chip->V[x] % SCREEN_WIDTH;
  uint8_t y_coordinate = chip->V[y] % SCREEN_HEIGHT;
  chip->V[15] = 0;

  for (int row = 0; row < n; row++) {
    uint8_t sprite_byte = chip->memory[chip->I + row];

    if ((y_coordinate + row) >= SCREEN_HEIGHT) {
      break;
    }

    for (int col = 0; col < 8; col++) {
      if ((x_coordinate + col) >= SCREEN_WIDTH) {
        break;
      }
      
      uint8_t sprite_pixel = sprite_byte & (128 >> col);
      int sprite_screen_index = ((y_coordinate + row) * SCREEN_WIDTH) + (x_coordinate + col);
      uint32_t *screen_pixel = &chip->display[sprite_screen_index];

      if (sprite_pixel != 0) {
        if (*screen_pixel == 1) {
          chip->V[15] = 1;
        }

        *screen_pixel ^= 1;
      }
    }
  }
}

void chip8_step(chip_eight_t *chip) {
  if (chip->waiting_for_key) {
    return;
  }

  uint16_t opcode = (chip->memory[chip->pc] << 8) | (chip->memory[chip->pc + 1]);
  chip->pc += 2;
  
  uint16_t nnn = opcode & 4095;
  uint8_t nn = opcode & 255;
  uint16_t n = opcode & 15;
  uint16_t x = (opcode & 3840) >> 8;
  uint16_t y = (opcode & 240) >> 4;
  uint16_t kk = opcode & 255;

  switch(opcode & 61440) {
    case OPCODE_GROUP_SYS_OR_RET: 
      if (opcode == OPCODE_CLS) {
        memset(chip->display, 0, sizeof(chip->display));
      } else if (opcode == OPCODE_RET) {
        chip->sc -= 1;
        chip->pc = chip->stack[chip->sc];
      }
      break;
    case OPCODE_GROUP_JUMP:
      chip->pc = nnn;
      break;
    case OPCODE_GROUP_CALL:
      chip->stack[chip->sc] = chip->pc;
      chip->sc += 1;
      chip->pc = nnn;
      break;
    case OPCODE_GROUP_SE_VX_BYTE:
      if (chip->V[x] == kk) {
        chip->pc += 2;
      }
      break;
    case OPCODE_GROUP_SNE_VX_BYTE:
      if (chip->V[x] != kk) {
        chip->pc += 2;
      }
      break;
    case OPCODE_GROUP_SE_VX_VY:
      if (chip->V[x] == chip->V[y]) {
        chip->pc += 2;
      }
      break;
    case OPCODE_GROUP_LD_VX_BYTE:
      chip->V[x] = kk;
      break;
    case OPCODE_GROUP_ADD_VX_BYTE:
      chip->V[x] += kk;
      break;
    case OPCODE_GROUP_ARITHMETIC:
      if (n == SUB_OP_LD) {
        chip->V[x] = chip->V[y];
      } else if (n == SUB_OP_OR) {
        chip->V[x] = chip->V[x] | chip->V[y];
      } else if (n == SUB_OP_AND) {
        chip->V[x] = chip->V[x] & chip->V[y];
      } else if (n == SUB_OP_XOR) {
        chip->V[x] = chip->V[x] ^ chip->V[y];
      } else if (n == SUB_OP_ADD) {
        uint16_t r = chip->V[x] + chip->V[y];
        chip->V[15] = (r > 255) ? 1 : 0;
        chip->V[x] = r & 255;
      } else if (n == SUB_OP_SUB) {
        chip->V[15] = (chip->V[x] >= chip->V[y]) ? 1 : 0;
        chip->V[x] = chip->V[x] - chip->V[y];
      } else if (n == SUB_OP_SHR) {
        uint8_t flag = chip->V[x] & 1;
        chip->V[15] = flag;
        chip->V[x] /= 2;
      } else if (n == SUB_OP_SUBN) {
        chip->V[15] = (chip->V[y] >= chip->V[x]) ? 1 : 0;
        chip->V[x] = chip->V[y] - chip->V[x];
      } else if (n == SUB_OP_SHL) {
        uint8_t flag = (chip->V[x] & 128) >> 7;
        chip->V[15] = flag;
        chip->V[x] *= 2;
      }
      break;
    case OPCODE_GROUP_SNE_VX_VY:
      if (chip->V[x] != chip->V[y]) {
        chip->pc += 2;
      }
      break;
    case OPCODE_GROUP_LD_I_ADDR:
      chip->I = nnn;
      break;
    case OPCODE_GROUP_JP_V0_ADDR:
      chip->pc = nnn + chip->V[0];
      break;
    case OPCODE_GROUP_RND_VX_BYTE:
      uint8_t num = rand() % 256;
      chip->V[x] = kk & num;
      break;
    case OPCODE_GROUP_DRW_VX_VY_N:
      draw_sprite(chip, n, x, y);
      break;
    case OPCODE_GROUP_KEY_SKIP:
      if (nn == OPCODE_SKP) {
        if (chip->keys[chip->V[x] & 0x0F] == 1) {
          chip->pc += 2;
        }
      } else if (nn == OPCODE_SKNP) {
        if (chip->keys[chip->V[x] & 0x0F] != 1) {
          chip->pc += 2;
        }
      }
      break;
    case OPCODE_GROUP_MISC:
      if (nn == SUB_OP_LD_VX_DT) {
        chip->V[x] = chip->dt;
      } else if (nn == SUB_OP_LD_VX_K) {
        chip->waiting_for_key = true;
        chip->key_register = x;
      } else if (nn == SUB_OP_LD_DT_VX) {
        chip->dt = chip->V[x];
      } else if (nn == SUB_OP_LD_ST_VX) {
        chip->st = chip->V[x];
      } else if (nn == SUB_OP_ADD_I_VX) {
        chip->I += chip->V[x];
      } else if (nn == SUB_OP_LD_F_VX) {
        chip->I = FONT_START + ((chip->V[x] & 0x0F) * 5);
      } else if (nn == SUB_OP_LD_B_VX) {
        chip->memory[chip->I] = chip->V[x] / 100;
        chip->memory[chip->I + 1] = (chip->V[x] / 10) % 10;
        chip->memory[chip->I + 2] = chip->V[x] % 10;
      } else if (nn == SUB_OP_LD_I_REG) {
        for (int i = 0; i <= x; i++) {
          chip->memory[chip->I + i] = chip->V[i];
        }
      } else if (nn == SUB_OP_LD_REG_I) {
        for (int i = 0; i <= x; i++) {
          chip->V[i] = chip->memory[chip->I + i];
        }
      }
      break;
    default:
      break;
  }
}

void chip8_tick_timers(chip_eight_t *chip) {
  if (chip->dt > 0) {
    chip->dt--;
  }
  if (chip->st > 0) {
    chip->st--;
  }
}
