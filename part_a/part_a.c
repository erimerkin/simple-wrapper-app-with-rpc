/**
* @file part_a.c
* @author Erim Erkin Doğan
*
* @brief Code to run a binary program from given path using a child process that returns an error or integer result, and write the result to specified output file.
*
*   The program takes 2 input arguments: Binary executable file location and output file location. The given binary executable file is executed in a child process, while
*   the outputs and inputs are redirected to the main process with pipes. Then the result is printed with a FAIL or SUCCESS message to the given output file.
*   Redirecting works by creating 3 one directional pipes: first pipe connects parent to child's STDIN, second one connects child's STDOUT and STDERR to parent.
*   Difference between error and successful run is made by using wait(status), as if there was an error in the program, return value wouldn't be 0. Then according to this
*   return value from blackbox, SUCCESS or FAIL messages are printed to the file.
*
*   Coded with the help of PS6 materials io_capture.c and simpleredirect.c
*
*   How to run:
*   > make
*   > ./part_a.out binary_executable_path output_file_path
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{

    int message2child[2], message2parent[2];
    char write_buffer[256], read_buffer[256];
    char *executable_path, *output_path;

    // Checks number of console args, if it is less than or more than 3, the program was ran wrongly
    if (argc != 3)
    {
        printf("[ERROR] Usage: %s binary_file_path output_file_path", argv[0]);
        return -1;
    }

    // Reading input arguments
    executable_path = argv[1];
    output_path = argv[2];

    //Creating pipes
    if ((pipe(message2child) == -1) || (pipe(message2parent) == -1))
    {
        perror("[ERROR] Couldn't create pipe.");
        return -1;
    }

    switch (fork())
    {
    case -1: // If the return value of fork() is -1, fork is failed

        perror("[ERROR] Child process couldn't be created.");
        return -1;

    case 0: // If the return value of fork() is 0, then it is the child process

        // Redirects STDIN, STDERR, STDOUT to pipes
        if (dup2(message2child[0], STDIN_FILENO) == -1 || dup2(message2parent[1], STDOUT_FILENO) == -1 || dup2(message2parent[1], STDERR_FILENO) == -1)
        {
            perror("[ERROR] Couldn't bind pipes for std file descriptors.");
            return -1;
        }

        // Closing pipes since they are duplicated for standard file descriptors, we don't need them open
        close(message2parent[0]);
        close(message2parent[1]);
        close(message2child[0]);
        close(message2child[1]);

        // Executing program from given path
        execl(executable_path, executable_path, NULL);
        break;

    default: // Otherwise, current process is the main process

        close(message2child[0]);  // Parent won't read from parent to child pipe
        close(message2parent[1]); // Parent won't write to message channel from child to parent

        /* Taking 2 new arguments as input for child process, and writing to the pipe connected to stdin of child process */
        int a, b;
        scanf("%d %d", &a, &b);
        sprintf(write_buffer, "%d %d\n", a, b);
        write(message2child[1], write_buffer, strlen(write_buffer));

        // Waiting for child process to finish, then saving the return status
        int status;
        wait(&status);

        // Initializing char pointer for reading from pipe
        char *full_message;
        full_message = (char *)malloc(sizeof(char));
        strcpy(full_message, "");

        // Buffer size is set as 256, so parent process will read till there is nothing to read.
        // Outputs bigger than 255 size need to be read more than once.
        ssize_t read_size;
        while ((read_size = read(message2parent[0], read_buffer, sizeof(read_buffer) - 1)) > 0)
        {
            // Creates a new local char array to hold temporary string with bigger size than full_message array
            char temp_string[read_size + strlen(full_message) + 1];

            // Copies full message to the newly created temp string
            strcpy(temp_string, full_message);
            read_buffer[read_size] = '\0';     // Adding EOS null char to end the string
            strcat(temp_string, read_buffer); //Concanterates newly read message to message that is read earlier

            // Reallocating memory and checking result
            full_message = realloc(full_message, sizeof(temp_string));
            if (full_message == NULL)
            {
                perror("[ERROR] Memory allocation error.\n");
                return -1;
            }

            // Copies the read message to full message again
            strcpy(full_message, temp_string);
        }

        /* Creating file for output operation, and binding the file to standard output */
        FILE *output_file;
        output_file = fopen(output_path, "a");

        // Checking the error status of blackbox, and printing respective output
        if (status == 0)
        {
            fprintf(output_file, "SUCCESS:\n%d\n", atoi(full_message));
        }
        else
        {

            // Checking if the returned error message ends with \n, then removing it since we add \n in fprintf()
            if (full_message[strlen(full_message)-1] == '\n'){
                full_message[strlen(full_message)-1] = '\0';
            }

            fprintf(output_file, "FAIL:\n%s\n", full_message);
        }

        free(full_message); // Free area allocated by malloc and realloc

        // Close file
        fclose(output_file);

        // Close remaining pipes
        close(message2child[1]);
        close(message2parent[0]);
    }

    return 0;
}