#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define FILENAME "shared_var.txt"

// Helper function to convert an integer to a string
void itoa(int value, char *str) {
    int i = 0;
    int is_negative = 0;

    // Handle negative numbers
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }

    // Convert the number to a string in reverse order
    do {
        str[i++] = (value % 10) + '0';
        value /= 10;
    } while (value > 0);
    

    // Add the negative sign if necessary
    if (is_negative) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string to get the correct order
    int j;
    for (j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}



void write_to_file(int value) {
    // Open the file with O_CREATE | O_RDWR to overwrite it
    int fd = open(FILENAME, O_CREATE | O_RDWR);
    if (fd < 0) {
        printf(1, "Error: Failed to open the file for writing\n");
        exit();
    }

    // Truncate the file manually
    // Overwrite the file by writing an empty string
    // if (write(fd, "", 0) < 0) {
    //     printf(1, "Error: Failed to truncate the file\n");
    //     close(fd);
    //     exit();
    // }
    // Convert the integer value to a string
    char buffer[16];
    itoa(value, buffer);
    int length = strlen(buffer);
    // Reset to zero
    write(fd, "0", 1);
    // Write the string to the file
    if (write(fd, buffer, length) != length) {
        printf(1, "Error: Failed to write to the file\n");
        close(fd);
        exit();
    }

    close(fd);
}

int read_from_file() {
    // Open the file for reading, starting at the beginning
    // int fd = open(FILENAME, O_RDONLY);
     int fd = open(FILENAME, O_CREATE | O_RDWR);
    if (fd < 0) {
        printf(1, "Error: Failed to open the file for reading\n");
        exit();
    }

    char buffer[16];
    int bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead < 0) {
        printf(1, "Error: Failed to read from the file\n");
        exit();
    }
    buffer[bytesRead] = '\0';

    close(fd);
    return atoi(buffer);
}

void increment_shared_variable() {
    // Open and Read it
    int final_value = read_from_file();

    // Increment it
    final_value += 1;

    //sleep(1);

    // Reset file "ponter" to the beginning
    //write_to_file(0);
    // Write it
    write_to_file(final_value);
}


int main() {
    // Create and initialize the shared file
    int fd = open(FILENAME, O_CREATE | O_RDWR);
    if (fd < 0) {
        printf(1, "Error: Failed to create the file\n");
        exit();
    }
    close(fd);
    write_to_file(0); // Initialize the shared variable to 0

    int pid = fork();

    if (pid < 0) {
        printf(1, "Error: Fork failed\n");
        exit();
    }

    if (pid == 0) {
        // Child process
        int i;
        for (i = 0; i < 2; i++) {
            increment_shared_variable();
        }
        

        exit();
    } else {
        // Parent process

        int i;
        for (i = 0; i < 2; i++) {
            increment_shared_variable();
        }
    
         wait(); // Wait for the child process to finish

        // Read the final value from the file
       int final_value = read_from_file();
       printf(1, "Final value of the shared variable: %d\n", final_value);

        // Reset the file to zero
        write_to_file(0);

        // final_value = read_from_file();
        // printf(1, "Check after reset: %d\n", final_value);
    }

   


    exit();
}