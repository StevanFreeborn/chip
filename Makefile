CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Itests
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = src/main.c src/chip8.c
OBJ = $(SRC:.c=.o)
TARGET_DIR = .bin
TARGET = $(TARGET_DIR)/chip.exe

TEST_SRC = tests/test_chip8.c tests/munit.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_TARGET = $(TARGET_DIR)/test_chip8.exe
TEST_LDFLAGS =

COVERAGE_DIR = $(TARGET_DIR)/coverage

# OS and shell command detection
ifeq ($(OS),Windows_NT)
    SHELL = cmd.exe
    MKDIR = if not exist $(TARGET_DIR) mkdir $(TARGET_DIR)
    MKDIR_COV = if not exist .bin\coverage mkdir .bin\coverage
    CLOSE_CHIP = taskkill /f /im chip.exe 2>NUL || exit 0
    CLEAN_CMD = del /f /q src\*.o tests\*.o 2>NUL & rmdir /s /q $(TARGET_DIR) 2>NUL
else
    MKDIR = mkdir -p $(TARGET_DIR)
    MKDIR_COV = mkdir -p $(COVERAGE_DIR)
    CLOSE_CHIP = killall chip 2>/dev/null || true
    CLEAN_CMD = rm -f src/*.o tests/*.o && rm -rf $(TARGET_DIR)
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CLOSE_CHIP)
	@$(MKDIR)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(TEST_TARGET): src/chip8.o $(TEST_OBJ)
	@$(MKDIR)
	$(CC) src/chip8.o $(TEST_OBJ) -o $(TEST_TARGET) $(TEST_LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@$(CLEAN_CMD)

run: all
	@$(TARGET) $(ROM)

test: $(TEST_TARGET)
	@$(TEST_TARGET)

coverage:
	@$(MAKE) clean
	@$(MAKE) test CFLAGS="$(CFLAGS) --coverage -g -O0" TEST_LDFLAGS="--coverage"
	@echo ====================================================
	@echo              TEST COVERAGE SUMMARY
	@echo ====================================================
	gcovr -r . --exclude tests --exclude include
	@$(MKDIR_COV)
	gcovr -r . --exclude tests --exclude include --html-details -o $(COVERAGE_DIR)/index.html
	@echo HTML report generated at: file:///C:/Users/sfree/Repositories/chip/.bin/coverage/index.html

.PHONY: all clean run test coverage
