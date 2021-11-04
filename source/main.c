
#include <stdio.h>
#include <example.h>
#include <buffer_test.h>
#include <safe_memory/safe_memory.h>

int main(int argc, char** argv)
{
	// SAFE_HANDLE(float)  block;
	// SAFE_HANDLE(char) 	string;

	// SAFE_CHECK(float) (block)[0] = 1.02f;
	// SAFE_CHECK(float) (block)[1] = 100.0f;
	// SAFE_CHECK(float) (block)[2] = 200.0f;
	// SAFE_CHECK(float) (block)[3] = 300.0f;		//throws an exception!

	// SAFE_CHECK(char) (string)[100] = 234;	//throws an exception!
	// SAFE_CHECK(char) (string)[101] = 234;	//throws an exception!
	// SAFE_CHECK(char) (string)[34] = 100;		//throws an exception!


	safe_memory_init(); 

	int* ints = safe_malloc(sizeof(int) * 10);
	

	safe_check(ints + 11, 0x1111);

	safe_free(ints);
	safe_memory_terminate();
	function();
	return 0;
}