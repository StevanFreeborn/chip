#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <windows.h>
#include <signal.h>
#include <string.h>

#define MEMORY_SIZE 4096
#define PROGRAM_START 512
#define CPU_CLOCK_HZ 600
#define TIMER_HZ 60
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

const int CYCLES_PER_FRAME = CPU_CLOCK_HZ / TIMER_HZ;

volatile sig_atomic_t keep_running = 1;

void handle_sigint() {
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
  uint32_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
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

int main(int argc, char *argv[]) {
  // Sets up Ctrl+C handling
  signal(SIGINT, handle_sigint);
  signal(SIGTERM, handle_sigint);

  // Validate file argument
  if (argc < 2) {
    println("You fucked up bro! We need a file to load into memory.");
    println("Usage: chip.exe <file-to-load>");
    return FILE_NOT_PROVIDED;
  }

  // Load file
  char* file_path = argv[1];
  println("Loading file at %s...", file_path);

  FILE *file_ptr = fopen(file_path, "rb");

  if (file_ptr == NULL) {
    println("File '%s' does not exist.", file_path);
    return FILE_MISSING;
  }

  fseek(file_ptr, 0, SEEK_END);
  int num_of_bytes = ftell(file_ptr);

  int max_program_size = MEMORY_SIZE - PROGRAM_START;

  if (num_of_bytes > max_program_size) {
    println("File at '%s' exceeds max program size of %d", file_path, max_program_size);
    fclose(file_ptr);
    return PROGRAM_TOO_LARGE;
  }

  fseek(file_ptr, 0, SEEK_SET);

  chip_eight_t *chip = malloc(sizeof(chip_eight_t));

  if (chip == NULL) {
    println("Failed to allocate memory for chip.");
    fclose(file_ptr);
    return MEMORY_ALLOCATION_FAILURE;
  }

  fread(&chip->memory[PROGRAM_START], sizeof(uint8_t), num_of_bytes, file_ptr);
  
  chip->pc = PROGRAM_START;
  chip->dt = 0;
  chip->st = 0;
  memset(chip->display, 0, sizeof(chip->display));

  println("File '%s' loaded successfully", file_path);

  fclose(file_ptr);

  // main loop
  while(keep_running) {
    if (chip->pc > MEMORY_SIZE) {
      break;
    }

    long frame_start = get_time();

    for(int i = 0; i < CYCLES_PER_FRAME; i++) {

      // we need to read 2 bytes at at time:
      // i.e. 00000000 00000000
      // byte 1: 00011000 
      // shift:  00011000 00000000
      // byte 2:          11100110
      // combine: |       11100110
      //
      // result: 00011000 11100110
      uint16_t opcode = (chip->memory[chip->pc] << 8) | (chip->memory[chip->pc + 1]);
      chip->pc += 2;
      
      // nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
      // i.e. 00001111 11111111
      // value:  11001111 11001100 (53196)
      // oper:   &
      // mask:   0000111111111111  (4095)
      // result: 00001111 11001100 (4044)
      uint16_t nnn = opcode & 4095;

      // n or nibble - A 4-bit value, the lowest 4 bits of the instruction
      // i.e. 00000000 00001111
      // value:  11001111 11001100 (53196)
      // oper: &
      // mask: 00000000 00001111 (15)
      uint16_t n = opcode & 15;

      // x - A 4-bit value, the lower 4 bits of the high byte of the instruction
      // i.e. 00001111 00000000
      // oper: >> 8
      // oper: &
      // mask: 00001111 00000000 (3840)
      uint16_t x = (opcode & 3840) >> 8;

      // y - A 4-bit value, the upper 4 bits of the low byte of the instruction
      // i.e. 00000000 11110000
      // oper: >> 4 
      // oper: &
      // mask: 00000000 11110000 (240)
      uint16_t y = (opcode & 240) >> 4;
      
      // kk or byte - An 8-bit value, the lowest 8 bits of the instruction
      // i.e. 00000000 11111111
      // oper: &
      // mask: 00000000 11111111 (255)
      uint16_t kk = opcode & 255;

      // mask: 11110000 00000000
      switch(opcode & 61440) {
        // Clear display (00E0)
        case 0:
          if (opcode == 224) {
            // println("Clean the display");
            memset(chip->display, 0, sizeof(chip->display));
            draw(chip);
          }
          break;
        case 4096:
          // println("Jump to address: %d", nnn);
          chip->pc = nnn;
          break;
        case 12288:
          // println("Skip next");

          if (chip->V[x] == kk) {
            // println("Skipping");
            chip->pc += 2;
          } else {
            // println("Not skipping");
          }

          break;
        case 24576:
          // println("Load register with immedate");
          chip->V[x] = kk;
          break;
        case 28672:
          // println("Add immedate");
          chip->V[x] += kk;
          break;
        case 40960:
          // println("Set index register");
          chip->I = nnn;
          break;
        case 45056:
          // println("Jump to V0 + addr");
          chip->pc = nnn + chip->V[0];
          break;
        case 53248:
          // println("Draw sprite");
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
              
              // 0 0 0 0 0 0 0 0
              // 1 0 0 0 0 0 0 0
              // 0 1 0 0 0 0 0 0
              // 0 0 1 0 0 0 0 0
              uint8_t sprite_pixel = sprite_byte & (128 >> col);

              // ****
              // **x*
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


          draw(chip);
          break;
        default:
          // println("Unhandled opcode: %x", opcode);
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
    
    float min_diff = 16.66;
    long frame_end = get_time();
    long diff = frame_end - frame_start;

    if (diff < min_diff) {
      Sleep(min_diff - diff);
    }
  }

  free(chip);

  return COMPLETED_SUCCESSFULLY;
}

