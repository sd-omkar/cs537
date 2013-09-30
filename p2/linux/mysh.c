#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>    
#include <ctype.h>

#define IN_SIZE 515
#define ARG_SIZE 515

char prompt_message[6] = "mysh> ";
char error_message[30] = "An error has occurred\n";
char *words[512];
char *dummy_words[512];
char *command[ARG_SIZE];
int is_batch = 0;
int is_redir = 0;
int is_bg = 0;
int is_internal = 0;
char *str_cd = "cd";
char *str_pwd = "pwd";
char *str_exit = "exit";
char *str_wait = "wait";
char *pre_token, *post_token, *dummy_in = NULL;

// Print shell prompt
void print_prompt() {
    if (!is_batch) write(STDOUT_FILENO, prompt_message, strlen(prompt_message)); 
}

// Print error message
void print_error() {
  write(STDERR_FILENO, error_message, strlen(error_message));
}

// Count the words and split the line
int split_line(char *line, char *words[]) {
  char *input = strdup(line);
  int count  = 0;
  while (1) {
    while (isspace(*input)) input++;
    if (*input == '\0') return count;
    words[count++] = input;
    while (!isspace(*input) && *input != '\0') input++;
    if (*input == '\0') return count;
    *input++ = '\0';
  }
}

// Check if 2 strings are equal
int is_equal(char *s1, char *s2) {
  while (*s1 && *s2) {
    if (!(*s1 == *s2))
      return 0;
    s1++;s2++;
  }
  return 1;
}

