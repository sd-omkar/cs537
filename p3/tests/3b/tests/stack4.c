/* stack should not grow into heap (program must terminate) */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE 4096
#define PGROUNDUP(sz) (((sz)+PGSIZE-1) & ~(PGSIZE-1))

#define assert(x) if (x) {} else { \
  printf(1, "%s: %d ", __FILE__, __LINE__); \
  printf(1, "assert failed (%s)\n", # x); \
  printf(1, "TEST FAILED\n"); \
  exit(); \
}

void
growstack(int n) 
{
  char filler[4096];
  filler[0] = filler[0]; // must use or compiler error...
  if(n > 1)
    growstack(n-1);
}

int
main(int argc, char *argv[])
{
  int ppid = getpid();
  uint sz = (uint) sbrk(0);
  uint stackpage = (160 - 1) * PGSIZE;
  uint guardpage = stackpage - PGSIZE;

  // ensure they actually placed the stack high...
  assert((uint)&ppid == 0x9ffcc);

  // should work fine
  growstack(1);
  stackpage -= PGSIZE;
  guardpage -= PGSIZE;

  // grow heap right below the guard page
  assert((int) sbrk(guardpage - sz) != -1);

  int pid = fork();
  if(pid == 0) {
    // should fail
    growstack(2);
    printf(1, "TEST FAILED\n");
    kill(ppid);
    exit();
  } else {
    wait();
  }

  printf(1, "TEST PASSED\n");
  exit();
}
