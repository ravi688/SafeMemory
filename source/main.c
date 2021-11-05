
#include <safe_memory.h>
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
	Float* data = checked_array(Float, 4, 100.0f, 200.0f, 300.0f, 400.0f);
	printf("Data: %f\n", checked_ref(double, data, 0));
	// printf("Data: %f\n", data[1]);
	// printf("Data: %f\n", data[2]);
	// printf("Data: %f\n", data[3]);

	u64* string = checked_array(u64, 5, 0, 1, 2, 3, 4);
	for(int i = 0; i < 5; i++)
		printf("Int: %u\n", string[i]);

	Double* packets = checked_struct_array(Double, 3, (Double) { 10.0, 20.0, 1 }, (Double) { 30.0, 40.0, 2 }, (Double) { 50.0, 60.0, 3 });
	for(u64 i = 0; i < 3; i++)
	{
		printf("Double[%u] -> %f\n",i, packets[i].value);
		printf("Double[%u] -> %f\n",i, packets[i].value2);
		printf("Double[%u] -> %u\n",i, packets[i].i);
	}
	safe_memory_terminate();
	return 0;
}