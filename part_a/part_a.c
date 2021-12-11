#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

/**
 * O_WRONLY: Open for writing only,
 * O_CREAT: If the file exists, this flag has no effect. Otherwise, the file is created,
 * O_APPEND: If set, the file offset will be set to the end of the file prior to each write.
*/
#define CREATE_FLAGS (O_WRONLY | O_CREAT | O_APPEND)

#define CREATE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char **argv)
{

    int parent2child[2], child2parent[2], error2parent[2];
    pid_t pid;
    char write_buffer[10000], read_buffer[10000];
    char *executable_path, *output_path;

    if (argc != 3)
    {
        printf("Insufficient arguments %d \n", argc);
        return -1;
    }

    // Reading input arguments
    executable_path = argv[1];
    output_path = argv[2];

    //Creating pipes
    if ((pipe(parent2child) == -1) || (pipe(child2parent) == -1 || (pipe(error2parent) == -1)))
    {
        perror("There was an error creating pipes.");
        return -1;
    }

    // Forks process
    pid = fork();
    if (pid == -1)
    {
        perror("Failed fork process");
        return -1;
    }

    // If pid=0 then it is the child process
    if (pid == 0)
    {
        int returnVal;

        if (dup2(parent2child[0], STDIN_FILENO) == -1 || dup2(child2parent[1], STDOUT_FILENO) == -1 || dup2(error2parent[1], STDERR_FILENO) == -1)
        {
            fprintf(stderr, "There was an error binding pipes for std file descriptors.\n");
            return -1;
        }

        // Closing pipes since they are duplicated for standard file descriptors, we don't need them open
        close(child2parent[0]);
        close(child2parent[1]);
        close(parent2child[0]);
        close(parent2child[1]);
        close(error2parent[0]);
        close(error2parent[1]);

        execl(executable_path, executable_path, NULL);
    }
    else
    // Parent process
    {
        int result_size, status;

        close(parent2child[0]); // Parent won't read from parent to child pipe
        close(child2parent[1]); // Parent won't write to child to parent pipe
        close(error2parent[1]);

        /* Taking 2 new arguements as input for child process */
        int a, b;
        scanf("%d %d", &a, &b);
        sprintf(write_buffer, "%d %d\n", a, b);
        write(parent2child[1], write_buffer, strlen(write_buffer));

        /* Creating file for output operation, and binding the file to standard output */
        FILE *output_file;
        output_file = fopen(output_path, "a");
        // for (int i = 0; i < strlen(read_buffer); i++){
        //     if (isdigit(read_buffer[i]) == 0 && isspace(read_buffer[i]) == 0){
        //         printf("FAIL:\n%s", read_buffer);
        //         return 0;
        //     }
        // }

        // Reading message sent from child process (Result of operation on child process)
        result_size = read(child2parent[0], read_buffer, sizeof(read_buffer));

        if (result_size == 0)
        {
            read(error2parent[0], read_buffer, sizeof(read_buffer));
            fprintf(output_file, "FAIL:\n%s", read_buffer);
        }
        else
        {
            fprintf(output_file, "SUCCESS:\n%s", read_buffer);
        }
        
        close(parent2child[1]);
        close(child2parent[0]);
        close(error2parent[0]);   
    }

    return 0;
}