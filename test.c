#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
    //strace(1);  // Enable tracing

    int i;
    for (i = 0; i < 3; i++) {
        int pid = fork();
        if (pid == 0) {
            //printf(1, "Child %d: Running\n", i);
            char *args[] = {"echo", "hello", 0};
            exec("echo", args);
            exit();
        }
    }

    for (i = 0; i < 3; i++) {
        wait();
    }

    //strace_dump();  // Dump trace logs
    //strace(0);      // Disable tracing
    exit();
}

//shows a failed test call
// int main(void) {
//     open("nonexistent_file.txt", 0);
//     exit();

    
// }
