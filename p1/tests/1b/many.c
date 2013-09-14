// Do not modify this file. It will be replaced by the grading scripts
// when checking your project.

#include "types.h"
#include "stat.h"
#include "user.h"

#define FAIL(x)\
   printf(1, "failed: %s\n", x);\
   exit();\

/* Created this macro to increase confidence in whether a test is passing
 * or failing. If they count too few system calls for a test, then it definitely
 * failed. If they count too many, it may be because another process got CPU
 * time and made additional system call/s.
 *
 * I am aware that this now means when we test open("file", 0), we end up
 * opening the file several times, and when we close a file descriptor, we follow that
 * by closing it another several times. We don't really care for the success of the
 * calls, just that they happen.
 */
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

  // fork, exit, wait (kind of hard to test after a fork otherwise due to race conditions)
  int pid;
  TEST(pid = fork(); if(pid == 0) exit(); else wait();, 4, "fork, exit, wait");

  // pipe
  int fd[2];
  TEST(pipe(fd);, 2, "pipe");
  
  // write
  TEST(write(fd[1], "hello", 5);, 2, "write");

  // read
  char str[5];
  TEST(read(fd[0], str, 1);, 2, "read");

  // close
  TEST(close(fd[0]); close(fd[1]);, 3, "close");

  // fork, kill
  TEST(pid = fork(); if(pid == 0) while(1); else kill(pid);, 3, "fork, kill");

  // getpid
  TEST(pid = getpid();, 2, "getpid");

  // sleep
  TEST(sleep(10);, 2, "sleep");

  // uptime
  before = getsyscallinfo();
  TEST(uptime();, 2, "uptime");

  // open, dup
  TEST(fd[0] = open("syscalltest.txt", 0); fd[1] = dup(fd[0]);, 3, "open, dup");

  // fstat
  struct stat fs;
  TEST(fstat(fd[0], &fs);, 2, "fstat");

  // mkdir
  TEST(mkdir("syscalltest");, 2, "mkdir");

  // chdir
  TEST(chdir("syscalltest");, 2, "chdir");

  // link
  TEST(link("syscalltest2.txt", "../syscalltest.txt");, 2, "link");

  // unlink
  TEST(unlink("syscalltest2.txt");, 2, "unlink");

  // now remove the directory we created
  chdir("..");
  unlink("syscalltest");

  // failed exec
  char* execargs[1];
  execargs[0] = "notactuallythenameofanexecutable";
  TEST(pid = exec(execargs[0], execargs);, 2, "exec");

  // mknod
  TEST(mknod("syscalltest", 1, 1);, 2, "mknod");
  unlink("syscalltest");

  // sbrk
  TEST(sbrk(0);, 2, "sbrk");

  printf(1, "TEST PASSED");
  exit();
}
