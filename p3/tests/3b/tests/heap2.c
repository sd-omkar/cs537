/* heap must not grow into the page below stack */
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

int
main(int argc, char *argv[])
{
  uint sz = (uint) sbrk(0);
  uint stackpage = (160 - 1) * 4096;
  uint guardpage = stackpage - 4096;

  assert((int) sbrk(guardpage - sz) != -1);
  assert((int) sbrk(-1*(guardpage - sz)) != -1);
  assert((int) sbrk(guardpage - sz + 1) == -1);
  printf(1, "TEST PASSED\n");
  exit();
}
