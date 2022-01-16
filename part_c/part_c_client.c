/**
 * @file 	part_c_client.c
 * @author 	Erim Erkin Doğan
 *
 * @brief 	RPC client to send input arguments to the server and print the returned result from server to an output file. 
 *
 *	This program reads the command line arguments and sends executable_path to the server. Then it scans for 2 integer user inputs
 *	which will be sent to the server for calculation by blackbox on executable_path. Then the result is returned with SUCCESS or FAIL
 *	title from the server, returned message is directly printed to output file given in command line arguments.
 *
 *   How to run:
 *   > make
 *   > ./part_c_client.out   blackbox_path       output_path     server_ip_address
 * 
 */

#include "part_c.h"

void part_c_1(char *host, char *runnable_path, char *output_path)
{
	CLIENT *clnt;
	char **result_1;
	arguments run_binary_1_arg;

#ifndef DEBUG
	clnt = clnt_create(host, PART_C, PART_C_VERS, "udp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}
#endif /* DEBUG */

	// Scanning input from STDIN (user input)
	int x, y;
	scanf("%d %d", &x, &y);

	// Read inputs are stored in struct
	run_binary_1_arg.a = x;
	run_binary_1_arg.b = y;
	run_binary_1_arg.executable_path = runnable_path;

	// handling response from server, checking if the return is a null pointer
	result_1 = run_binary_1(&run_binary_1_arg, clnt);
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
	part_c_1(host, executable_path, output_path);
	exit(0);
}
