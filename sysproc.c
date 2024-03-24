#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

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
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0) // selecting which will run first 
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
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_shutdown(void)
{
  /* Either of the following will work. Does not harm to put them together. */
  outw(0xB004, 0x0|0x2000); // working for old qemu
  outw(0x604, 0x0|0x2000); // working for newer qemu
  
  return 0;
}

extern int sched_trace_enabled;
extern int sched_trace_counter;
int sys_enable_sched_trace(void)
{
  if (argint(0, &sched_trace_enabled) < 0)
  {
    cprintf("enable_sched_trace() failed!\n");
  }
  
  sched_trace_counter = 0;

  return 0;
}


//for fork_rc_test 0 for parent first 1 child first
int sys_fork_winner(void){
  int runFirst;
  FORK = 0;
  if(argint(0,&runFirst) < 0) return -1;
  if(runFirst == 1) FORK = 1;
  return 0;
}

//proj2 part 2 added here
int sys_set_sched(void){
  int runFirst;
  // SCHEDULER = 0;
  if(argint(0,&runFirst)<0) return -1;
  if(runFirst == 1) SCHEDULER = 1;    
  else SCHEDULER = 0;
  return 0;
}

extern int tickets_owned(int);
extern int transfer_tickets(int pid, int tickets, struct proc *myproc);

int sys_tickets_owned(void){
  int pid;
  argint(0,&pid);
  return tickets_owned(pid);
}
int sys_transfer_tickets(void){
  int pid;
  int tickets;
	argint(0,&pid);
	argint(1,&tickets);
	return transfer_tickets(pid,tickets,myproc());
}
//end