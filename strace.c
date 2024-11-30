#include "types.h"
#include "stat.h"
#include "user.h"


// void strace_log_event(struct proc *p, int syscall_num, int return_value) {
//     struct syscall_event *event = &p->syscalls[p->syscall_index];

//     // Log the event details
//     event->pid = p->pid;
//     safestrcpy(event->command_name, p->name, sizeof(event->command_name));
//     safestrcpy(event->syscall_name, syscall_name(syscall_num), sizeof(event->syscall_name));
//     event->return_value = return_value;

//     // Update the circular buffer index
//     p->syscall_index = (p->syscall_index + 1) % N;
// }

// void strace_dump_events(struct proc *p) {
//     struct syscall_event *event;

//     cprintf("System Call Trace for pid %d (%s):\n", p->pid, p->name);

//     for (int i = 0; i < N; i++) {
//         int idx = (p->syscall_index + i) % N;
//         event = &p->syscalls[idx];

//         // Print only initialized events
//         if (event->pid != 0) {
//             cprintf("TRACE: pid = %d | command_name = %s | syscall = %s | return value = %d\n",
//                     event->pid,
//                     event->command_name,
//                     event->syscall_name,
//                     event->return_value);
//         }
//     }
// }

// int sys_strace_dump(void) {
//     strace_dump_events(proc);  // Dump logs for the current process
//     return 0;
// }
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(2, "Usage: strace <on|off|run> [<command>]\n");
        exit();
    }

    // Handle "on" or "off" tracing
    if (strcmp(argv[1], "on") == 0) {
        strace(1);  // Enable strace
        exit();
    }

    if (strcmp(argv[1], "off") == 0) {
        strace(0);  // Disable strace
        exit();
    }

    // Handle "run" to enable tracing and execute a command
    if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            printf(2, "Usage: strace run <command>\n");
            exit();
        }

        // Enable tracing for the current process
        strace(1);

        // Execute the specified command
        exec(argv[2], &argv[2]);

        // If exec fails
        printf(2, "strace: exec %s failed\n", argv[2]);
        exit();
    }

    // Invalid argument
    printf(2, "Invalid argument. Use 'on', 'off', or 'run <command>'.\n");
    exit();
}