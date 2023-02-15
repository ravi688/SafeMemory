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
	ALLOCATION_TYPE_HEAP,
	/* aligned memory block allocated on the heap */
	ALLOCATION_TYPE_ALIGNED_HEAP
} allocation_type_t;

typedef struct 
{
	void* basePtr; 
	u64 size;
} allocationData_t;

enum
{
	ALLOCATION_TYPE_OFFSET = 1,
	ALLOCATION_SIZE_OFFSET = 4
};

#define ALLOCATION_TYPE(basePtr) (*(u8*)((void*)(basePtr) - ALLOCATION_TYPE_OFFSET))
#define ALLOCATION_SIZE(basePtr) (*(u32*)((void*)(basePtr) - ALLOCATION_SIZE_OFFSET))

static bool comparer(void* basePtr, void* data);

static function_signature(void*, register_allocation, void* basePtr, u64 size);
#define register_allocation(...) define_alias_function_macro(register_allocation, __VA_ARGS__)


SAFE_MEMORY_API function_signature(void*, register_stack_allocation, void* basePtr, u64 size)
{
	if((basePtr != NULL) && ((basePtr - HEAD_SIZE) != NULL))
		ALLOCATION_TYPE(basePtr) = ALLOCATION_TYPE_STACK;
	/* if basePtr == NULL then let the assertion happen in the register_allocation function call */
	return register_allocation(basePtr, size);
}

SAFE_MEMORY_API function_signature(void*, register_heap_allocation, void* basePtr, u64 size)
{
	if((basePtr != NULL) && ((basePtr - HEAD_SIZE) != NULL))
		ALLOCATION_TYPE(basePtr) = ALLOCATION_TYPE_HEAP;
	/* if basePtr == NULL then let the assertion happen in the register_allocation function call */
	return register_allocation(basePtr, size);
}

SAFE_MEMORY_API function_signature(void*, register_aligned_heap_allocation, void* basePtr, u64 size)
{
	if((basePtr != NULL) && ((basePtr - HEAD_SIZE) != NULL))
		ALLOCATION_TYPE(basePtr) = ALLOCATION_TYPE_ALIGNED_HEAP;
	/* if basePtr == NULL then let the assertion happen in the register_allocation function call */
	return register_allocation(basePtr, size);
}

SAFE_MEMORY_API function_signature(void*, register_aligned_heap_reallocation, void* basePtr, u64 size, u32 align)
{
	BUFpush_binded();
	BUFbind(allocationList);
	BUFremove(basePtr, comparer);
	void* ptr = register_allocation(basePtr, size);
	BUFpop_binded();
	return ptr;
}

