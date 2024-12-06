#include "types.h"
#include "stat.h"
#include "user.h"
#include "strace.h"

// Static array mapping syscall names to their numbers
struct syscall_map {
    const char *name;
    int number;
};

// Example syscall mapping
static struct syscall_map syscall_table[] = {
    {"fork", 1},
    {"exit", 2},
    {"wait", 3},
    {"pipe", 4},
    {"read", 5},
    {"kill", 6},
    {"exec", 7},
    {"fstat", 8},
    {"chdir", 9},
    {"dup", 10},
    {"getpid", 11},
    {"sbrk", 12},
    {"sleep", 13},
    {"uptime", 14},
    {"open", 15},
    {"write", 16},
    {"mknod", 17},
    {"unlink", 18},
    {"link", 19},
    {"mkdir", 20},
    {"close", 21},
    {"strace", 22}, // Add your custom syscall number here
    {"strace_dump", 23}, // Add your custom dump syscall number here
    {"strace_option", 24}, // Add your custom option syscall number here
    {0, -1} // Sentinel value
};

int syscall_number_from_name(const char *name) {
    int i;
    for (i = 0; syscall_table[i].name != 0; i++) {
        if (strcmp(syscall_table[i].name, name) == 0) {
            return syscall_table[i].number;
        }
    }
    return -1; // Return -1 if the name is not found
}


void usage() {
    printf(1,"Usage:\n");
    printf(1,"  strace on\n");
    printf(1,"  strace off\n");
    printf(1,"  strace run <command> [args...]\n");
    printf(1,"  strace dump\n");
    exit();
}

int main(int argc, char *argv[]) {
    //struct strace_option option; // Store parsed options
    // static int temporary_override = 0; // Tracks temporary overrides
   

    if (argc < 2) {
        usage();
        exit();
    }

    // Parse options
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
           int active_option = STRACE_OPTION_FILTER;
            int syscall_filter_id = syscall_number_from_name(argv[++i]);
            int option_call = 1;
            strace_option(active_option, syscall_filter_id, option_call);
            exit();
        } else if (strcmp(argv[i], "-s") == 0) {
            int active_option = STRACE_OPTION_SUCCESS;

            strace_option(active_option, -1, 1);
            exit();
        } else if (strcmp(argv[i], "-f") == 0) {
            int active_option = STRACE_OPTION_FAIL;
            strace_option(active_option, -1, 1);
            exit();
        }
    }


    // Handle "on" or "off" tracing
    if (strcmp(argv[1], "on") == 0) {
        strace(1);  // Enable strace
        
    }
    else if (strcmp(argv[1], "off") == 0) {
        strace(0);  // Disable strace
    }
    // Handle "run" to enable tracing and execute a command
    else if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            printf(2, "Usage: strace run <command>\n");
        }

        int pid = fork();
        if (pid == 0) {
            // Child process: Enable per-process tracing and run command
            strace(1);
            exec(argv[2], &argv[2]);
            printf(1,"exec failed\n");
            exit();
        } else {
            // Parent process: Wait for child
            wait();
            strace(0);
            exit();
        }
    }
    else if (strcmp(argv[1], "dump") == 0) {
        strace_dump();  // Call the kernel to print the syscall log
    }
    else {
        usage();
    }

    exit();
}
