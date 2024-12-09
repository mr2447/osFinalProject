#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "strace.h"

int strcmp(const char *p, const char *q) {
  while (*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

int sys_strace(void) {
    int mode;

    // Retrieve the argument (0 for off, 1 for on)
    if (argint(0, &mode) < 0) {
        return -1; // Error in retrieving argument
    }

    if(mode == 0){ // If strace is being turned off then reset the strace option
        proc->parent->strace_option.syscall_filter_id = -1;
        proc->parent->strace_option.success = 0;
        proc->parent->strace_option.fail = 0;
        proc->parent->strace_option.option_call = 0;
    }

    // Set the global strace mode
    proc->parent->strace_flag = mode;
    proc->strace_flag = mode;
    // cprintf("strace flag sysproc: %d\n", proc->strace_flag);
    // cprintf("proc->pid: %d\n", proc->pid);
    return 0; // Success
}

int sys_strace_option(void){
    int syscall_filter_id;
    int success;
    int fail;
    int option_call;

    if(proc->strace_flag == 0) { //If strace is not enabled then options don't work
        cprintf("Strace is not enabled\n");
        return -1; // Error in retrieving argument
    }

    //Retrieve the active option
    if (argint(0, &syscall_filter_id) < 0) {
        return -1; // Error in retrieving argument
    }

    if (argint(1, &success) < 0) {
        return -1; // Error in retrieving argument
    }

    if (argint(2, &fail) < 0) {
        return -1; // Error in retrieving argument
    }

    if (argint(3, &option_call) < 0) {
        return -1; // Error in retrieving argument
    }

    // Set sh process's strace option to pass onto next command
    proc->parent->strace_option.syscall_filter_id = syscall_filter_id;
    proc->parent->strace_option.success = success;
    proc->parent->strace_option.fail = fail;
    proc->parent->strace_option.option_call = option_call;
    proc->strace_option.option_call = option_call;
    return 0; // Success
}

int sys_strace_set_output(void) {
    int fd;

    if(proc->strace_flag == 0) { //If strace is not enabled then options don't work
        cprintf("Strace is not enabled\n");
        return -1; // Error in retrieving argument
    }

    // Retrieve the file descriptor from user space
    if (argint(0, &fd) < 0) {
        return -1; // Invalid argument
    }

    // Validate the file descriptor
    if (fd < 0 || fd >= NOFILE || proc->ofile[fd] == 0) {
        return -1; // Invalid or unopened file descriptor
    }

    // Save the file descriptor for strace output
    proc->parent->strace_fd = fd;
    proc->strace_fd = fd;
    cprintf("strace_fd: %d\n", proc->strace_fd);
    cprintf("strace_fd Parent: %d\n", proc->parent->strace_fd);
    return 0; // Success
}

int sys_strace_dump(void) {
    // Print all events in the circular buffer
    int i;
    for (i = 0; i < N; i++) {
        struct strace_event *event = &strace_buffer[i];
        if (event->pid != 0) { // Check if the slot is populated
            if(strcmp(event->syscall, "exit") == 0){
            cprintf("TRACE: pid = %d | command_name = %s | syscall = %s\n",
                    event->pid, event->name, event->syscall);
            }
            else{
              cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                    event->pid, event->name, event->syscall, event->retval);
            }
        }
    }
    return 0; // Success
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
