/**
 * @file    part_c_server.c
 * @author  Erim Erkin Doğan
 *
 * @brief   RPC server to run and return the result of a binary program from given path by the RPC client. 
 *
 *   The server takes 3 arguments from the client. This arguments are blackbox's path to run the program in a child process and 2 integers to feed the data
 *   to the blackbox. These 2 integers are delivered to the child process with help of pipes, and the result of the running program in child 
 *   process is also redirected to parent process with again use of pipes. Then the read result is returned to the client with a FAIL or SUCCESS message
 *   which will be written to an output file. This program also connects to a logger via TCP socket connection from given ip address and ports. Connection to
 *   socket is set in first run as static variables, and will continue until the program is exited. Passed log to the logger changes depending on the blackbox's output.
 *
 *   Redirecting the inputs and outputs to blackbox works by creating 2 one directional pipes: first pipe connects parent to child's STDIN, second one 
 *   connects child's STDOUT and STDERR to the parent process. 
 *   Blackbox's fail or success is checked by use of wait(status), in which if status 0 blackbox runs successfully otherwise it should be an error.
 *
 *   To pass command line arguments to the server(this program), another wrapper program(part_c_server_wrapper.c) will be executed with wanted 
 *   command line arguments. Then this wrapper program will run this server as a child process and redirect input via pipes. To accomodate that wrapper program runs with
 *   ./part_c_server.out command, this file is compiled as part_c_server_wrapped.out.
 *
 *   This code is referenced from PS6's rpc tutorials and io_capture.c and simpleredirect.c
 *   Socket client codes are referenced from https://www.binarytides.com/socket-programming-c-linux-tutorial/
 *
 *   How to run:
 *   > make
 *   > ./part_c_server.out   logger_ip_address   logger_port_number
 * 
 */

#include "part_c.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>

static int server_socket, run;
static struct sockaddr_in server_address;

