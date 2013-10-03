#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#define check(exp, msg) if(exp) {} else {\
   printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
   exit();}

int
main(int argc, char *argv[])
{
   struct pstat st;

   sleep(10);
   check(getpinfo(&st) == 0, "getpinfo");

   int count = 0;
   int i;
   for(i = 0; i < NPROC; i++) {
      if (st.inuse[i]) {
         count++;
         printf(1, "pid: %d hticks: %d lticks:%d\n", st.pid[i], st.hticks[i], st.lticks[i]);
         check(st.hticks[i] > 0, "all processes must have run at least once");
      }
   }

   check(count == 3, "should be three processes: init, sh, tester");

   printf(1, "TEST PASSED");
   exit();
}
