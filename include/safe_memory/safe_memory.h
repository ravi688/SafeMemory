
/*
	Author: Ravi Prakash Singh (rp0412204@gmail.com)
	---------------------------
	DOCUMENTATION (Safe Memory: A memory sandboxing library)
	---------------------------
	
	SafeMemory is a memory sandboxing library which doesn't actually sandboxes anything at the system level,
	However it does records the activities related to memory allocation, dereferences, and deallocations.

	

 */


#pragma once

#include <safe_memory/defines.h>

#include <stdlib.h>

#if !defined(SAFE_MEMORY_RELEASE) && !defined(SAFE_MEMORY_DEBUG)
#	warning "None of SAFE_MEMORY_RELEASE or SAFE_MEMORY_DEBUG is defined; using SAFE_MEMORY_DEBUG"
#	define SAFE_MEMORY_DEBUG
#endif

#include <calltrace/calltrace.h>

#include <safe_memory/template_instantiations.h>

BEGIN_CPP_COMPATIBLE

/*	dereferences the memory referenced by 'basePtr + index' while checking if the memory reference is a valid reference 
	returns: *(basePtr + index)
 */
#define checked_ref(type, basePtr, index) (*checked_refp(type, basePtr, index))

/*	checks if the memory referenced by 'basePtr + index' is a valid memory reference
	returns: basePtr + index
 */
#define checked_refp(type, basePtr, index) ((type*)safe_check((void*)((basePtr) + (index) * sizeof(type)), sizeof(type), (void*)(basePtr)))

/* allocates a memory block on the heap with size 'size' while enabling internal memory safety flags */
#define checked_malloc(size) safe_malloc(size)

/* reallocates a memory block on the heap with memory block referenced by 'old_ptr' and new size 'size' */
#define checked_realloc(old_ptr, size) safe_realloc(old_ptr, size)

/* allocates an aligned memory block on the heap with size 'size' and alignment 'align' while enabling internal memory safety flags */
#define checked_aligned_malloc(size, align) safe_aligned_malloc(size, align)

/* reallocates an aligned memory block on the heap with earlier pointer 'old_ptr', and new size as 'size' and aligment 'align' while enabling internal memory safety flags */
#define checked_aligned_realloc(old_ptr, size, align) safe_aligned_realloc(old_ptr, size, align)

/* 	allocates a memory block on the stack with size 'size' while enabling internal memory safety flags */
#define checked_alloca(size) safe_alloca(size)

/*	frees a memory block referenced by 'ptr' (works for both stack and heap allocated memory blocks) */
#define checked_free(ptr) safe_free((void*)(ptr))

/*	frees a memory block referenced by 'ptr' (works for both stack and heap allocated memory blocks) 
	without report any errors.
 */
#define checked_silent_free(ptr) safe_silent_free((void*)(ptr))

/* frees an aligned memory block referenced by 'ptr' (works for both stack and heap allocated memory blocks)*/
#define checked_aligned_free(ptr) safe_aligned_free((void*)(ptr))

/* copies the data from the source pointer to the destination pointer while checking their sizes */
#define checked_copy(dstPtr, srcPtr, size) safe_memcpy((void*)(dstPtr), (void*)(srcPtr), size)

/* sets the value 'value' to the entire memory block referenced by the pointer 'dstPtr' */
#define checked_memset(dstPtr, value, size) safe_memset((void*)(dstPtr), value, size)

/*	creates an array allocated on the stack memory while enabling internal memory safety flags 
	returns: pointer to the array
 */
#define checked_array(type, count, ...) safe_array(type)(checked_alloca(sizeof(type) * (count)), (count), __VA_ARGS__)

#define instantiate_implementation_checked_array(T) instantiate_implementation_safe_array(T)
#define instantiate_declaration_checked_array(T) instantiate_declaration_safe_array(T)

/* initializes the memory sandbox */
#define safe_memory_init(...) define_alias_function_void_macro(safe_memory_init)
SAFE_MEMORY_API function_signature_void(void, safe_memory_init);

/* terminates the memory sandbox, also reclaims any memory left to be unallocated before termination */
#define safe_memory_terminate(...) define_alias_function_void_macro(safe_memory_terminate)
SAFE_MEMORY_API function_signature_void(void, safe_memory_terminate);

#define HEAD_SIZE 5 /* (1 byte + 4 byte) the size of the header appened at every allocated block */

#define CLAMP(size) (((size) <= HEAD_SIZE) ? (HEAD_SIZE + 1) : (size))

