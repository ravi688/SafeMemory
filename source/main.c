
#include <safe_memory.h>

int main(int argc, char** argv)
{
	safe_memory_init();

	int* ints = checked_malloc(sizeof(int) * 10);
	float* floats = checked_malloc(sizeof(float) * 2);

	for(u64 i = 0; i <= 4; i++)
	checked_ref(float, floats, i) = 100.0f;

	checked_free(ints);
	checked_free(floats);
	
	safe_memory_terminate();
	return 0;
}