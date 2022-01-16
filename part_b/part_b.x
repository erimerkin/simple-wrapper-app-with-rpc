/* Specify the arguments */
struct operands{
	string executable_path<>;
	int a;
	int b;
};

/* 
 * 1. Name the program and give it a unique number.
 * 2. Specify the version of the program.
 * 3. Specify the signature of the program.
*/
program PART_B{
	version PART_B_VERS{
		string runCompiled(operands)=1;
	}=1;
}=0x12345678;