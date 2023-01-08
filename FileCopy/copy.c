#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PATH_LEN 100U

void aquire_file_path(char *file_path, const char *promt_msg) {
  /// write to stdout
  write(STDOUT_FILENO, promt_msg, strlen(promt_msg));
  /// Here the user input from terminal will first goes to tty driver, the tty
  /// driver has a input buffer and output buffer for this terminal, also called
  /// tty device, when user press enter key, the line, together with the enter
  /// '\n' will be fed to the foreground program, here it is this line of
  /// program. Only the foreground program can use the input and output buffer
  /// of this tty device. If user input more character than the read function
  /// needs, the unread character will still be in the buffer, and will be
  /// treated as next command that is passed to bash program to execute. The
  /// 'input buffer' is the stdin file descriptor; The 'output buffer' is the
  /// stdout file descriptor.
  /// For more info: http://www.linusakesson.net/programming/tty/
  int cnt = read(STDIN_FILENO, file_path, PATH_LEN);
  /// make the input a null terminated C style string
  if (file_path[cnt - 1] != '\n') {
    printf("Source file path more than 100 character, exit!\n");
    /// drain all rest characters inside stdin(tty driver)
    char c;
    int n = read(STDIN_FILENO, &c, 1);
    while (c != '\n') {
      /// if no character can be read, read function will block at here
      n = read(STDIN_FILENO, &c, 1);
    }
    exit(1);
  } else {
    file_path[cnt - 1] = '\0';
  }

  /// write to stdout for confirmation
  const char *text1 = "File path:\n";
  const char new_line = '\n';
  write(STDOUT_FILENO, text1, strlen(text1));
  write(STDOUT_FILENO, file_path, strlen(file_path));
  write(STDOUT_FILENO, &new_line, 1);
}

int main() {

  int b = 2;
  /// a is assigned 3; (b=3) returns 3
  int a = (b = 3);

  /// printf is a function that is used to print to stdout; the function itself
  /// has buffer area, the buffer area is not written to the file descriptor of
  /// stdout except when: the buffer is full; newline character is met; fflush
  /// manually. printf use syscall write to write to file descriptor.
  /// However, the buffer of printf is in user mode. the syscall write does not
  /// have any buffer at all, anything written into stdout will be immediately
  /// be delivered to pty slave side and passed to master side through line
  /// discipline.(Note that from master to slave also through line discipline
  /// but will be buffered and trimmed by it, when a newline is recieved
  /// by line discipline the line will be passed to the slave side. This is all
  /// determined by the line discipline).
  printf("%u", a);

  /// as a comparison, output to stderr does not use buffer, meaning that
  /// following line will be displayed immediately
  fprintf(stderr, "THIS DO NOT BUFF");

  /// wait to see the difference
  sleep(2);

  /// following line add a newline into the buffer to stdout, so together with
  /// variable a+"Shang\n" will be displayed.
  printf("Shang\n");

  /// write syscall does not buffer, the output will be send to master side of
  /// pty by line discipline of tty driver directly; the output of tty devices
  /// does not buffer; but the user input to master side is buffered and trimmed
  /// by line discipline.
  write(STDOUT_FILENO, "\n", 1);

  /// store source file path string
  char *source_file_path = malloc(PATH_LEN);
  /// store target file path string
  char *dest_file_path = malloc(PATH_LEN);

  /// get source file path
  aquire_file_path(source_file_path, "Please input source file path:\n");
  /// get destination file path
  aquire_file_path(dest_file_path, "Please input destination file path:\n");

  /// open source file
  int src_fd;
  if ((src_fd = open(source_file_path, O_RDONLY)) == -1) {
    printf("Open \"%s\" failed!\n", source_file_path);
    exit(1);
  }
  /// create destination file
  int dst_fd;
  if ((dst_fd = open(dest_file_path, O_RDONLY | O_CREAT | O_EXCL)) == -1) {
    printf("Create \"%s\" failed!\n", dest_file_path);
    exit(1);
  }

  /// free file_path
  free(source_file_path);
  free(dest_file_path);
}
