
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

#ifdef GLOBAL_RELEASE
#	ifndef SAFE_MEMORY_RELEASE
#		define SAFE_MEMORY_RELEASE
#	endif
#endif

#ifdef GLOBAL_DEBUG
#	ifndef SAFE_MEMORY_DEBUG
#		define SAFE_MEMORY_DEBUG
#	endif
#endif

#if defined(SAFE_MEMORY_RELEASE) && defined(SAFE_MEMORY_DEBUG)
#	warning "Both SAFE_MEMORY_RELEASE and SAFE_MEMORY_DEBUG are defined; using SAFE_MEMORY_DEBUG"
#	undef SAFE_MEMORY_RELEASE
#endif

#if !defined(SAFE_MEMORY_RELEASE) && !defined(SAFE_MEMORY_DEBUG)
#	warning "None of SAFE_MEMORY_RELEASE or SAFE_MEMORY_DEBUG is defined; using SAFE_MEMORY_DEBUG"
#	define SAFE_MEMORY_DEBUG
#endif

#ifdef SAFE_MEMORY_DEBUG
#	ifndef CALLTRACE_DEBUG
#		define CALLTRACE_DEBUG
#	endif
#endif

#ifdef SAFE_MEMORY_RELEASE
#	ifndef CALLTRACE_RELEASE
#		define CALLTRACE_RELEASE
#	endif
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

/* 	allocates a memory block on the heap with size 'size' while enabling internal memory safety flags */
#define checked_malloc(size) safe_malloc(size)

/* 	allocates a memory block on the stack with size 'size' while enabling internal memory safety flags */
#define checked_alloca(size) safe_alloca(size)

/*	frees a memory block referenced by 'ptr' (works for both stack and heap allocated memory blocks */
#define checked_free(ptr) safe_free((void*)ptr)

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

#define HEAD_SIZE 1 /* (1 byte) the size of the header appened at every allocated block */

/* allocates a memory block on the stack with size 'size' and returns a pointer to it */
#define safe_alloca(size) register_stack_allocation(alloca(HEAD_SIZE + (size)) + HEAD_SIZE, (size))
/* allocates a memory block on the heap with size 'size' and returns a pointer to it */
#define safe_malloc(size) register_heap_allocation(malloc(HEAD_SIZE + (size)) + HEAD_SIZE, (size))

/* frees a memory block already allocated (works for heap and stack both) */
#define safe_free(...) define_alias_function_macro(safe_free, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void, safe_free, void* basePtr);

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
	returns: the same pointer that has been passes as the first parameter, i.e. 'basePtr'
 */
#define register_heap_allocation(...) define_alias_function_macro(register_heap_allocation, __VA_ARGS__)
SAFE_MEMORY_API function_signature(void*, register_heap_allocation, void* basePtr, u64 size);

END_CPP_COMPATIBLE
