// Do not modify this file. It will be replaced by the grading scripts
// when checking your project.

#include "types.h"
#include "stat.h"
#include "user.h"

#define FAIL(x)\
   printf(1, "failed: %s\n", x);\
   exit();\

#define SEVERAL 3
#define TEST(CODE, EXPECTED, MSG) \
  printf(1, "calling (%s), should count %d syscalls\n", MSG, EXPECTED);\
  failed = passed = 0;\
  for(i = 0; i < SEVERAL; i++) {\
    before = getsyscallinfo();\
    CODE;\
    after = getsyscallinfo();\
    printf(1, "%d) counted %d syscalls\n", i+1, after-before);\
    if(after - before < EXPECTED) { FAIL("count too low"); }\
    else if(after - before == EXPECTED) { passed++; }\
    else { failed++; }\
  }\
  if (failed > passed) { FAIL("count consistently too high"); };

int
main(int argc, char *argv[])
{
  int i, failed, passed;
  int before, after;

  TEST(;, 1, "getsyscallinfo");

  printf(1, "TEST PASSED");
  exit();
}
