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
  printf(1,"1\n");
  /* below code/heap */
  str = (char*) 0xfff;
  assert(open(str, O_WRONLY|O_CREATE) == -1);
  printf(1,"2\n");
  /* within heap */
  str = (char*) sbrk(0) - 4;
  strcpy(str, "tmp");
  fd = open(str, O_WRONLY|O_CREATE);
  printf(1,"3\n");
  assert(fd != -1);
  printf(1,"4\n");
  assert(unlink(str) != -1);
  printf(1,"5\n");
  /* spanning heap top */
  str[3] = 'a';
  printf(1,"6\n");
  assert(open(str, O_WRONLY|O_CREATE) == -1);
  printf(1,"7\n");
  /* below stack */
  str = (char*) STACK-1;
  assert(open(str, O_WRONLY|O_CREATE) == -1);
 printf(1,"1\n");
  /* within stack */
  str = (char*) STACK+1024;
  strcpy(str, "tmp");
  printf(1,"2\n");
  printf(1,"%p\n",str);
  fd = open(str, O_WRONLY|O_CREATE);
  printf(1,"3\n");
  assert(fd != -1);
  printf(1,"4\n"); 
  assert(unlink(str) != -1);
  
  /* spanning stack top */
  str = (char*) USERTOP-1;
  str[0] = 'a';
  assert(open(str, O_WRONLY|O_CREATE) == -1);

  printf(1, "TEST PASSED\n");
  exit();
}
