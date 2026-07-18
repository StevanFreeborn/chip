#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <raylib.h>

#define MEMORY_SIZE 4096
#define PROGRAM_START 512
#define FONT_START 80
#define CPU_CLOCK_HZ 600
#define TIMER_HZ 60
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

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

const int CYCLES_PER_FRAME = CPU_CLOCK_HZ / TIMER_HZ;

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
  (void)sig;
  keep_running = 0;
}

enum ExitCodes {
  COMPLETED_SUCCESSFULLY,
  FILE_NOT_PROVIDED,
  FILE_MISSING,
  PROGRAM_TOO_LARGE,
  MEMORY_ALLOCATION_FAILURE,
};

// TODO: Add additional members
// i.e. registers
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

void println(const char *format, ...) {
  va_list args;

  va_start(args, format);

  vprintf(format, args);

  va_end(args);

  printf("\n");
}

long get_time() {
  struct timeval tv;

  gettimeofday(&tv, NULL);

  const int ms_per_sec = 1000;
  long ms = tv.tv_sec / ms_per_sec;

  return ms;
}

void draw(chip_eight_t *chip) {
  printf("\033[J\033[H\033[?25l");

  for (int i = 0; i < SCREEN_HEIGHT; i++) {
    for (int j = 0; j < SCREEN_WIDTH; j++) {
      uint32_t screen_pixel = chip->display[i * SCREEN_WIDTH + j];

      if (screen_pixel == 1) {
        printf("█");
      } else {
        printf(" ");
      }
    }

    printf("\n");
  }
}

void setup_graceful_exit() {
  signal(SIGINT, handle_sigint);
  signal(SIGTERM, handle_sigint);
}

int load_rom(chip_eight_t *chip, char *file_path) {
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

  fread(&chip->memory[PROGRAM_START], sizeof(uint8_t), num_of_bytes, file_ptr);
  
  fclose(file_ptr);

  return COMPLETED_SUCCESSFULLY;
}

void init_chip(chip_eight_t *chip) {
  chip->pc = PROGRAM_START;
  chip->dt = 0;
  chip->st = 0;
  memset(chip->display, 0, sizeof(chip->display));
  memset(chip->keys, 0, sizeof(chip->keys));
  chip->waiting_for_key = false;

  for (int i = 0; i < 80; i++) {
    chip->memory[FONT_START + i] = fontset[i];
  }
}

