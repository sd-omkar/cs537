#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define IN_SIZE 515
#define ARG_SIZE 515

char prompt_message[6] = "mysh> ";
char error_message[30] = "An error has occurred\n";
char *words[100];
int is_batch = 0;
int is_redir = 0;
int is_bg = 0;
char *str_cd = "cd";
char *str_pwd = "pwd";
char *str_exit = "exit";
char *str_wait = "wait";
char *pre_token, *post_token, *dummy_in = NULL;

// Print shell prompt
void print_prompt() {
  if (!is_batch)
    write(STDOUT_FILENO, prompt_message, strlen(prompt_message)); 
}

// Print error message
void print_error() {
  write(STDERR_FILENO, error_message, strlen(error_message));
}

// Count the words and split the line
int split_line(char *line, char *words[]) {
  char *input = line;
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
  char input[IN_SIZE];
  int word_count = 0, i;

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
    int is_redir = 0;
    int is_bg = 0;
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
      else {
        // Check for valid output file
      }
    }
    /*
    for (i=0; i<word_count; i++)
      printf("%s\n", words[i]);
    */
    
    // Check for background job
    if (is_equal("&", words[word_count - 1]))
      is_bg = 1;

    // Form command line from words

    print_prompt();
  }

  return 0;
}
