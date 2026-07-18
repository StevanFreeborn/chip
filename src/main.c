#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <math.h>
#include <raylib.h>
#include "chip8.h"

#define CPU_CLOCK_HZ 600
#define TIMER_HZ 60

const int CYCLES_PER_FRAME = CPU_CLOCK_HZ / TIMER_HZ;

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
  (void)sig;
  keep_running = 0;
}

void println(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  printf("\n");
}

void setup_graceful_exit() {
  signal(SIGINT, handle_sigint);
  signal(SIGTERM, handle_sigint);
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
    println("We need a file to load into memory.");
    println("Usage: chip.exe <file-to-load>");
    return FILE_NOT_PROVIDED;
  }

  chip_eight_t *chip = malloc(sizeof(chip_eight_t));

  if (chip == NULL) {
    println("Failed to allocate memory for chip.");
    return MEMORY_ALLOCATION_FAILURE;
  }

  chip8_init(chip);

  int load_rom_result = chip8_load_rom(chip, argv[1]);

  if (load_rom_result != COMPLETED_SUCCESSFULLY) {
    if (load_rom_result == FILE_MISSING) {
      println("Error: The file '%s' could not be found or opened.", argv[1]);
    } else if (load_rom_result == PROGRAM_TOO_LARGE) {
      println("Error: The program is too large to fit in CHIP-8 memory.");
    } else {
      println("Error: Failed to load ROM (code %d).", load_rom_result);
    }
    free(chip);
    return load_rom_result;
  }

  // Setup audio
  InitAudioDevice();

  // Generate 440Hz Sine wave beep in memory
  int sampleRate = 44100;
  float durationSeconds = 0.1f;
  int frameCount = sampleRate * durationSeconds;
  float *data = (float *)malloc(frameCount * sizeof(float));
  if (data != NULL) {
    for (int i = 0; i < frameCount; i++) {
      data[i] = sinf(2.0f * PI * 440.0f * ((float)i / sampleRate)) * 0.2f;
    }
  }
  Wave wave = { 0 };
  wave.frameCount = frameCount;
  wave.sampleRate = sampleRate;
  wave.sampleSize = 32;
  wave.channels = 1;
  wave.data = data;

  Sound beep_sound = LoadSoundFromWave(wave);
  UnloadWave(wave); // Frees the 'data' buffer in system RAM

  int scale_factor = 10;
  int display_width = SCREEN_WIDTH * scale_factor;
  int display_height = SCREEN_HEIGHT * scale_factor;
  InitWindow(display_width, display_height, "chip");
  SetTargetFPS(60);

  while(!WindowShouldClose() && keep_running)
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
      if (chip->waiting_for_key) {
        break;
      }
      chip8_step(chip);
    }

    bool play_beep = (chip->st > 0);
    chip8_tick_timers(chip);

    if (play_beep) {
      if (!IsSoundPlaying(beep_sound)) {
        PlaySound(beep_sound);
      }
    } else {
      if (IsSoundPlaying(beep_sound)) {
        StopSound(beep_sound);
      }
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

  UnloadSound(beep_sound);
  CloseAudioDevice();

  CloseWindow();

  return COMPLETED_SUCCESSFULLY;
}
