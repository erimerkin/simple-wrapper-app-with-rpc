/**
 * @file    part_c_server_wrapper.c
 * @author  Erim Erkin Doğan
 * 
 * @brief   This code wraps the part_c_server.c's compiled program part_c_server_wrapped.out to redirect command line args to the server for logger connection.
 * 
 *  This code creates a child process and runs part_c_server_wrapped.out binary executable file compiled from part_c_server.c. The aim of this approach is to
 *  handle command line arguments for RPC server which is not stable if we try to take arguments from svc file. The given command line arguments are passed to 
 *  the child process(RPC server in this case) via a pipe redirected to child process' STDIN. Then the passed arguments are used to connect and send data to logger 
 *  server from RPC serverThen parent/main process waits until child process(RPC server) quits.
 * 
 *  This code will be compiled with a name part_c_server.out while the main server code from part_c_server.c will be compiled to part_c_server_wrapped.out
 * 
 *  How to run:
 *  > make
 *  > ./part_c_server.out   logger_ip_address   logger_port
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    int port, child_return_status;
    char *server_address;
    int wrapper2server[2];
    char write_buffer[1024];

    // Checking the argument count
    if (argc != 3)
    {
        fprintf(stderr, "[ERROR] Invalid parameters: Usage %s LOGGER_IP_ADDRESS PORT_NUMBER", argv[0]);
        return -1;
    }

    // Process arguments
    server_address = argv[1];
    port = atoi(argv[2]);

    // Creating pipe
    if (pipe(wrapper2server) == -1)
    {
        perror("[ERROR] Couldn't create pipe.");
        return -1;
    }

    // Forks for child process
    switch (fork())
    {

    case -1:    // Error happened when forking
        perror("[ERROR] Child process couldn't be created.");
        return -1;

    case 0:     // Child process

        // Redirecting pipe to STDIN server, which will be used to redirect arguments
        if (dup2(wrapper2server[0], STDIN_FILENO) == -1)
        {
            perror("[ERROR] Couldn't bind STDIN of child process to pipe.");
            return -1;
        }

        // Closing pipes since STDIN is redirected
        if ((close(wrapper2server[0]) == -1) || (close(wrapper2server[1]) == -1))
        {
            perror("[ERROR] Pipe couldn't be closed.\n");
            return -1;
        }

        // Running the server with pipe redirected to STDIN, so we can deliver command line args to server
        execl("./part_c_server_wrapped.out", "./part_c_server_wrapped.out", NULL);
        break;

    default:    //parent process
        
        close(wrapper2server[0]); // Closing read end of the pipe since we won't read anything

        // Redirecting starting command line arguments to the server via pipe
        sprintf(write_buffer, "%s %d\n", server_address, port);
        write(wrapper2server[1], write_buffer, strlen(write_buffer));

        close(wrapper2server[1]); // Closing the write end of the pipe

        wait(&child_return_status); // Waiting for child process to finish its execution

        // Child process exited with error
        if (child_return_status != 0)
        {
            perror("[ERROR] An error occurred when executing part_c_server");
            return -1;
        }
    }
    return 0;
}