#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#include "param.h"

// print usage info of all procresses
void
printinfo()
{
   struct pstat st;

   int r = getpinfo(&st);
   if (r != 0) {
      exit();
   }

   int i;
   for(i = 1; i < NPROC; i++) {
      if (st.inuse[i]) {
         printf(1, "pid: %d hticks: %d lticks: %d\n", st.pid[i], st.hticks[i], st.lticks[i]);
      }
   }
}

// spin for a while
int
spin() {
   int i, j;
   for (i = 0; i < 10000000; i++) {
      j = i % 11;
   }
   return j;
}

int
main(int argc, char *argv[])
{

   if (argc < 3) {
      printf(1, "Usage: usage ticks tickets1 [tickets2]...\n\n");
      printf(1, "Spawns subprocesses, each of which will run for \n"
            "approximately the given number of ticks. For each ticket\n"
            "ammount given, a child process will spawn and request that\n"
            "number of tickets.\n");
      exit();
   }

   int total = 0;
   int i;
   for (i = 0; i < argc - 2; i++) {
      total += atoi(argv[i+2]);
   }
   int ret = settickets(total);
   if (ret < 0) {
      printf(1, "settickets failed\n");
      exit();
   }

   // pids of children
   int pids[NPROC];

   // spawn children
   for (i = 0; i < argc - 2; i++) {
      pids[i] = fork();
      if (pids[i] == 0) {
         int ret = settickets(atoi(argv[i + 2]));
         if (ret < 0) {
            printf(1, "settickets failed\n");
            exit();
         }
         // spin so we get scheduled
         while(1) {
            spin();
         }
      }
   }

   // print usage info
   int ticks = atoi(argv[1]);
   int start = uptime();
   while(uptime() < start + ticks) {
      sleep(100);
      printf(1, "time: %d\n", uptime() - start);
      printinfo();
   }

   // kill children
   for (i = 0; i < argc - 2; i++) {
      kill(pids[i]);
      wait();
   }

   exit();
}
