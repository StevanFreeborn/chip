#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

#define MEMORY_SIZE 4096
#define PROGRAM_START 512

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
  uint8_t memory[MEMORY_SIZE] ;
} chip_eight_t;

void println(const char *format, ...) {
  va_list args;

  va_start(args, format);

  vprintf(format, args);

  va_end(args);

  printf("\n");
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    println("You fucked up bro! We need a file to load into memory.");
    println("Usage: chip.exe <file-to-load>");
    return FILE_NOT_PROVIDED;
  }

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
    return PROGRAM_TOO_LARGE;
  }

  fseek(file_ptr, 0, SEEK_SET);

  chip_eight_t *chip = malloc(sizeof(chip_eight_t));

  if (chip == NULL) {
    println("Failed to allocate memory for chip.");
    return MEMORY_ALLOCATION_FAILURE;
  }

  fread(&chip->memory[PROGRAM_START], sizeof(uint8_t), num_of_bytes, file_ptr);

  println("File '%s' loaded successfully", file_path);

  fclose(file_ptr);

  // TODO: Parse instructions
  println("INSTRUCTIONS:");

  for (int i = 0; i < MEMORY_SIZE; i++) {
    println("%d: %x ", i, chip->memory[i]);
  }

  free(chip);

  return COMPLETED_SUCCESSFULLY;
}

