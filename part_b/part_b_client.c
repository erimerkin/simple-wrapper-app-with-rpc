/**
 * @file part_b_client.c
 * @author Erim Erkin Doğan
 *
 * @brief RPC client to send input arguments to the server and print the returned result from server to an output file. 
 *
 *	This program reads the command line arguments and sends executable_path to the server. Then it scans for 2 integer user inputs
 *	which will be sent to the server for calculation by blackbox on executable_path. Then the result is returned with SUCCESS or FAIL
 *	title from the server, returned message is directly printed to output file given in command line arguments.
 *
 *   How to run:
 *   > make
 *   > ./part_b_client.out   blackbox_path       output_path     server_ip_address
 * 
 */
#include "part_b.h"

void part_b_1(char *host, char *runnable_path, char *output_path)
{
	CLIENT *clnt;
	char **result_1;			
	operands runcompiled_1_arg;	// Struct to carry data to server

#ifndef DEBUG
	clnt = clnt_create(host, PART_B, PART_B_VERS, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}
#endif /* DEBUG */
	
	// Scanning input from STDIN (user input)
	int x, y;
	scanf("%d %d", &x, &y);

	// Given inputs are stored in struct to be transferred to the server
	runcompiled_1_arg.a = x;
	runcompiled_1_arg.b = y;
	runcompiled_1_arg.executable_path = runnable_path;

	// Calling the server with given user inputs
	result_1 = runcompiled_1(&runcompiled_1_arg, clnt);

	// Handling server response
	if (result_1 == (char **)NULL)
	{
		clnt_perror(clnt, "call failed");
	}
	else
	{
		// Opening file for output operation, and printing the result to the file
		FILE *output_file;
		output_file = fopen(output_path, "a");	
		fprintf(output_file, "%s", *result_1);
		fclose(output_file);
	}


#ifndef DEBUG
	clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[])
{
	char *host, *executable_path, *output_path;

	// Checking command line arguments
	if (argc != 4)
	{
		printf("[ERROR] Usage: %s executable_path output_path server_ip_address\n", argv[0]);
		exit(1);
	}

	// Processing command line arguments
	executable_path = argv[1];
	output_path = argv[2];
	host = argv[3];

	// Sends request to the server
	part_b_1(host, executable_path, output_path);
	exit(0);
}
