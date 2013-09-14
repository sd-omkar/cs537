// Do not modify this file. It will be replaced by the grading scripts
// when checking your project.

#include "types.h"
#include "stat.h"
#include "user.h"

#define assert(x) if (x) { /* pass */ } else { \
   printf(1, "assert failed %s %s %d\n", #x , __FILE__, __LINE__); \
   exit(); \
   }

int
main(int argc, char *argv[])
{
  int n = getsyscallinfo();
  assert(n != -1);
  printf(1, "TEST PASSED");
  exit();
}
