/* Specify the arguments */
struct arguments{
	string executable_path<>;
	int a;
	int b;
};

/* 
 * 1. Name the program and give it a unique number.
 * 2. Specify the version of the program.
 * 3. Specify the signature of the program.
*/
program PART_C{
	version PART_C_VERS{
		/* Takes a numbers structure and gives the integer result. */
		string run_binary(arguments)=1;
	}=1;
}=0x12345678;