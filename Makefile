CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = src/main.c src/chip8.c
OBJ = $(SRC:.c=.o)
TARGET_DIR = .bin
TARGET = $(TARGET_DIR)/chip.exe

# OS and shell command detection
ifeq ($(OS),Windows_NT)
    MKDIR = if not exist $(TARGET_DIR) mkdir $(TARGET_DIR)
    CLOSE_CHIP = taskkill /f /im chip.exe 2>NUL || exit 0
    CLEAN_CMD = if exist src\main.o del /f src\main.o & if exist src\chip8.o del /f src\chip8.o & if exist $(TARGET_DIR) rmdir /s /q $(TARGET_DIR)
else
    MKDIR = mkdir -p $(TARGET_DIR)
    CLOSE_CHIP = killall chip 2>/dev/null || true
    CLEAN_CMD = rm -f src/*.o && rm -rf $(TARGET_DIR)
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CLOSE_CHIP)
	@$(MKDIR)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(CLEAN_CMD)

run: all
	@$(TARGET) $(ROM)

.PHONY: all clean run
