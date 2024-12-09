#include "types.h"
#include "stat.h"
#include "user.h"
#include "strace.h"
#include "fcntl.h"

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
    {"strace", 22}, 
    {"strace_dump", 23}, 
    {"strace_option", 24}, 
    {"strace_set_output", 25}, 
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
    printf(1,"  strace -e <syscall_name>\n");
    printf(1,"  strace -s\n");
    printf(1,"  strace -f\n");
    printf(1,"  strace -o <filename>\n");
    exit();
}

int main(int argc, char *argv[]) {
    //struct strace_option option; // Store parsed options
    // static int temporary_override = 0; // Tracks temporary overrides

   

    if (argc < 2) {
        usage();
        exit();
    }


    if (strcmp(argv[1], "-s") == 0) {
        if((strcmp(argv[2], "-e") == 0) && argc == 4) { //-s and -e
            int syscall_filter_id = syscall_number_from_name(argv[3]);
            if(syscall_filter_id == -1) {
                printf(1, "Invalid syscall name: %s\n", argv[3]);
                exit();
            }
            strace_option(syscall_filter_id, STRACE_OPTION_SUCCESS, STRACE_OPTION_NONE, 1);
            exit();
        }
        else if(argc == 2){ //only -s
            strace_option(-1, STRACE_OPTION_SUCCESS, STRACE_OPTION_NONE, 1);
            exit();
        }
        else{ //wrong format
            usage();
            exit();
            
        }
    }
    else if (strcmp(argv[1], "-f") == 0) {
        if(strcmp(argv[2], "-e") == 0 && argc == 4) { //-f and -e
            int syscall_filter_id = syscall_number_from_name(argv[3]);
            if(syscall_filter_id == -1) {
                printf(1, "Invalid syscall name: %s\n", argv[3]);
                exit();
            }
            strace_option(syscall_filter_id, STRACE_OPTION_NONE, STRACE_OPTION_FAIL, 1);
            exit();
        }
        else if(argc == 2){ //only -f
            strace_option(-1, STRACE_OPTION_NONE, STRACE_OPTION_FAIL, 1);
            exit();
        }
        else{ //wrong format
            usage();
            exit();
        }
    }
    else if (strcmp(argv[1], "-e") == 0 && argc == 3) {
        int syscall_filter_id = syscall_number_from_name(argv[2]);
        if(syscall_filter_id == -1) {
            printf(1, "Invalid syscall name: %s\n", argv[2]);
            exit();
        }
        strace_option(syscall_filter_id, STRACE_OPTION_NONE, STRACE_OPTION_NONE, 1);
        exit();
    }
   
            
        

    // // Parse options
    // int i;
    // for (i = 1; i < argc; i++) {
    //     if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
    //        int active_option = STRACE_OPTION_FILTER;
    //         int syscall_filter_id = syscall_number_from_name(argv[++i]);
    //         if(syscall_filter_id == -1) {
    //             printf(1, "Invalid syscall name: %s\n", argv[i]);
    //             exit();
    //         }
    //         strace_option(active_option, syscall_filter_id, 1);
    //         exit();
    //     } else if (strcmp(argv[i], "-s") == 0) {
    //         int active_option = STRACE_OPTION_SUCCESS;

    //         strace_option(active_option, -1, 1);
    //         exit();
    //     } else if (strcmp(argv[i], "-f") == 0) {
    //         int active_option = STRACE_OPTION_FAIL;
    //         strace_option(active_option, -1, 1);
    //         exit();
    //     } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc){
    //         const char *filename = argv[++i];
    //         // Open the file for writing
    //         printf(1, "Opening file: %s\n", filename);
    //         int fd = open((char *)filename, O_CREATE | O_WRONLY);
    //         printf(1, "File descriptor: %d\n", fd);
    //         if(fd < 0) {
    //             printf(1, "Failed to open file: %s\n", filename);
    //             exit();
    //         }
    //         // Set the output file descriptor
    //         if(strace_set_output(fd) < 0) {
    //             printf(1, "Failed to set output file\n");
    //             exit();
    //         }
    //         strace_option(STRACE_OPTION_OUTPUT, -1, 1);
    //         exit();
    //     }

    //}

    // Handle "on" or "off" tracing
    if (strcmp(argv[1], "on") == 0 && argc == 2) {
        strace(1);  // Enable strace
        exit();
        
    }
    else if (strcmp(argv[1], "off") == 0 && argc == 2) {
        strace(0);  // Disable strace
        exit();
    }
    // Handle "run" to enable tracing and execute a command
    else if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            usage();
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
    else if (strcmp(argv[1], "dump") == 0 && argc == 2) {
        strace_dump();  // Call the kernel to print the syscall log
        exit();
    }
    else {
        usage();
    }

    exit();
}
