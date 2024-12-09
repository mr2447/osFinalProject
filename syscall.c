#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "syscall.h"
#include "strace.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint addr, int *ip)
{
  if(addr >= proc->sz || addr+4 > proc->sz)
    return -1;
  *ip = *(int*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint addr, char **pp)
{
  char *s, *ep;

  if(addr >= proc->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)proc->sz;
  for(s = *pp; s < ep; s++)
    if(*s == 0)
      return s - *pp;
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  return fetchint(proc->tf->esp + 4 + 4*n, ip);
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  int i;

  if(argint(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= proc->sz || (uint)i+size > proc->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  int addr;
  if(argint(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);
extern int sys_strace(void);
extern int sys_strace_dump(void);
extern int sys_strace_option(void);
extern int sys_strace_set_output(void);

static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
[SYS_pipe]    sys_pipe,
[SYS_read]    sys_read,
[SYS_kill]    sys_kill,
[SYS_exec]    sys_exec,
[SYS_fstat]   sys_fstat,
[SYS_chdir]   sys_chdir,
[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
[SYS_sbrk]    sys_sbrk,
[SYS_sleep]   sys_sleep,
[SYS_uptime]  sys_uptime,
[SYS_open]    sys_open,
[SYS_write]   sys_write,
[SYS_mknod]   sys_mknod,
[SYS_unlink]  sys_unlink,
[SYS_link]    sys_link,
[SYS_mkdir]   sys_mkdir,
[SYS_close]   sys_close,
[SYS_strace]  sys_strace,
[SYS_strace_dump] sys_strace_dump,
[SYS_strace_option] sys_strace_option,
[SYS_strace_set_output] sys_strace_set_output,
};

static char *syscall_names[] = {
    [SYS_fork]    "fork",
    [SYS_exit]    "exit",
    [SYS_wait]    "wait",
    [SYS_pipe]    "pipe",
    [SYS_read]    "read",
    [SYS_kill]    "kill",
    [SYS_exec]    "exec",
    [SYS_fstat]   "fstat",
    [SYS_chdir]   "chdir",
    [SYS_dup]     "dup",
    [SYS_getpid]  "getpid",
    [SYS_sbrk]    "sbrk",
    [SYS_sleep]   "sleep",
    [SYS_uptime]  "uptime",
    [SYS_open]    "open",
    [SYS_write]   "write",
    [SYS_mknod]   "mknod",
    [SYS_unlink]  "unlink",
    [SYS_link]    "link",
    [SYS_mkdir]   "mkdir",
    [SYS_close]   "close",
    [SYS_strace]  "strace",
    [SYS_strace_dump] "strace_dump",
    [SYS_strace_option] "strace_option",
    [SYS_strace_set_output] "strace_set_output",
};

void itoa(int num, char *str) {
    int i = 0, is_negative = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }
    if (is_negative)
        str[i++] = '-';
    str[i] = '\0';
    int j, k;
    for (j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

char *strcat(char *dest, const char *src) {
    char *d = dest;
    while (*d) d++;           // Move to the end of the destination string
    while ((*d++ = *src++));  // Copy the source string
    return dest;
}


// System call handler
void syscall(void) {
    int num;

    num = proc->tf->eax; // Get the system call number

    if(num == SYS_exit){
        if(proc->strace_option.fail == 0 && 
        (proc->strace_option.syscall_filter_id == -1 || proc->strace_option.syscall_filter_id == SYS_exit)
        && proc->strace_flag){
            cprintf("TRACE: pid = %d | command_name = %s | syscall = exit\n",
            proc->pid, proc->name);
        }
        if(proc->strace_flag){
        log_strace_event(proc->pid, proc->name, "exit", 0);
        }
    }

    if (num > 0 && num < NELEM(syscalls) && syscalls[num]) {

        int return_value = syscalls[num](); // Call the system call handler

        // Store the return value in the trapframe (for user-space access)
        proc->tf->eax = return_value;

        // Get the current process's tracing option
        //struct strace_option *option = &proc->strace_option;
        if (proc->strace_flag && proc->pid != 2 && proc->pid != 1) {
          int success = proc->strace_option.success;
          int fail = proc->strace_option.fail;
          int filter = proc->strace_option.syscall_filter_id != -1;
            // Print the system call details to the console
            if(filter) { //Check if we have the filter option 
              if(proc->strace_option.syscall_filter_id == num){//Check if the syscall number is the same as the filter
                if(success) { //Check if we want to print successful syscalls 
                  if(return_value >= 0){ //Check if the return value is successful
                    cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                        proc->pid,
                        proc->name,
                        syscall_names[num], // System call name
                        return_value);        // Return value
                  }
                }
                else if(fail) { //Check if we want to print failed syscalls
                  if(return_value < 0){ //Check if the return value is failed
                    cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                        proc->pid,
                        proc->name,
                        syscall_names[num], // System call name
                        return_value);        // Return value
                  }
                }
                else { //Otherwise we are only printing filtered syscalls
                  cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                      proc->pid,
                      proc->name,
                      syscall_names[num], // System call name
                      return_value);        // Return value
                }
              }
            }

            else if(success) { //Check if we want to print successful syscalls 
              if(return_value >= 0){ //Check if the return value is successful
                cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                    proc->pid,
                    proc->name,
                    syscall_names[num], // System call name
                    return_value);        // Return value
              }
            }
            else if(fail) { //Check if we want to print failed syscalls
              if(return_value < 0){ //Check if the return value is failed
                cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                    proc->pid,
                    proc->name,
                    syscall_names[num], // System call name
                    return_value);        // Return value
              }
            }
// else if (proc->strace_option.active_option == STRACE_OPTION_OUTPUT) {
//     if (proc->strace_fd >= 0) {
//         struct file *f = proc->ofile[proc->strace_fd];
//         if (!f) {
//             cprintf("Error: File not found for strace_fd %d\n", proc->strace_fd);
//             return;
//         }

//         // Prepare the trace message using a buffer
//         char buf[256];
//         memset(buf, 0, sizeof(buf)); // Clear the buffer

//         // Write formatted trace message to the buffer
//         safestrcpy(buf, "TRACE: pid = ", sizeof(buf));
//         itoa(proc->pid, buf + strlen(buf));
//         safestrcpy(buf + strlen(buf), " | command_name = ", sizeof(buf) - strlen(buf));
//         safestrcpy(buf + strlen(buf), proc->name, sizeof(buf) - strlen(buf));
//         safestrcpy(buf + strlen(buf), " | syscall = ", sizeof(buf) - strlen(buf));
//         safestrcpy(buf + strlen(buf), syscall_names[num], sizeof(buf) - strlen(buf));
//         safestrcpy(buf + strlen(buf), " | return value = ", sizeof(buf) - strlen(buf));
//         itoa(return_value, buf + strlen(buf));
//         safestrcpy(buf + strlen(buf), "\n", sizeof(buf) - strlen(buf));

//         // Calculate the length of the string
//         int len = strlen(buf);

//         // Debug buffer content
//         cprintf("Debug: Trace message: %s", buf);

//         // Write the trace message to the file
//         int written = filewrite(f, buf, len);
//         if (written < 0) {
//             cprintf("Error: filewrite failed\n");
//         } else if (written < len) {
//             cprintf("Warning: Incomplete write. Written %d of %d bytes\n", written, len);
//         } else {
//             cprintf("Debug: Successfully wrote %d bytes\n", written);
//         }
//     } else {
//         cprintf("Error: Invalid strace_fd\n");
//     }
// }
            
            else{ //Otherwise we have no options and we are printing all syscalls
              cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
                    proc->pid,
                    proc->name,
                    syscall_names[num], // System call name
                    return_value);        // Return value
            }
            log_strace_event(proc->pid, proc->name, syscall_names[num], return_value); // Log the event
        }

    } else {
        // Unknown system call
        cprintf("%d %s: unknown sys call %d\n", proc->pid, proc->name, num);
        proc->tf->eax = -1;
    }
}
