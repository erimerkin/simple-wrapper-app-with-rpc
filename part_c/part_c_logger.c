/**
 * @file    part_c_logger.c
 * @author  Erim Erkin Doğan
 *
 * @brief   This code creates a socket and listens for part_c_server to send data. Then the data is outputted to given output file. Only supports 1 concurrent connection.
 * 
 *  With use of TCP sockets, this program listens sets up a socket in given port number and listens for connections. Only one connection at a time is possible. 
 *  Received data from part_c_server is then outputted to a file in given command line arguments. Supports queue of 5 requests, will run until an error or force termination.
 * 
 *  Referenced from: https://www.binarytides.com/socket-programming-c-linux-tutorial/ 
 * 
 *  How to run:
 *  > make
 *  > ./part_c_logger.out   output_path.log     port
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
    char *log_path;
    int socket_address, client_address, port, address_length;
    struct sockaddr_in server, client;
    char client_message[5];

    // Checking argument count
    if (argc != 3)
    {
        fprintf(stderr, "[ERROR] Correct usage: %s log_file_path PORT\n", argv[0]);
        return -1;
    }

    // Processing the arguments
    log_path = argv[1];
    port = atoi(argv[2]);

    //Create socket
    socket_address = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_address == -1)
    {
        fprintf(stderr, "[ERROR] Socket couldn't be created.");
    }

    // Preparing the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Binding socket
    if (bind(socket_address, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("[ERROR] Socket couldn't bind to given port");
        return -1;
    }
    // Listening for clients
    listen(socket_address, 5);

    FILE *output_file;
    output_file = fopen(log_path, "a");

    // Accept connections
    address_length = sizeof(struct sockaddr_in);
    while ((client_address = accept(socket_address, (struct sockaddr *)&client, (socklen_t *)&address_length)))
    {
        // Checking if connection is valid
        if (client_address == -1)
        {
            perror("[ERROR] Client connection couldn't be accepted. Trying another connection.\n");
        }

        //Receive messages from client
        ssize_t read_size;
        while ((read_size = read(client_address, client_message, sizeof(client_message))) > 0)
        {
            client_message[read_size] = '\0';               //adding eos char
            fprintf(output_file, "%s", client_message);     // printing the message
            fflush(output_file);                            //flushing file so it doesn't buffer
        }

        if (read_size == -1)
        {
            perror("[ERROR] The message couldn't be received");
            return -1;
        }
    }

    fclose(output_file); // Close output file

    return 0;
}