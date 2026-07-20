# Chip

A CHIP-8 emulator written in C, built as a portfolio project for [boot.dev](https://boot.dev).

CHIP-8 is an interpreted programming language from the late 1970s, originally used on the COSMAC VIP and other early microcomputers. This emulator reproduces the original hardware behavior - loading ROM files into a simulated 4KB memory space, decoding and executing 2-byte opcodes, rendering monochrome graphics, handling keyboard input, and producing sound.

## Features

- **Complete instruction set** - All 35 standard CHIP-8 opcodes implemented
- **64x32 monochrome display** - Rendered at 10x scale via Raylib
- **Keyboard input** - Full hex keypad mapped to QWERTY
- **Sound emulation** - 440Hz sine wave beep when the sound timer is active
- **Delay & sound timers** - Decrement at 60Hz, matching original hardware timing
- **CPU clock** - 600Hz (10 cycles per frame at 60 FPS)
- **Bounds checking** - Validates ROM size, key registers, and memory addresses
- **Unit tests** - Comprehensive test suite using the [munit](https://nemequ.github.io/munit/) framework

## Prerequisites

- **GCC** - via [MSYS2](https://www.msys2.org/) (UCRT64 environment recommended)
- **Raylib** - Install via MSYS2: `pacman -S mingw-w64-ucrt-x86_64-raylib`
- **GNU Make**
- **gcovr** _(optional)_ - For test coverage reports: `pip install gcovr`

## Installation

```bash
# Clone the repository
git clone https://github.com/sfree/chip.git
cd chip

# Build the emulator
make
```

The compiled binary will be placed at `.bin/chip.exe`.

## Usage

```bash
# Run with a ROM file
make run ROM=test_files/ibm_logo.ch8

# Or run the binary directly
.bin/chip.exe <path-to-rom>
```

### Included ROMs

| ROM                             | Description                                         |
| ------------------------------- | --------------------------------------------------- |
| `test_files/ibm_logo.ch8`       | Classic IBM logo - the standard CHIP-8 test program |
| `test_files/space_invaders.ch8` | Space Invaders - a fully playable game              |
| `test_files/test_opcode.ch8`    | Opcode validation ROM                               |

## Testing

```bash
# Run the unit test suite
make test

# Generate a test coverage report (HTML)
make coverage
```

The coverage report is generated at `.bin/coverage/index.html`.

## Architecture

The project is split into two layers:

- **`chip8.c` / `chip8.h`** - Platform-independent CPU emulator. Handles memory, opcode decoding/execution, registers, timers, and display state. No Raylib dependency - portable to other platforms by swapping only the frontend.
- **`main.c`** - Platform-specific frontend built with Raylib. Manages the window, rendering, keyboard input, audio, and the main loop driving the CPU at the correct clock speed.

## Keyboard Mapping

| CHIP-8 Key | QWERTY Key |     | CHIP-8 Key | QWERTY Key |
| ---------- | ---------- | --- | ---------- | ---------- |
| `1`        | `1`        |     | `C`        | `4`        |
| `2`        | `2`        |     | `D`        | `R`        |
| `3`        | `3`        |     | `E`        | `F`        |
| `4`        | `Q`        |     | `A`        | `Z`        |
| `5`        | `W`        |     | `0`        | `X`        |
| `6`        | `E`        |     | `B`        | `C`        |
| `7`        | `A`        |     | `F`        | `V`        |
| `8`        | `S`        |     |            |            |
| `9`        | `D`        |     |            |            |

## License

MIT License - see [LICENSE.md](LICENSE.md).

## Acknowledgments

- [boot.dev](https://boot.dev) - Learning platform
- [Raylib](https://www.raylib.com/) - C library for game programming
- [munit](https://nemequ.github.io/munit/) - Lightweight C unit testing framework
- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) - Opcode specification