char **
run_binary_1_svc(arguments *argp, struct svc_req *rqstp)
{

    static char *result;

    int message2child[2], message2parent[2];
    char write_buffer[256], read_buffer[256];

    //Creating pipes
    if ((pipe(message2child) == -1) || (pipe(message2parent) == -1))
    {
        perror("[ERROR] Couldn't create pipe.");
        exit(-1);
    }

    // Cleaning results from previous call to the function
    xdr_free((xdrproc_t)xdr_wrapstring, (char *)&result);

    // Creates a child process
    switch (fork())
    {
    // If return value of fork() is -1, then there was an error creating child process
    case -1:
        perror("[ERROR] Failed fork process.");
        exit(-1);

    // If return of fork() is 0, then this is the child process
    case 0:

        // Redirecting STDIN, STDOUT and STDERR to pipes
        if (dup2(message2child[0], STDIN_FILENO) == -1 || dup2(message2parent[1], STDOUT_FILENO) == -1 || dup2(message2parent[1], STDERR_FILENO) == -1)
        {
            perror("[ERROR] There was an error binding pipes for standard file descriptors.");
            exit(-1);
        }

        // Closing pipes since they are duplicated for standard file descriptors, we don't need them open
        close(message2parent[0]);
        close(message2parent[1]);
        close(message2child[0]);
        close(message2child[1]);

        execl(argp->executable_path, argp->executable_path, NULL);
        break;

    // If the return value is not 0 or -1, then it should be the parent process
    default:

        close(message2child[0]);  // Parent won't read from parent to child pipe
        close(message2parent[1]); // Parent won't write to message channel from child to parent

        /////////////////////////////////////////////////////////
        //  SOCKET SETUP for first run
        //
        //  Socket codes are learned and referenced from https://www.binarytides.com/socket-programming-c-linux-tutorial/
        ////////////////////////////////////////////////////////
        // If this is the first run, initializes and connect to the socket
        // Socket details are being hold in static variables, which will be stored until the program exits
        if (run == 0)
        {
            char server_ip[256];
            int port;

            //Taking arguments from the wrapper connected via pipe
            scanf("%s %d", server_ip, &port);

            // If ip argument is given as localhost, change it to 127.0.0.1 for successful ip translation from string
            if (strcmp(server_ip, "localhost") == 0)
            {
                strcpy(server_ip, "127.0.0.1");
            }

            // Setting up ipv4 address for connection
            if (inet_pton(AF_INET, server_ip, &(server_address.sin_addr)) <= 0)
            {
                perror("[ERROR] Given ip address couldn't be converted from string.");
                exit(-1);
            }
            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(port);

            run = 1; //Setting run=1 to block the initialization in other calls
        }

        // initializing socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        //Creates socket
        if (server_socket == -1)
        {
            printf("[ERROR] Could not create socket");
            exit(-1);
        }

        // Connecting to the socket
        if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
        {
            perror("[ERROR] Socket connection failed.");
            exit(-1);
        }

        /* Taking 2 new arguments as input for child process */
        sprintf(write_buffer, "%d %d\n", argp->a, argp->b);
        // Redirecting the input to child process as standard input
        write(message2child[1], write_buffer, strlen(write_buffer));

        // Waiting for child process to finish, then saving the return status
        int status;
        wait(&status);

        // Initializing char pointer for reading from pipe
        char *full_message;
        full_message = (char *)malloc(sizeof(char));
        strcpy(full_message, "");

        // Buffer size is set as 256, so parent process will read till there is nothing to read.
        // Since outputs bigger than 255 size needs to be read more than once.
        ssize_t read_size;
        while ((read_size = read(message2parent[0], read_buffer, sizeof(read_buffer) - 1)) > 0)
        {
            // Creates a new local char array to hold temporary string with bigger size than full_message array
            char read_message[read_size + strlen(full_message) + 1];

            // Copies full message to the newly created temp string
            strcpy(read_message, full_message);
            read_buffer[read_size] = '\0';     // Adding EOS null char to end the string
            strcat(read_message, read_buffer); //Concanterates newly read message to message that is read earlier

            // Reallocating memory and checking result
            full_message = realloc(full_message, sizeof(read_message));
            if (full_message == NULL)
            {
                perror("[ERROR] Memory allocation error.\n");
                exit(-1);
            }

            // Copies the read message to full message again
            strcpy(full_message, read_message);
        }

        char log_message[256];

        // Checking the error status of blackbox, and printing respective output
        if (status == 0)
        {
            // Creating a temp string with enough space for full message and SUCCESS title
            char temp_string[strlen(full_message) + 10];

            // SUCCESS and full message are formatted and concentrated, log message is also created
            int returned_result = atoi(full_message);
            sprintf(temp_string, "SUCCESS:\n%d\n", returned_result);
            sprintf(log_message, "%d %d %d\n", argp->a, argp->b, returned_result);

            // Reserves heap memory space for the result to be copied
            result = (char *)malloc(sizeof(temp_string));
            strcpy(result, temp_string);
        }
        else
        {
            // Checking if the returned error message ends with \n, then removing it since we add \n in fprintf()
            if (full_message[strlen(full_message) - 1] == '\n'){
                full_message[strlen(full_message) - 1] = '\0';
            }

            // Creating a temp string with enough space for full message and FAIL title. log message is also created
            char temp_string[strlen(full_message) + 7];
            sprintf(temp_string, "FAIL:\n%s\n", full_message);
            sprintf(log_message, "%d %d _\n", argp->a, argp->b);

            // Reserves heap memory space for the result to be copied
            result = (char *)malloc(sizeof(temp_string));
            strcpy(result, temp_string);
        }

        free(full_message); // Free area allocated by malloc and realloc

        if (send(server_socket, log_message, strlen(log_message), 0) == -1)
        {
            perror("[ERROR] Couldn't send message to the logger server_address.");
            exit(-1);
        }

        // Closing remaining pipes
        close(message2child[1]);
        close(message2parent[0]);

        // Shutting down socket, by doing this before closeing we ensure all data has been sent
        if (shutdown(server_socket, 2) == -1)
        {
            perror("[ERROR] Socket couldn't disconnect");
            exit(-1);
        }
        // Closing socket connection
        close(server_socket);
    }

    return &result;
}