void draw_sprite(chip_eight_t *chip, uint16_t n, uint16_t x, uint16_t y) {
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
      
      // extracting individual bits (pixels) from byte in memory
      uint8_t sprite_pixel = sprite_byte & (128 >> col);

      // flatten 2-d coordinates to index
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

void update_keys(chip_eight_t *chip) {
  chip->keys[1] = IsKeyDown(KEY_ONE);
  chip->keys[2] = IsKeyDown(KEY_TWO);
  chip->keys[3] = IsKeyDown(KEY_THREE);
  chip->keys[12] = IsKeyDown(KEY_FOUR);

  chip->keys[4] = IsKeyDown(KEY_Q);
  chip->keys[5] = IsKeyDown(KEY_W);
  chip->keys[6] = IsKeyDown(KEY_E);
  chip->keys[13] = IsKeyDown(KEY_R);

  chip->keys[7] = IsKeyDown(KEY_A);
  chip->keys[8] = IsKeyDown(KEY_S);
  chip->keys[9] = IsKeyDown(KEY_D);
  chip->keys[14] = IsKeyDown(KEY_F);

  chip->keys[10] = IsKeyDown(KEY_Z);
  chip->keys[0] = IsKeyDown(KEY_X);
  chip->keys[11] = IsKeyDown(KEY_C);
  chip->keys[15] = IsKeyDown(KEY_V);
}

int main(int argc, char *argv[]) {
  setup_graceful_exit();

  if (argc < 2) {
    println("You fucked up bro! We need a file to load into memory.");
    println("Usage: chip.exe <file-to-load>");
    return FILE_NOT_PROVIDED;
  }

  chip_eight_t *chip = malloc(sizeof(chip_eight_t));

  if (chip == NULL) {
    println("Failed to allocate memory for chip.");
    return MEMORY_ALLOCATION_FAILURE;
  }

  init_chip(chip);

  int load_rom_result = load_rom(chip, argv[1]);

  if (load_rom_result != 0) {
    // TODO: Check for return value
    // print helpful error message
    return load_rom_result;
  }

  int scale_factor = 10;
  int display_width = SCREEN_WIDTH * scale_factor;
  int display_height = SCREEN_HEIGHT * scale_factor;
  InitWindow(display_width, display_height, "chip");
  SetTargetFPS(60);

  while(!WindowShouldClose() || !keep_running)
  {
    update_keys(chip);

    if (chip->waiting_for_key) {
      bool key_pressed = false;

      for (int i = 0; i < 16; i++) {
        if (chip->keys[i]) {
          chip->V[chip->key_register] = i;
          chip->waiting_for_key = false;
          key_pressed = true;
          break;
        }
      }

      if (!key_pressed) {
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < SCREEN_HEIGHT; i++) {
          for (int j = 0; j < SCREEN_WIDTH; j++) {
            uint32_t screen_pixel = chip->display[i * SCREEN_WIDTH + j];

            if (screen_pixel == 1) {
              DrawRectangle(j * scale_factor, i * scale_factor, scale_factor, scale_factor, RAYWHITE);
            }
          }
        }

        EndDrawing();
        continue;
      }

    }

    for(int i = 0; i < CYCLES_PER_FRAME; i++) {
      // read 2 bytes at a time from memory
      // reading first 1 byte shifting left 1 byte ORing 2 byte
      uint16_t opcode = (chip->memory[chip->pc] << 8) | (chip->memory[chip->pc + 1]);
      chip->pc += 2;
      
      // extracting the lowest 12 bits of the instruction
      uint16_t nnn = opcode & 4095;

      //extracting the lowest 8 bits of the instruction
      uint8_t nn = opcode & 255;

      //extracting the lowest 4 bits of the instruction
      uint16_t n = opcode & 15;

      // extracting the lower 4 bits of the high byte of the instruction
      uint16_t x = (opcode & 3840) >> 8;

      // extracting the upper 4 bits of the low byte of the instruction
      uint16_t y = (opcode & 240) >> 4;
      
      // extracting the lowest 8 bits of the instruction
      uint16_t kk = opcode & 255;

      switch(opcode & 61440) {
        case 0: 
          if (opcode == 224) { // 00E0
            memset(chip->display, 0, sizeof(chip->display));
          } else if (opcode == 238) { // 00EE
            chip->sc -= 1;
            chip->pc = chip->stack[chip->sc];
          }
          break;
        case 4096: // 1nnn
          chip->pc = nnn;
          break;
        case 8192: // 2nnn
          chip->stack[chip->sc] = chip->pc;
          chip->sc += 1;
          chip->pc = nnn;
          break;
        case 12288: // 3xkk
          if (chip->V[x] == kk) {
            chip->pc += 2;
          }
          break;
        case 16384: // 4xkk
          if (chip->V[x] != kk) {
            chip->pc += 2;
          }
          break;
        case 20480: // 5xy0
          if (chip->V[x] == chip->V[y]) {
            chip->pc += 2;
          }
          break;
        case 24576: // 6xkk
          chip->V[x] = kk;
          break;
        case 28672: // 7xkk
          chip->V[x] += kk;
          break;
        case 32768: // 8xy0
          // TODO: Maybe switch?
          if (n == 0) {
            chip->V[x] = chip->V[y];
          } else if (n == 1) {
            chip->V[x] = chip->V[x] | chip->V[y];
          } else if (n == 2) {
            chip->V[x] = chip->V[x] & chip->V[y];
          } else if (n == 3) {
            chip->V[x] = chip->V[x] ^ chip->V[y];
          } else if (n == 4) {
            uint16_t r = chip->V[x] + chip->V[y];
            chip->V[15] = (r > 255) ? 1 : 0;
            chip->V[x] = r & 255;
          } else if (n == 5) {
            chip->V[15] = (chip->V[x] > chip->V[y]) ? 1 : 0;
            chip->V[x] = chip->V[x] - chip->V[y];
          } else if (n == 6) {
            uint8_t flag = chip->V[x] & 1;
            chip->V[15] = flag;
            chip->V[x] /= 2;
          } else if (n == 7) {
            chip->V[15] = (chip->V[y] > chip->V[x]) ? 1 : 0;
            chip->V[x] = chip->V[y] - chip->V[x];
          } else if (n == 14) {
            uint8_t flag = (chip->V[x] & 128) >> 7;
            chip->V[15] = flag;
            chip->V[x] *= 2;
          }
          break;
        case 36864: // 9xy0
          if (chip->V[x] != chip->V[y]) {
            chip->pc += 2;
          }
          break;
        case 40960: // ANNN
          chip->I = nnn;
          break;
        case 45056: // Bnnn
          chip->pc = nnn + chip->V[0];
          break;
        case 49152: // Cxkk
          uint8_t num = rand() % 256;
          chip->V[x] = kk & num;
          break;
        case 53248: // Dxyn
          draw_sprite(chip, n, x, y);
          break;
        case 57344: // E000
          if (nn == 158) {
            if (chip->keys[chip->V[x]] == 1) {
              chip->pc += 2;
            }
          } else if (nn == 161) {
            if (chip->keys[chip->V[x]] != 1) {
              chip->pc += 2;
            }
          }
          break;
        case 61440: // F000
          if (nn == 7) {
            chip->V[x] = chip->dt;
          } else if (nn == 10) {
            chip->waiting_for_key = true;
            chip->key_register = x;
          } else if (nn == 21) {
            chip->dt = chip->V[x];
          } else if (nn == 24) {
            chip->st = chip->V[x];
          } else if (nn == 30) {
            chip->I += chip->V[x];
          } else if (nn == 41) {
            chip->I = FONT_START + (chip->V[x] * 5);
          } else if (nn == 51) {
            chip->memory[chip->I] = chip->V[x] / 100;
            chip->memory[chip->I + 1] = (chip->V[x] / 10) % 10;
            chip->memory[chip->I + 2] = chip->V[x] % 10;
          } else if (nn == 85) {
            for (int i = 0; i <= x; i++) {
              chip->memory[chip->I + i] = chip->V[i];
            }
          } else if (nn == 101) {
            for (int i = 0; i <= x; i++) {
              chip->V[i] = chip->memory[chip->I + i];
            }
          }
          break;
        default:
          break;
      }
    }

    if (chip->dt > 0) {
      chip->dt--;
    }

    if (chip->st > 0) {
      chip->st++;
      // TODO: Play sound
    } else {
      // TODO: Play sound
    }

    BeginDrawing();
    ClearBackground(BLACK);

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
      for (int j = 0; j < SCREEN_WIDTH; j++) {
        uint32_t screen_pixel = chip->display[i * SCREEN_WIDTH + j];

        if (screen_pixel == 1) {
          DrawRectangle(j * scale_factor, i * scale_factor, scale_factor, scale_factor, RAYWHITE);
        }
      }
    }

    EndDrawing();
  }

  free(chip);

  CloseWindow();

  return COMPLETED_SUCCESSFULLY;
}

