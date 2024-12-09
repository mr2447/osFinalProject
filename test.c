#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
    //strace(1);  // Enable tracing

    printf(1, "Hello, world!\n");

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

    open("nonexistent_file.txt", 0); //shows a failed call

    // strace_dump();  // Dump trace logs
    //strace(0);      // Disable tracing
    exit();
}



// int main(void) {
//     //strace(1);  // Enable tracing

//     int i;
//     for (i = 0; i < 3; i++) {
//         int pid = fork();
//         if (pid == 0) {
//             int j;
//             for (j = 0; j < 3; j++) {
//                 int pid = fork();
//                 if (pid == 0) {
//                     //I want it to allocate memory and then exit
//                     char *args[] = {"sbrk", "1000", 0};
//                     exec("sbrk", args);
//                     exit();
           
//                 }
//             }
//             for (j = 0; j < 3; j++) {
//                 wait();
//             }
//         }
//     }

//     for (i = 0; i < 3; i++) {
//         wait();
//     }

//     open("nonexistent_file.txt", 0);

//     //strace_dump();  // Dump trace logs
//     //strace(0);      // Disable tracing
//     exit();
// }
