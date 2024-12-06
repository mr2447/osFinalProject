#ifndef STRACE_H
#define STRACE_H

// Include dependencies indirectly through the including files

// Constants for tracing options
#define STRACE_OPTION_NONE 0
#define STRACE_OPTION_FILTER 'e'
#define STRACE_OPTION_SUCCESS 's'
#define STRACE_OPTION_FAIL 'f'

#define N 100 // Number of events in the circular buffer

// Struct for per-process tracing state
struct strace_state {
    int active_option;          // Current active option ('e', 's', 'f', or 0)
    int syscall_filter_id;      // Syscall number to filter for -e
};

// Struct for logging system call events
struct strace_event {
    int pid;                    // Process ID
    char name[16];              // Process name
    char syscall[16];           // System call name
    int retval;                 // System call return value
};

// Circular buffer for logging events
extern struct strace_event strace_buffer[N];
extern int strace_buffer_index;
extern char* syscall_filter_name;

// Function prototypes
void parse_options(int argc, char **argv, struct strace_state *state);
int syscall_number_from_name(const char *name);
int should_log_syscall(int syscall_num, int retval, const struct strace_state *state);
void log_strace_event(int pid, const char *name, const char *syscall, int retval);

#endif // STRACE_H