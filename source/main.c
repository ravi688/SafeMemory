
#include <safe_memory/safe_memory.h>
#include <stdarg.h>
#include <memory.h>
#include <stdio.h>

typedef struct 
{
	uint32_t v1;
	uint64_t v2;
} packet_t;

typedef struct 
{
	double value;
} Float;

typedef struct
{
	double value;
	double value2;
	u8 i;
} Double;

int main(int argc, char** argv)
{
	safe_memory_init();
	u64* values = checked_array(u64, 2, 500, 600);
	for(u64 i = 0; i < 2; i++)
		printf("Value: %u\n", values[i]);

	register_stack_allocation(NULL, 0);

	float* fvalues = checked_array(float, 2, 4.0f, 5.5f);
	for(u64 i = 0; i < 2; i++)
		printf("Value: %f\n", fvalues[i]);
	safe_memory_terminate();
	return 0;
}