static function_signature(void*, register_allocation, void* basePtr, u64 size)
{
	ASSERT(DESCRIPTION(((basePtr != NULL) && ((basePtr - HEAD_SIZE) != NULL))
	#ifndef SAFE_MEMORY_ZERO_SIZED_ALLOCATION_IGNORE
	 && (size != 0)
	#endif /* SAFE_MEMORY_ZERO_SIZED_ALLOCATION_IGNORE */
	 )
	, 
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
	#endif /* SAFE_MEMORY_ALREADY_IN_USE_IGNORE */

	/* otherwise register a new allocation data */
	ALLOCATION_SIZE(basePtr) = size;
	allocationData_t data =  { basePtr, size };
	BUFpush(&data);
	BUFpop_binded();
	return basePtr;	
}

static bool comparer(void* basePtr, void* data)
{
	return basePtr == (((allocationData_t*)(data))->basePtr);
}

SAFE_MEMORY_API function_signature(bool, safe_silent_free, void* basePtr)
{
	BUFpush_binded();
	BUFbind(allocationList);

	bool is_valid = BUFfind_index_of(basePtr, comparer) != BUF_INVALID_INDEX;
	
	if(!is_valid)
		return false;
	
	switch(ALLOCATION_TYPE(basePtr))
	{
		case ALLOCATION_TYPE_HEAP:
			free(&ALLOCATION_TYPE(basePtr)); 
			break;
		case ALLOCATION_TYPE_ALIGNED_HEAP:
			_aligned_free(&ALLOCATION_TYPE(basePtr));
	}

	bool result = BUFremove(basePtr, comparer);
	ASSERT(DESCRIPTION(result == true), "Failed to remove Base Address %p from allocationList", basePtr);
	
	BUFpop_binded();
	return is_valid;
}

SAFE_MEMORY_API function_signature(void, safe_free, void* basePtr)
{
	bool result = safe_silent_free(basePtr);
	ASSERT(DESCRIPTION(result = true), 
		"the memory block at address %p you are trying to free is not valid\n"
		"It happens due to one or more of the following cases:\n"
		"\t1. You are trying to free a memory block which is not allocated using safe_alloca, safe_malloca, checked_alloca, or checked_malloca\n"
		"\t2. You are trying to free a memory block which has already been freed using safe_free()."
		, basePtr);
}

static bool _comparer(void* pair, void* data)
{
	_assert(sizeof(void*) == sizeof(u64));
	void* ptr = CAST_TO(void*, DEREF_TO(u64, pair));
	u32 size = DEREF_TO(u32, pair + sizeof(void*));
	allocationData_t* alloc_data = CAST_TO(allocationData_t*, data);
	return (ptr >= alloc_data->basePtr) && ((ptr + size) <= (alloc_data->basePtr + alloc_data->size));
}

static allocationData_t* find_allocation_for_address(void* bytePtr, u32 size)
{
	struct pair_t { void* ptr; u32 size; } pair = { bytePtr, size };
	buf_ucount_t index = BUFfind_index_of(&pair, _comparer);
	return (index == BUF_INVALID_INDEX) ? NULL : BUFget_ptr_at_typeof(allocationData_t, index);
}

SAFE_MEMORY_API function_signature(void, safe_memset, void* basePtr, int value, u32 size)
{
	ASSERT(DESCRIPTION((basePtr != NULL) && ((basePtr - HEAD_SIZE) != NULL)), "pointer is NULL and doesn't point to a memory block allocated by safe memory sandbox");
	ASSERT(DESCRIPTION(size != 0), "requested size is zero, nothing to memset");
	BUFpush_binded();
	BUFbind(allocationList);
	allocationData_t* data = find_allocation_for_address(basePtr, size);
	u32 _size = ALLOCATION_SIZE(data->basePtr) - (basePtr - data->basePtr);
	ASSERT(DESCRIPTION(_size >= size),
		"size of the memory block %u you are trying to memset is less than then size you specified %u",
		_size, size);
	memset(basePtr, value, size);
	BUFpop_binded();
}

SAFE_MEMORY_API function_signature(void, safe_memcpy, void* dstPtr, const void* srcPtr, u32 size)
{
	u32 dstSize = ALLOCATION_SIZE(dstPtr);
	u32 srcSize = ALLOCATION_SIZE(srcPtr);
	ASSERT(DESCRIPTION(dstSize >= size), 
		"invalid memory write, size of the destination memory block %u is less than the size you specified %u",
		dstSize, dstSize, size);
	ASSERT(DESCRIPTION(srcSize >= size),
		"invalid memory read, the size of the source memory block %u is less than the size you specified %u",
		srcSize, srcSize, size);
	memcpy(dstPtr, srcPtr, size);
}

SAFE_MEMORY_API function_signature(void*, safe_check, void* bytePtr, u32 size, void* basePtr)
{
	ASSERT(DESCRIPTION(bytePtr != NULL), "the memory reference pointer 'bytePtr' is not a valid pointer as it holds a NULL value");
	BUFpush_binded();
	BUFbind(allocationList);

	if((basePtr != NULL) && ((basePtr - HEAD_SIZE) != NULL))
		ASSERT(DESCRIPTION(BUFfind_index_of(basePtr, comparer) != BUF_INVALID_INDEX), 
			"the memory block at address %p has never been registered in the safe memory sandbox\n"
			"but you are still trying to reference a sub-block at address %p in that memory block"
			, basePtr, bytePtr);

	allocationData_t* data = find_allocation_for_address(bytePtr, size);

	if(((basePtr != NULL) && ((basePtr - HEAD_SIZE) != NULL)) && (data != NULL))
		ASSERT(DESCRIPTION(data->basePtr == basePtr),
			"the memory you are trying to reference %p is valid but it is not contained in the requested memory block at the address %p\n"
			"instead it is located in the memory block at address %p"
			, bytePtr, basePtr, data->basePtr);

	ASSERT(DESCRIPTION(data != NULL), "invalid memory reference range [%p, %p] (size: %u), out of bound access", bytePtr, bytePtr + size - 1, size);

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
		safe_free(CAST_TO(allocationData_t*, BUFget_top())->basePtr);

	BUFfree();
	BUFpop_binded();
}
