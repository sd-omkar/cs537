#include "types.h"
#include "stat.h"
#include "user.h"

#define check(exp, msg) if(exp) {} else {\
   printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
   exit();}

int
main(int argc, char *argv[])
{
   check(settickets(1) == 0, "settickets to one");
   check(settickets(10) == 0, "settickets to ten");
   check(settickets(0) == -1, "settickets to zero should fail");
   check(settickets(-1) == -1, "settickets to negative should fail");

   printf(1, "TEST PASSED");
   exit();
}
