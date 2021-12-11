#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h> 



int main(int argc, char **argv)
{
    char *log_path;
    int socket_address , client_address, port, read_size, address_length;
    struct sockaddr_in server, client;
    char client_message[2000];
    
    if (argc != 3) {
        fprintf(stderr, "[Error] Correct usage: %s log_file_path PORT\n", argv[0]);
        return -1;
    }

    log_path = argv[1];
    port = atoi(argv[2]);

    //Create socket
    socket_address = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_address == -1)
    {
        fprintf(stderr, "[ERROR] Socket couldn't be created.");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // Binding socket
    if( bind(socket_address,(struct sockaddr *)&server , sizeof(server)) == -1)
    {
        //print the error message
        perror("[ERROR] Socket couldn't bind.");
        return -1;
    }
    //Listening for clients
    listen(socket_address , 10);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    address_length = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_address = accept(socket_address, (struct sockaddr *)&client, (socklen_t*)&address_length);
    if (client_address == -1)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    FILE *output_file;
    output_file = fopen(log_path, "a");

    //Receive a message from client
    while( (read_size = read(client_address, client_message , sizeof(client_message))) > 0 )
    {

        fprintf(output_file, "%s", client_message);

    }

    fclose(output_file);

    if(read_size == 0)
    {
        printf("Client disconnected.\n");
    }
    else if(read_size == -1)
    {
        perror("[ERROR] The message couldn't be received");
        return -1;
    }


    return 0;

}