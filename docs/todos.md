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
- [ ] Parse individual instructions from binary CHIP-8 file
