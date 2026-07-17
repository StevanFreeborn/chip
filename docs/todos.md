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

  | x | Opcode | Type | Description                  |
  |---|--------|------|------------------------------|
  | x | `00E0` | 00E0 | Clear display                |
  | x | `1nnn` | 1nnn | Jump to address              |
  | x | `3xkk` | 3xkk | Skip next if Vx == kk        |
  | x | `6xkk` | 6xkk | Load register with immediate |
  | x | `7xkk` | 7xkk | Add immediate to Vx          |
  | x | `ANNN` | ANNN | Set index register           |
  | x | `Bnnn` | Bnnn | Jump to V0 + addr            |
  | x | `Dxyn` | Dxyn | Draw sprite                  |

- [x] Refactor main
- [x] Migrate drawing to raylib window

- [ ] Handle keyboard input

- [ ] Implement remaining instructions

  | x | Opcode | Type | Description                  |
  |---|--------|------|------------------------------|
  | x | `00EE` | 00EE | Return from subroutine       |
  | x | `2nnn` | 2nnn | Call subroutine at nnn       |
  | x | `4xkk` | 4xkk | Skip next if Vx != kk        |
  | x | `5xy0` | 5xy0 | Skip next if Vx == Vy        |
  | x | `8xy0` | 8xy0 | Stores Vy in Vx              |
  | x | `8xy1` | 8xy1 ||
  | x | `8xy2` | 8xy2 ||
  | x | `8xy3` | 8xy3 ||
  | x | `8xy4` | 8xy4 ||
  | x | `8xy5` | 8xy5 ||
  | x | `8xy6` | 8xy6 ||
  | x | `8xy7` | 8xy7 ||
  | x | `8xy8` | 8xy8 ||
  | x | `Cxkk` | Cxkk ||
