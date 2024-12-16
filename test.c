#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
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
    exit();
}

