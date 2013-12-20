/* stack should grow automatically on a page fault */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define assert(x) if (x) {} else { \
  printf(1, "%s: %d ", __FILE__, __LINE__); \
  printf(1, "assert failed (%s)\n", # x); \
  printf(1, "TEST FAILED\n"); \
  exit(); \
}

void
recurse(int n) 
{
  if(n > 0)
    recurse(n-1);
}

int
main(int argc, char *argv[])
{
  int pid = fork();
  if(pid == 0) {
    recurse(500); // if the fault is not handled, we will not reach the print
    printf(1, "TEST PASSED\n");
    exit();
  } else {
    wait();
  }
  exit();
}
