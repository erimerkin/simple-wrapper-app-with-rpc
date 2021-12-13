#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int sock, port;
    pid_t pid;
    struct sockaddr_in server;
    char *server_address;
    int wrapper2server[2];

    if (argc != 3)
    {
        fprintf(stderr, "[ERROR] Invalid parameters: Usage %s LOGGER_IP_ADDRESS PORT_NUMBER", argv[0]);
        return -1;
    }

    server_address = argv[1];
    port = atoi(argv[2]);

    if (pipe(wrapper2server) == -1)
    {
        perror("[ERROR] Couldn't create pipe.");
        return -1;
    }

    pid = fork();

    if (pid == -1)
    {
        perror("[ERROR] Child process couldn't be created.");
        return -1;
    }

    if (pid > 0) // Child process
    {
        char write_buffer[10000];
        int status;


        sprintf(write_buffer, "%s %d\n", server_address, port);
        printf("sent args: %s", write_buffer);
        write(wrapper2server[1], write_buffer, strlen(write_buffer));

        waitpid(-1, &status, 0);
    }
    else // Parent Process
    {
        printf("inside child process\n");
        if (dup2(wrapper2server[0], STDIN_FILENO) == -1)
        {
            perror("[ERROR] Couldn't bind STDIN of child process to pipe.");
            return -1;
        }

        close(wrapper2server[0]);
        close(wrapper2server[1]);

        execl("./part_c_server_wrapped.out", "./part_c_server_wrapped.out", NULL);
    }

    return 0;
}