/* allocates a memory block on the stack with size 'size' and returns a pointer to it */
#define safe_alloca(size) register_stack_allocation(alloca(HEAD_SIZE + (size)) + HEAD_SIZE, (size))
/* allocates a memory block on the heap with size 'size' and returns a pointer to it */
#define safe_malloc(size) register_heap_allocation(malloc(HEAD_SIZE + (size)) + HEAD_SIZE, (size))
/* reallocates a memory block on the heap with old memory block referenced by 'old_ptr' and the new size 'size' */
#define safe_realloc(old_ptr, size) register_aligned_heap_reallocation(old_ptr, realloc((old_ptr == NULL) ? NULL : (old_ptr - HEAD_SIZE), HEAD_SIZE + (size)) + HEAD_SIZE, (size), 0)
/* allocates an aligned memory block on the heap with size 'size' and alignment 'align' and returns a pointer to it */
#define safe_aligned_malloc(size, align) register_aligned_heap_allocation(_aligned_offset_malloc(CLAMP(size), (align), HEAD_SIZE) + HEAD_SIZE, (size))
/* reallocates an aligned memory block on the heap with previous memory block referenced by 'old_ptr', new size 'size' and alignment 'align' and returns a pointer to it */
#define safe_aligned_realloc(old_ptr, size, align) register_aligned_heap_reallocation(old_ptr, _aligned_offset_realloc((old_ptr == NULL) ? NULL : (old_ptr - HEAD_SIZE), CLAMP(size), align, HEAD_SIZE) + HEAD_SIZE, (size), (align))
// #define safe_aligned_realloc(old_ptr, size, align) ((CAST_TO(u64, old_ptr) <= HEAD_SIZE) ? safe_aligned_malloc(size, align) : register_aligned_heap_reallocation(_aligned_realloc(old_ptr, HEAD_SIZE + (size), align + HEAD_SIZE) + HEAD_SIZE, (size), (align)))
/* frees a memory block already allocated (works for heap and stack both) */
#define safe_aligned_free(...) safe_free(__VA_ARGS__)
#define safe_free(...) define_alias_function_macro(safe_free, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void, safe_free, void* basePtr);
#define safe_silent_free(...) define_alias_function_macro(safe_silent_free, __VA_ARGS__)
SAFE_MEMORY_API function_signature(bool, safe_silent_free, void* basePtr);
/* copies data from the destination to the source memory while checking the sizes of the memory blocks */
#define safe_memcpy(...) define_alias_function_macro(safe_memcpy, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void, safe_memcpy, void* dstPtr, const void* srcPtr, u32 size);

/* sets the value 'value' to the entire memory block referenced by the pointer 'dstPtr' */
#define safe_memset(...) define_alias_function_macro(safe_memset, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void, safe_memset, void* basePtr, int value, u32 size);

/*	checks if a memory address 'bytePtr' is a valid byte reference in the memory block referenced by 'basePtr' 
	returns: the same ptr 'bytePtr'
 */
#define safe_check(...) define_alias_function_macro(safe_check, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void*, safe_check, void* bytePtr, u32 size, void* basePtr);


/*Internal*/

/*	registers a memory block, allocated on the stack, starting at the address 'basePtr' and size 'size'
	returns: the same pointer that has been passes as the first parameter, i.e. 'basePtr'
 */
#define register_stack_allocation(...) define_alias_function_macro(register_stack_allocation, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void*, register_stack_allocation, void* basePtr, u64 size);

/*	registers a memory block, allocated on the heap, starting at the address 'basePtr' and size 'size'
	returns: the same pointer that has been passed as the first parameter, i.e. 'basePtr'
 */
#define register_heap_allocation(...) define_alias_function_macro(register_heap_allocation, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void*, register_heap_allocation, void* basePtr, u64 size);

/* registers an aligned memory block, allocated on the heap, starting at the address 'basePtr' and 'size' 
   returns: the same pointer that has been passed as the first parameter, i.e. 'basePtr'
 */
#define register_aligned_heap_allocation(...) define_alias_function_macro(register_aligned_heap_allocation, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void*, register_aligned_heap_allocation, void* basePtr, u64 size);

/* registers an aligned memory block, reallocated on the heap,
 	note: it does't returns the first parameter, instead it returns newly allocated memory block's address
 */
#define register_aligned_heap_reallocation(...) define_alias_function_macro(register_aligned_heap_reallocation, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void*, register_aligned_heap_reallocation, void* oldPtr, void* basePtr, u64 size, u32 align);

END_CPP_COMPATIBLE
