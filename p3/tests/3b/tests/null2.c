/* any dereference from first page should fault */
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
   int ppid = getpid();

   if (fork() == 0) {
      uint * badp = (uint*)4095;
      printf(1, "bad dereference (0x0fff): ");
      printf(1, "%x %x\n", badp, *badp);
      // this process should be killed
      printf(1, "TEST FAILED\n");
      kill(ppid);
      exit();
   } else {
      wait();
   }

   printf(1, "TEST PASSED\n");
   exit();
}
