#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>


int main(int argc, char **argv)
{

    int message2child[2], message2parent[2], error2parent[2];
    pid_t pid;
    char write_buffer[10000], read_buffer[10000];
    char *executable_path, *output_path;

    // Checks number of console args, if it is less than or more than 3 wrong input was given
    if (argc != 3)
    {
        printf("Insufficient arguments %d \n", argc);
        return -1;
    }

    // Reading input arguments
    executable_path = argv[1];
    output_path = argv[2];

    //Creating pipes
    if ((pipe(message2child) == -1) || (pipe(message2parent) == -1 || (pipe(error2parent) == -1)))
    {
        perror("[ERROR] Couldn't create pipe.");
        return -1;
    }

    // Forks process
    pid = fork();
    if (pid == -1)
    {
        perror("[ERROR] Child process couldn't be created.");
        return -1;
    }

    // If pid=0 then it is the child process
    if (pid == 0)
    {
        int returnVal;

        // Redirects STDIN, STDERR, STDOUT to pipes
        if (dup2(message2child[0], STDIN_FILENO) == -1 || dup2(message2parent[1], STDOUT_FILENO) == -1 || dup2(error2parent[1], STDERR_FILENO) == -1)
        {
            fprintf(stderr, "[ERROR] Couldn't bind pipes for std file descriptors.\n");
            return -1;
        }

        // Closing pipes since they are duplicated for standard file descriptors, we don't need them open
        close(message2parent[0]);
        close(message2parent[1]);
        close(message2child[0]);
        close(message2child[1]);
        close(error2parent[0]);
        close(error2parent[1]);

        // Executing program from given path
        execl(executable_path, executable_path, NULL);
    }
    else
    // Parent process
    {
        int result_size, status;

        close(message2child[0]); // Parent won't read from parent to child pipe
        close(message2parent[1]); // Parent won't write to message channel from child to parent
        close(error2parent[1]); // Parent won't write to error channel from child to parent

        /* Taking 2 new arguements as input for child process */
        int a, b;
        scanf("%d %d", &a, &b);
        sprintf(write_buffer, "%d %d\n", a, b);
        write(message2child[1], write_buffer, strlen(write_buffer));

        /* Creating file for output operation, and binding the file to standard output */
        FILE *output_file;
        output_file = fopen(output_path, "a");

        // Reading message sent from child process (Result of operation on child process)
        result_size = read(message2parent[0], read_buffer, sizeof(read_buffer));

        // Checks if the program prints an output or error
        if (result_size == 0)
        {
            // The binary programs outputs error message, print error message to file
            read(error2parent[0], read_buffer, sizeof(read_buffer));
            fprintf(output_file, "FAIL:\n%s", read_buffer);
        }
        else
        {
            // Operation was successfull, print result to file
            fprintf(output_file, "SUCCESS:\n%s", read_buffer);
        }
        
        // Close all pipes
        close(message2child[1]);
        close(message2parent[0]);
        close(error2parent[0]);   
    }

    return 0;
}