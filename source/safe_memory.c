

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <buffer.h>
#include <debug.h>

#include <safe_memory/safe_memory.h>

static pBUFFER allocationList = BUF_INVALID;

#ifdef ASSERT
#	undef ASSERT
#endif

#if defined(GLOBAL_DEBUG) && defined(LOG_DEBUG)
#	define ASSERT(boolean, ...)\
	do\
	{\
		if(!(boolean))\
		{\
			printf("Assertion Failed: ");\
			printf(__VA_ARGS__);\
			printf(", at %u, %s, %s\n", __line__, __function__, __file__);\
			exit(0);\
		}\
	} while(0)
#else
#	define ASSERT(boolean, ...)
#endif

typedef struct 
{
	void* basePtr; 
	u64 size;
} allocationData_t;

static bool comparer(void* basePtr, void* data);

function_signature(void*, register_allocation, void* basePtr, u64 size)
{
	ASSERT(basePtr != NULL, "Allocation failed for %u bytes, basePtr == NULL", size);
	BUFpush_binded();
	BUFbind(allocationList);
	ASSERT(BUFfind_index_of(basePtr, comparer) == BUF_INVALID_INDEX, "%p is already in use!", basePtr);
	allocationData_t data =  { basePtr, size };
	BUFpush(&data);
	BUFpop_binded();
	return basePtr;	
}

static bool comparer(void* basePtr, void* data)
{
	return basePtr == (((allocationData_t*)(data))->basePtr);
}

function_signature(void, safe_free, void* basePtr)
{
	BUFpush_binded();
	BUFbind(allocationList);
	ASSERT(BUFfind_index_of(basePtr, comparer) != BUF_INVALID_INDEX, "Invalid Base Address");
	free(basePtr); 
	bool result = BUFremove(basePtr, comparer);
	ASSERT(result == true, "Failed to remove Base Address %p from allocationList", basePtr);
	BUFpop_binded();
}

function_signature(void*, safe_check, void* bytePtr, void* basePtr)
{
	ASSERT(bytePtr != NULL, "bytePtr is NULL");
	ASSERT(basePtr != NULL, "basePtr is NULL");
	BUFpush_binded();
	BUFbind(allocationList);
	buf_ucount_t index;
	ASSERT((index = BUFfind_index_of(basePtr, comparer)) != BUF_INVALID_INDEX, "Invalid Base Address");

	allocationData_t* data = BUFget_ptr_at_typeof(allocationData_t, index);
	ASSERT(data != NULL, "allocationData_t* data == NULL");
	ASSERT(data->basePtr == basePtr, "data->basePtr != basePtr");
	ASSERT(((data->basePtr + data->size) > bytePtr), "Out of bound memory access! data->basePtr + data->size) < bytePt");
	ASSERT(data->basePtr < bytePtr, "Out of bound memory access! data->basePtr < bytePtr");
	BUFpop_binded();
	return bytePtr;
}

function_signature_void(void, safe_memory_init)
{
	ASSERT(allocationList == BUF_INVALID, "allocationList is already initialized");
	allocationList = BUFcreate(NULL, sizeof(allocationData_t), 0, 0);
}

function_signature_void(void, safe_memory_terminate)
{
	ASSERT(allocationList != BUF_INVALID, "allocationList is already terminated");
	BUFpush_binded();
	BUFbind(allocationList);
	BUFfree();
	BUFpop_binded();
}