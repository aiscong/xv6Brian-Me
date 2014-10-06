#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "pstat.h"
#include "ptable.h"
#define MAX_BID 2147483647

int sys_reserve(void){
  int percent = -1;
  if(argint(0, &percent) < 0){
    return -1;
  }
  if(percent < 0 || percent > 100){
    return -1;
  }
  if((total_percent + percent) > 200){
    return -1;
  }
  proc->bid = 0;
  proc->percent = percent;
  total_percent += percent;
  proc->type = 1; //indicate its a reserved type
  return 0;
}

int sys_spot(void){
  int bid = -1;
  if(argint(0, &bid) < 0){
    return -1;
  }

  if( bid < 0 || bid > MAX_BID){
    return -1;
  }
  proc->percent = 0;
  proc->bid = bid;
  proc->type = 0;
  return 0;
}

int sys_getpinfo(void){
  struct pstat *p;
  int i;
  if(argptr(0, (void*)&p, sizeof(*p)) < 0){
    return -1;
  }
  struct proc *proc;
  i = 0;
  for (proc = ptable.proc; proc < &ptable.proc[NPROC]; proc++) {
    //inuse populate
    if(proc->state == UNUSED){
      p->inuse[i] = 0;
    }else{
      p->inuse[i] = 1;
      p->pid[i] = proc->pid;
      p->chosen[i] = proc->chosen;
      p->time[i] = proc->time;
      p->charge[i] = proc->dollcharge + proc->nanocharge*1.0/1000000000;
    }
    i++;
  }
  return 0;
}

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
