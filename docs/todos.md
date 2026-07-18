# Todos

- [x] How to capture arguments passed to a C program

      ```c
          // argc is the number of args passed
          // argc is always 1 or more
          // argc always has name/path of program as first arg
          int main(int argc, char *argv[]) {
          // do stuff with the args
          }
          ```

- [x] How to print argument value

      ```
          printf("%s", argv[1]);
          ```

- [x] Read a file

      ```c
          // open file in read mode. returns NULL if doesn't exist
          FILE *file_ptr = fread(file_path, "r");

          // seeking to end of file.
          // useful telling number of bytes
          fseek(file_ptr, 0, SEEK_END);

          // get number of bytes
          int num_of_bytes = ftell(file_ptr);

          // seek to begin of file
          fseek(file_ptr, 0, SEEK_SET);
          ```

- [x] Store contents of file in memory
- [x] Print file contents
- [x] Print instructions
- [x] Make sure program occupies correct space in chip memory
- [x] Parse individual instructions from binary CHIP-8 file
- [x] Print all instructions from CHIP-8 file
- [x] Write all CHIP-8 IBM logo opcodes to CSV file
- [x] Implement opcodes present in the IMB logo CHIP-8 file

  | x   | Opcode | Type | Description                  |
  | --- | ------ | ---- | ---------------------------- |
  | x   | `00E0` | 00E0 | Clear display                |
  | x   | `1nnn` | 1nnn | Jump to address              |
  | x   | `3xkk` | 3xkk | Skip next if Vx == kk        |
  | x   | `6xkk` | 6xkk | Load register with immediate |
  | x   | `7xkk` | 7xkk | Add immediate to Vx          |
  | x   | `ANNN` | ANNN | Set index register           |
  | x   | `Bnnn` | Bnnn | Jump to V0 + addr            |
  | x   | `Dxyn` | Dxyn | Draw sprite                  |

- [x] Refactor main
- [x] Migrate drawing to raylib window
- [x] Handle keyboard input

    CHIP-8 Pad                  QWERTY Keyboard
    ┌───┬───┬───┬───┐           ┌───┬───┬───┬───┐
    │ 1 │ 2 │ 3 │ C │           │ 1 │ 2 │ 3 │ 4 │
    ├───┼───┼───┼───┤           ├───┼───┼───┼───┤
    │ 4 │ 5 │ 6 │ D │   ====>   │ Q │ W │ E │ R │
    ├───┼───┼───┼───┤           ├───┼───┼───┼───┤
    │ 7 │ 8 │ 9 │ E │           │ A │ S │ D │ F │
    ├───┼───┼───┼───┤           ├───┼───┼───┼───┤
    │ A │ 0 │ B │ F │           │ Z │ X │ C │ V │
    └───┴───┴───┴───┘           └───┴───┴───┴───┘

- [x] Implement remaining instructions

  |  x  | Opcode | Type | Description                                                     |
  | --- | ------ | ---- | --------------------------------------------------------------- |
  |  x  | `00EE` | 00EE | Return from subroutine                                          |
  |  x  | `2nnn` | 2nnn | Call subroutine at nnn                                          |
  |  x  | `4xkk` | 4xkk | Skip next if Vx != kk                                           |
  |  x  | `5xy0` | 5xy0 | Skip next if Vx == Vy                                           |
  |  x  | `8xy0` | 8xy0 | Stores Vy in Vx                                                 |
  |  x  | `8xy1` | 8xy1 | Set Vx = Vx OR Vy                                               |
  |  x  | `8xy2` | 8xy2 | Set Vx = Vx AND Vy                                              |
  |  x  | `8xy3` | 8xy3 | Set Vx = Vx XOR Vy                                              |
  |  x  | `8xy4` | 8xy4 | Set Vx = Vx + Vy, set VF = carry                                |
  |  x  | `8xy5` | 8xy5 | Set Vx = Vx - Vy, set VF = NOT borrow                           |
  |  x  | `8xy6` | 8xy6 | Set Vx = Vx SHR 1                                               |
  |  x  | `8xy7` | 8xy7 | Set Vx = Vy - Vx, set VF = NOT borrow                           |
  |  x  | `8xyE` | 8xyE | Set Vx = Vx SHL 1                                               |
  |  x  | `9xy0` | 9xy0 | Skip next if Vx != Vy                                           |
  |  x  | `Cxkk` | Cxkk | Set Vx = random byte AND kk                                     |
  |  x  | `Ex9E` | Ex9E | Skip next if key Vx is pressed                                  |
  |  x  | `ExA1` | ExA1 | Skip next if key Vx is not pressed                              |
  |  x  | `Fx07` | Fx07 | Set Vx = delay timer value                                      |
  |  x  | `Fx0A` | Fx0A | Wait for key press, store key in Vx                             |
  |  x  | `Fx15` | Fx15 | Set delay timer = Vx                                            |
  |  x  | `Fx18` | Fx18 | Set sound timer = Vx                                            |
  |  x  | `Fx1E` | Fx1E | Set I = I + Vx                                                  |
  |  x  | `Fx29` | Fx29 | Set I = location of sprite for digit Vx                         |
  |  x  | `Fx33` | Fx33 | Store BCD representation of Vx in memory                        |
  |  x  | `Fx55` | Fx55 | Store registers V0 through Vx in memory starting at location I  |
  |  x  | `Fx65` | Fx65 | Read registers V0 through Vx from memory starting at location I |
  |  -  | `0nnn` | 0nnn | Jump to machine code routine (ignored)                          |
