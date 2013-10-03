#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "pstat.h"


int
main(int argc, char *argv[])
{
   int x = settickets(23);
   
   struct pstat *stat1;
   stat1 = malloc(sizeof(*stat1));   
   int y = getpinfo(stat1);


int counter = 0;
for(counter = 0; counter < NPROC; counter++){
   printf(1, "PID %d, \n", stat1->pid[counter]);
   printf(1, "INUSE %d, \n", stat1->inuse[counter]);
   printf(1, "LTICKS %d, \n", stat1->hticks[counter]);   
   printf(1, "HTICKS %d, \n", stat1->lticks[counter]);   
}

      
   
// = malloc(sizeof(stat1));
   y = y+0;
   //printf(1, x);
   printf(1, "hello world %d, \n", x);
  exit();
}
