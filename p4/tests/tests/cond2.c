/* test cv_wait and cv_signal, don't wakeup on wrong cv */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

int ppid;
int global = 0;
lock_t lock;
cond_t cond1, cond2;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void worker(void *arg_ptr);

int
main(int argc, char *argv[])
{
   ppid = getpid();

   lock_init(&lock);


   int thread_pid = thread_create(worker, 0);
   assert(thread_pid > 0);

   sleep(50);
   lock_acquire(&lock);
   global = 2;
   cv_signal(&cond2);
   lock_release(&lock);

   sleep(50);
   lock_acquire(&lock);
   global = 1;
   cv_signal(&cond1);
   lock_release(&lock);

   int join_pid = thread_join();
   assert(join_pid == thread_pid);

   printf(1, "TEST PASSED\n");
   exit();
}

void
worker(void *arg_ptr) {
  lock_acquire(&lock);
  assert(global == 0);
  cv_wait(&cond1, &lock);
  assert(global == 1);
  lock_release(&lock);
  exit();
}

