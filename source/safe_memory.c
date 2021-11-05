#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <memory.h>
#include <buffer.h>
#include <safe_memory.h>

static pBUFFER allocationList = BUF_INVALID;

#ifdef ASSERT
#	undef ASSERT
#endif

#if defined(SAFE_MEMORY_DEBUG)
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

#define HEAD_BYTE(basePtr) (*((u8*)basePtr - 1))

static bool comparer(void* basePtr, void* data);

function_signature(static void*, register_allocation, void* basePtr, u64 size);
#define register_allocation(...) define_alias_function_macro(register_allocation, __VA_ARGS__)


function_signature(void*, register_stack_allocation, void* basePtr, u64 size)
{
	HEAD_BYTE(basePtr) = 0;		//stack allocation
	return register_allocation(basePtr, size);
}

function_signature(void*, register_heap_allocation, void* basePtr, u64 size)
{
	HEAD_BYTE(basePtr) = 1;		//heap allocation
	return register_allocation(basePtr, size);
}

function_signature(static void*, register_allocation, void* basePtr, u64 size)
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
	if(HEAD_BYTE(basePtr))
		free(&HEAD_BYTE(basePtr)); 
	
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
	ASSERT((data->basePtr - 1 + data->size) >= bytePtr, "Out of bound memory access! (data->basePtr + data->size) =< bytePt");
	ASSERT(data->basePtr <= bytePtr, "Out of bound memory access! data->basePtr > bytePtr");
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

void* safe_array(void* buffer, u64 count, u64 size, ...)
{
	void* ptr = &size; ptr += 8/*bytes*/;
	u64 i = 0;
	while(i < count)
	{
		memcpy(buffer + size * i, ptr, size);
		ptr += size;
		i++;
	}
	return buffer;
}

void* safe_struct_array(void* buffer, u64 count, u64 size, ...)
{
	va_list args;
	va_start(args, size); 
	u64 i = 0;
	while(count > 0)
	{
		typedef struct  { u8 bytes[size]; } _data;
		_data b;
		b = va_arg(args, _data);
		memcpy(buffer + i * size, &b, size);
		--count;
		++i;
	}
	return buffer;
}

#ifdef SAFE_MEMORY_DEBUG
function_signature(u64, should_be_greater_than_word_size, u64 size)
{
	ASSERT(size > sizeof(u64), "You should use checked_array instead of checked_struct_array; size > sizeof(u64)");
	return size;
}

function_signature(u64, should_be_equal_to_word_size, u64 size)
{
	ASSERT(size == sizeof(u64), "size != sizeof(u64)");
	return size;
}
#endif