int main (int argc, char *argv[]) {
  // Decide where to take input from
  FILE *in_file = NULL;
  int fd_out, fd_stdout;
  char input[IN_SIZE];
  int word_count = 0, command_count = 0, i, status;
  pid_t child, child_wait;

  if (argc == 1)
    in_file = stdin;
  else if (argc == 2) {
    is_batch = 1;
    in_file = fopen(argv[1], "r");
    if (in_file == NULL) {
      print_error();
      exit(1);
    }
  }
  else {
    print_error();
    exit(1);
  }

  print_prompt();
  // Parse input lines
  while (fgets(input, IN_SIZE, in_file)) {
    // Input too large
    if (strlen(input) > 513) {
      if (is_batch) write(STDOUT_FILENO, input, strlen(input));
      print_error();
      print_prompt();
      continue;
    }
    // Empty command line allowed
    if (split_line(input, dummy_words) == 0) {
      print_prompt();
      continue;
    }

    // Cannot start with > or &
    if (split_line(input, dummy_words) > 0)
      if (is_equal("&", dummy_words[0]) || 
          is_equal(">", dummy_words[0])) {
        print_error();
        print_prompt();
        continue;
      }

    is_redir = 0;
    is_bg = 0;
    is_internal = 0;
    pre_token = NULL;
    post_token = NULL;
    dummy_in = strdup(input);
    
    if (is_batch) write(STDOUT_FILENO, input, strlen(input));
   
    // Check for redirection
    pre_token = strtok(dummy_in, ">");
    if (strlen(pre_token) != strlen(input)) {
      is_redir = 1;
      post_token = strtok(NULL, ">");
      if (strtok(NULL, ">")) {
        // Too many redirects
        print_error();
        print_prompt();
        continue;
      }
    }
    
    if (!is_redir)
      word_count = split_line(input, words);
    else {
      word_count = split_line(pre_token, words);
      i = split_line(post_token, &(words[word_count]));
      word_count += i;
      if (i < 1) {
        // No output file specified
        print_error();
        print_prompt();
        continue;
      }
      else if (i > 2) {
        // Too many output file arguments
        print_error();
        print_prompt();
        continue;
      }
      else if (i == 2 && !(is_equal(words[word_count - 1], "&"))) {
        // Too many output file arguments
        print_error();
        print_prompt();
        continue;
      }
      else if (i == 1 && (is_equal(words[word_count - 1], "&"))) {
        // No output file specified
        print_error();
        print_prompt();
        continue;
      }
    }
    
    // Check for background job
    if (is_equal("&", words[word_count - 1]) || 
        strlen(words[word_count - 1]) == (1 + strlen(strtok(words[word_count - 1], "&")))) {
      is_bg = 1;
      if (is_equal("&", words[word_count - 1])) {
        words[word_count - 1] = NULL;
        word_count--;
      }
    }

    /*
    for (i=0; i<word_count; i++)
      printf("%s\n", words[i]);
    printf("Redirect = %d\n", is_redir);
    printf("BG = %d\n", is_bg);
    */

    // In case of redirect, check for valid output file
    if (is_redir) {
      fd_stdout = dup(1);
      //fd_out = open(words[word_count-1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      fd_out = open(words[word_count-1], O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
      if (fd_out < 0) {
        print_error();
        print_prompt();
        continue;
      }
      else if (dup2(fd_out, 1) < 0) {
        print_error();
        print_prompt();
        continue;
      }
    }

    // Internal Commands
    // exit
    if (!strcmp(str_exit, words[0])) {
      if (word_count != 1) {
        print_error();
        print_prompt();
        continue;
      }
      else
        exit(0);
    }

    // pwd
    if (!strcmp(str_pwd, words[0])) {
      if (word_count != 1) {
          print_error();
          print_prompt();
          dup2(fd_stdout, 1);
          continue;
      }
      else {
      char path[1000];
      is_internal = 1;
      getcwd(path, sizeof(path));
      write(1, path, strlen(path));
      write(1, "\n", strlen("\n"));
      }
    }

    // cd
    if (!strcmp(str_cd, words[0])) {
      int chd;
      is_internal = 1;
      if (word_count == 1) {
        chd = chdir(getenv("HOME"));
        if (chd != 0) {
          print_error();
          print_prompt();
          dup2(fd_stdout, 1);
          continue;
        }
      }
      else if (word_count == 2) {
        chd = chdir(words[word_count - 1]);
        if (chd != 0) {
          print_error();
          print_prompt();
          dup2(fd_stdout, 1);
          continue;
        }
      }
    }

    // Internal implementation of wait
    if (!strcmp(str_wait, words[0])) {
      if (word_count != 1) {
          print_error();
          print_prompt();
          dup2(fd_stdout, 1);
          continue;
      }
      else {
      while (waitpid(-1, NULL, 0)) {
        if (errno == ECHILD)  break;
      }
      print_prompt();
      dup2(fd_stdout, 1);
      continue;
      }
    }

    // Form command line from words
    // Delete next line and restore at the end
    
    if (is_redir) {
      for (i=0; i < word_count - 1; i++)
        command[i] = strdup(words[i]);
      command_count = i;
      command[command_count] = NULL;
    }
    else {
      for (i=0; i < word_count; i++)
        command[i] = strdup(words[i]);
      command_count = i;
      command[command_count] = NULL;
    }

    // Python feature
    int length = strlen(command[0]);
    if (command[0][length - 3] == '.' && 
        command[0][length - 2] == 'p' && 
        command[0][length - 1] == 'y') {
      for (i=0; i < command_count; i++)
        command[command_count - i] = command[command_count - 1 - i];
      command[0] = "python";
      command_count++;
      command[command_count] = NULL;
    }

    // For all non-internal commands
    // Create child process, call execpvp
    if (!is_internal) {
      child = fork();
      // Executed by child
      if (child == 0) {
        execvp(command[0], command);
        print_error();
      }
      // Fork error
      else if (child == (pid_t)-1){
        print_error();
      }
      // Execute by parent
      else {
        if (!is_bg)
          child_wait = wait(&status);
      }
    }
    
    // End of iteration
    dup2(fd_stdout, 1);
    print_prompt();
  }

  return 0;
}
