/* pass arguments from stack and heap to kernel */
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#undef NULL
#define NULL ((void*)0)

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   exit(); \
}

int global;

int
main(int argc, char *argv[])
{
  /* stack */
  int local = 0;
  assert(dup(local) != -1);

  // ensure stack is actually high...
  assert((uint) &local > 639*1024);

  /* heap */
  char *filename = (char*) malloc(16);
  strcpy(filename, "3b.tmp");
  int fd = open(filename, O_WRONLY|O_CREATE);
  assert(fd != -1);
  close(fd);

  /* global data */
  global = 10;
  assert(sleep(global) != -1);

  /* pointer to memory between stack and heap */
  int *pipefds = (int*) ((uint) sbrk(0));
  assert(pipe(pipefds) == -1);

  /* pointer to memory past the stack */
  pipefds = (int*)(640 * 1024);
  assert(pipe(pipefds) == -1);

  printf(1, "TEST PASSED\n");
  exit();
}
