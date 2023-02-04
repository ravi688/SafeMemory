#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <memory.h>
#include <bufferlib/buffer.h>

#define SAFE_MEMORY_IMPLEMENTATION
#include <safe_memory/safe_memory.h>

#include <safe_memory/assert.h>
#include <safe_memory/defines.h>

static pBUFFER allocationList = BUF_INVALID;

typedef enum allocation_type_t
{
	/* memory block allocated on the stack */
	ALLOCATION_TYPE_STACK,
	/* memory block allocated on the heap */
	ALLOCATION_TYPE_HEAP
} allocation_type_t;

typedef struct 
{
	void* basePtr; 
	u64 size;
} allocationData_t;

#define ALLOCATION_TYPE(basePtr) (*(u8*)((void*)basePtr - HEAD_SIZE))

static bool comparer(void* basePtr, void* data);

static function_signature(void*, register_allocation, void* basePtr, u64 size);
#define register_allocation(...) define_alias_function_macro(register_allocation, __VA_ARGS__)


SAFE_MEMORY_API function_signature(void*, register_stack_allocation, void* basePtr, u64 size)
{
	if(basePtr != NULL)
		ALLOCATION_TYPE(basePtr) = ALLOCATION_TYPE_STACK;
	/* if basePtr == NULL then let the assertion happen in the register_allocation function call */
	return register_allocation(basePtr, size);
}

SAFE_MEMORY_API function_signature(void*, register_heap_allocation, void* basePtr, u64 size)
{
	if(basePtr != NULL)
		ALLOCATION_TYPE(basePtr) = ALLOCATION_TYPE_HEAP;
	/* if basePtr == NULL then let the assertion happen in the register_allocation function call */
	return register_allocation(basePtr, size);
}

static function_signature(void*, register_allocation, void* basePtr, u64 size)
{
	ASSERT(DESCRIPTION((basePtr != NULL) && (size != 0)), 
		"Allocation registration failed for the memory block at %p and size %u bytes\n"
		"Either the memory address equal to NULL or the size of the memory block is 0\n"
		"It happens due one or more of the following cases:\n"
		"\t1. calling safe_alloca --> calls stdlib::alloca, that might have returned NULL value\n"
		"\t2. you tried to call register_allocate with NULL or zero valued parameter"
		, basePtr, size);
	BUFpush_binded();
	BUFbind(allocationList);
	buf_ucount_t result = BUFfind_index_of(basePtr, comparer);
	#ifndef SAFE_MEMORY_ALREADY_IN_USE_IGNORE
	ASSERT(DESCRIPTION(result == BUF_INVALID_INDEX), 
		"%p is already in use with size %u bytes\n"
		"It happens due to one or more of the following cases:\n"
		"\t1. you forgot to free a memory block using safe_free that you are supposed to do\n"
		"\t2. you are trying to call realloc which is returning the same address, currently it is not suported\n"
		"\t3. if the memory block has been allocated on the stack then it might the case of stack corruption.\n"
		, basePtr, size);
	#else
	if(result != BUF_INVALID_INDEX)
	{
		/* just update the size of the memory block if it is not equal to the previous one */
		allocationData_t* data = BUFget_ptr_at_typeof(allocationData_t, result);
		if ((data->basePtr == basePtr) && (data->size != size))
			data->size = size;
		BUFpop_binded();
		return basePtr;
	}
	#endif

	/* otherwise register a new allocation data */
	allocationData_t data =  { basePtr, size };
	BUFpush(&data);
	BUFpop_binded();
	return basePtr;	
}

static bool comparer(void* basePtr, void* data)
{
	return basePtr == (((allocationData_t*)(data))->basePtr);
}

SAFE_MEMORY_API function_signature(void, safe_free, void* basePtr)
{
	BUFpush_binded();
	BUFbind(allocationList);
	ASSERT(BUFfind_index_of(basePtr, comparer) != BUF_INVALID_INDEX, 
		"the memory block at address %p you are trying to free is not valid\n"
		"It happens due to one or more of the following cases:\n"
		"\t1. You are trying to free a memory block which is not allocated using safe_alloca, safe_malloca, checked_alloca, or checked_malloca\n"
		"\t2. You are trying to free a memory block which has already been freed using safe_free()."
		, basePtr);

	/* if the memory block had been allocated on the heap then free it */
	if(ALLOCATION_TYPE(basePtr) == ALLOCATION_TYPE_HEAP)
		free(&ALLOCATION_TYPE(basePtr)); 
	
	bool result = BUFremove(basePtr, comparer);
	ASSERT(DESCRIPTION(result == true), "Failed to remove Base Address %p from allocationList", basePtr);
	BUFpop_binded();
}

SAFE_MEMORY_API function_signature(void*, safe_check, void* bytePtr, void* basePtr)
{
	ASSERT(DESCRIPTION(bytePtr != NULL), "the memory reference pointer 'bytePtr' is not a valid pointer as it holds a NULL value");
	ASSERT(DESCRIPTION(basePtr != NULL), "the memory block pointer (base pointer) 'basePtr' is not a valid poitner as it holds a NULL value");
	BUFpush_binded();
	BUFbind(allocationList);
	buf_ucount_t index;
	ASSERT((index = BUFfind_index_of(basePtr, comparer)) != BUF_INVALID_INDEX, 
		"the memory block at address %p has never been registered in the safe memory sandbox\n"
		"but you are still trying to reference a sub-block at address %p in that memory block\n"
		, basePtr, bytePtr);

	allocationData_t* data = BUFget_ptr_at_typeof(allocationData_t, index);
	ASSERT(DESCRIPTION(data != NULL), "data == NULL, allocationData_t for the memory block at address %p is not found due to unkown reason", basePtr);
	ASSERT(DESCRIPTION(data->basePtr == basePtr), "data->basePtr != basePtr, allocationData for the memory block at address %p is corrupt due to unknown reason", basePtr);
	//TODO: replace data->basePtr - 1 with &ALLOCATION_TYPE(basePtr)
	ASSERT(DESCRIPTION(((data->basePtr - HEAD_SIZE + data->size) >= bytePtr) && (data->basePtr <= bytePtr)), 
		"invalid memory reference at %p (out of bound memory access)\n"
		"you are trying to access the memory at adress %p within a memory block at address %p with size %u bytes which doesn't contain that address",
		bytePtr, bytePtr, basePtr, data->size);
	BUFpop_binded();
	return bytePtr;
}

SAFE_MEMORY_API function_signature_void(void, safe_memory_init)
{
	ASSERT(DESCRIPTION(allocationList == BUF_INVALID), "safe memory sandbox is already initialized");
	allocationList = BUFcreate(NULL, sizeof(allocationData_t), 0, 0);
}

SAFE_MEMORY_API function_signature_void(void, safe_memory_terminate)
{
	ASSERT(DESCRIPTION(allocationList != BUF_INVALID), "safe memory sandbox is already terminated");
	BUFpush_binded();
	BUFbind(allocationList);

	/* free the memory allocations which were not freed by the user */
	buf_ucount_t allocation_count = BUFget_element_count();
	for(buf_ucount_t i = 0; i < allocation_count; i++)
		safe_free(BUFget_top());

	BUFfree();
	BUFpop_binded();
}
