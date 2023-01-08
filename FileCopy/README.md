# About

- Demonstrate usage of `a = (b=3)`
- Difference between `printf` and syscall `write`:
  - `printf` encapulate write and has buffer of the output; the buffer is in user mode
  - `write` does not have any buffer, anything written to file discriptor takes effect immediately, such as when write to the tty devices, the character immediately shows on the screen
