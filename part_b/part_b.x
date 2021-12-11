/*
 * How to save, compile, and run:
 * sudo apt install rpcinfo // to use rpcinfo, run once
 * Save file with .x extension.
 *
 * rpcgen -C add.x // -C for creating all the files, generate code in ANSI C.
 * rpcgen -a add.x // -a generate all the files including sample code for client and server side.
 * make -f Makefile.add // -f to use given file as a makefile.
 */

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
		/* Takes a numbers structure and gives the integer result. */
		string runCompiled(operands)=1;
	}=1;
}=0x12345678;