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
    {"write", 5},
    {"read", 6},
    {"close", 7},
    {"kill", 8},
    {"exec", 9},
    {"open", 10},
    {"mknod", 11},
    {"unlink", 12},
    {"fstat", 13},
    {"link", 14},
    {"mkdir", 15},
    {"chdir", 16},
    {"dup", 17},
    {"getpid", 18},
    {"sbrk", 19},
    {"sleep", 20},
    {"uptime", 21},
    {"strace", 22}, // Add your custom syscall number here
    {"strace_dump", 23}, // Add your custom dump syscall number here
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

void parse_options(int argc, char **argv, struct strace_state *state) {
    state->active_option = STRACE_OPTION_NONE;   // Reset active option
    state->syscall_filter_id = -1;               // Reset filter

    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            state->active_option = STRACE_OPTION_FILTER;
            state->syscall_filter_id = syscall_number_from_name(argv[++i]);
            break;
        } else if (strcmp(argv[i], "-s") == 0) {
            state->active_option = STRACE_OPTION_SUCCESS;
            break;
        } else if (strcmp(argv[i], "-f") == 0) {
            state->active_option = STRACE_OPTION_FAIL;
            break;
        }
    }
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
    struct strace_state state; // Store parsed options
    // static int temporary_override = 0; // Tracks temporary overrides


    if (argc < 2) {
        usage();
        exit();
    }


    // if (temporary_override) {
    //     // Handle the next command after a temporary override
    //     int pid = fork();
    //     if (pid == 0) {
    //         // Child process: Apply the tracing state and execute the command
    //         strace_set_options(&state); // Pass temporary tracing state to kernel
    //         exec(argv[1], &argv[1]);
    //         printf(2, "Error: exec failed\n");
    //         exit();
    //     } else if (pid > 0) {
    //         // Parent process: Wait for the child to complete
    //         wait();

    //         // Clear the temporary override after the command
    //         temporary_override = 0;
    //         state.active_option = STRACE_OPTION_NONE;
    //         state.syscall_filter_id = -1;
    //         strace_set_options(&state); // Reset tracing state
    //     } else {
    //         printf(2, "Error: Failed to fork process\n");
    //     }
    //     exit();
    // }

    // Parse options
    parse_options(argc, argv, &state);

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
