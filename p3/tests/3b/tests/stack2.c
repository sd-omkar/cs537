/* stack should grow towards lower addresses as usual */
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
foo(void *mainlocal) 
{
  int local;
  assert((uint) &local < (uint) mainlocal);
}

int
main(int argc, char *argv[])
{
  int local;
  assert((uint)&local > 639*1024);
  foo((void*) &local);
  printf(1, "TEST PASSED\n");
  exit();
}
