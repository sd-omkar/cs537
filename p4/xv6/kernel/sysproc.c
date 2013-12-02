#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_clone(void) {
  // TODO
  void *fcn, *arg, *stack;

  if(argptr(0, (void *)&fcn, sizeof(void *)) < 0)
    return -1;

  if(argptr(1, (void *)&arg, sizeof(void *)) < 0)
    return -1;

  if(argptr(2, (void *)&stack, sizeof(void *)) < 0)
    return -1;

  if ((uint)stack % PGSIZE != 0)
    return -1;

  if ((uint)proc->sz - (uint)stack == PGSIZE/2)
    return -1;

  return clone(fcn, arg, stack);
}

int sys_join(void) {
  // TODO
  void **stack;

  if(argptr(0, (void *)&stack, sizeof(void *)) < 0)
    return -1;

  int ret = join(stack);
  //cprintf("kernel ret = %d\n", ret);
  return ret;
}

int sys_sleepcv(void) {
  void *cv, *temp;

  if(argptr(0, (void *)&cv, sizeof(void *)) < 0)
    return -1;

  if(argptr(1, (void *)&temp, sizeof(void *)) < 0)
    return -1;
  lock_t *lock = (lock_t *)temp;

  //((cond_t *)cv)->lock = lock;

  sleep2(cv, lock);

  //while(xchg(&(lock->flag), 1) != 0);
  return 0;
} 

int sys_wakecv(void) {
  void *cv;
  
  if(argptr(0, (void *)&cv, sizeof(void *)) < 0)
    return -1;

  wakeup2(cv);

  return 0;
}
