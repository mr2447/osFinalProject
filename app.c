#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define FILENAME "shared_var.txt"

void write_to_file(int value) {
    int fd = open(FILENAME, O_CREAT | O_RDWR | O_TRUNC, 0644); // Open file with proper flags
    if (fd < 0) {
        printf("Error: Failed to open the file for writing\n");
        exit(1);
    }

    char buffer[16];
    sprintf(buffer, "%d", value); // Convert integer to string
    int length = strlen(buffer);

    if (write(fd, buffer, length) != length) {
        printf("Error: Failed to write to the file\n");
        close(fd);
        exit(1);
    }

    close(fd);
}

int read_from_file() {
    int fd = open(FILENAME, O_RDONLY);
    if (fd < 0) {
        printf("Error: Failed to open the file for reading\n");
        exit(1);
    }

    char buffer[16];
    int bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead < 0) {
        printf("Error: Failed to read from the file\n");
        close(fd);
        exit(1);
    }
    buffer[bytesRead] = '\0';

    close(fd);
    return atoi(buffer); // Convert string to integer
}

void increment_shared_variable() {
    int final_value = read_from_file(); // Read current value
    final_value += 1;                  // Increment value
    write_to_file(final_value);        // Write back to file
}

int main() {
    // Create and initialize the shared file
    int fd = open(FILENAME, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) {
        printf("Error: Failed to create the file\n");
        exit(1);
    }
    close(fd);

    write_to_file(0); // Initialize the shared variable to 0

    int pid = fork();

    if (pid < 0) {
        printf("Error: Fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Child process
       
        for ( int i = 0; i < 2; i++) {
            increment_shared_variable();
        }
        
        exit(0);
    } else {
        // Parent process
         // Wait for the child process to finish
        
        for(int i = 0; i < 2; i++) {
            increment_shared_variable();
        }

        wait(NULL);

        // Read the final value from the file
        int final_value = read_from_file();
        printf("Final value of the shared variable: %d\n", final_value);

        // Reset the file to zero
        write_to_file(0);
        final_value = read_from_file();
        // printf("Check after reset: %d\n", final_value);

        
    }

    return 0;
}