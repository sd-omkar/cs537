/* syscall argument checks (string arg) */
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

void foo() {
  int local[100];
  if((uint)&local >= 150*4096) foo();
}

int
main(int argc, char *argv[])
{
  char *str;
  int fd;

  // ensure stack is actually high...
  uint STACK = 159*4096;
  uint USERTOP = 160*4096;
  assert((uint) &str > STACK);

  /* below code/heap */
  str = (char*) 0xfff;
  assert(open(str, O_WRONLY|O_CREATE) == -1);

  /* within heap */
  str = (char*) sbrk(0) - 4;
  strcpy(str, "tmp");
  fd = open(str, O_WRONLY|O_CREATE);
  assert(fd != -1);
  assert(unlink(str) != -1);

  /* spanning heap top */
  str[3] = 'a';
  assert(open(str, O_WRONLY|O_CREATE) == -1);

  /* below stack */
  str = (char*) STACK-1;
  assert(open(str, O_WRONLY|O_CREATE) == -1);

  /* within stack */
  str = (char*) STACK+1024;
  strcpy(str, "tmp");
  fd = open(str, O_WRONLY|O_CREATE);
  assert(fd != -1);
  assert(unlink(str) != -1);

  /* spanning stack top */
  str = (char*) USERTOP-1;
  str[0] = 'a';
  assert(open(str, O_WRONLY|O_CREATE) == -1);

  printf(1, "TEST PASSED\n");
  exit();
}
