
#pragma once

#include <defines.h>
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

#include <calltrace.h>

/*User*/
#define checked(basePtr, size) register_stack_allocation((void*)(basePtr), size)
#define checked_ref(type, basePtr, index) (*(type*)safe_check((void*)((basePtr) + (index)), (void*)(basePtr)) )
#define checked_malloc(size) safe_malloc(size)
#define checked_alloca(size) safe_alloca(size)
#define checked_free(ptr) safe_free((void*)ptr)
#define checked_array(type, count, ...) safe_array(checked_alloca(should_be_equal_to_word_size(sizeof(type)) * (count)), (count), sizeof(type), __VA_ARGS__)
#define checked_struct_array(type, count, ...) safe_struct_array(checked_alloca(should_be_greater_than_word_size(sizeof(type)) * (count)), (count), sizeof(type), __VA_ARGS__)


#define safe_memory_init(...) define_alias_function_void_macro(safe_memory_init)
function_signature_void(void, safe_memory_init);

#define safe_memory_terminate(...) define_alias_function_void_macro(safe_memory_terminate)
function_signature_void(void, safe_memory_terminate);

#define safe_alloca(size) register_stack_allocation(alloca(1 + (size)) + 1, (size))
#define safe_malloc(size) register_heap_allocation(malloc(1 + (size)) + 1, (size))

#define safe_free(...) define_alias_function_macro(safe_free, __VA_ARGS__)
function_signature(void, safe_free, void* basePtr);

#define safe_check(...) define_alias_function_macro(safe_check, __VA_ARGS__)
function_signature(void*, safe_check, void* bytePtr, void* basePtr);

void* safe_struct_array(void* buffer, u64 count, u64 size, ...);
void* safe_array(void* buffer, u64 count, u64 size, ...);

/*Internal*/
#define register_stack_allocation(...) define_alias_function_macro(register_stack_allocation, __VA_ARGS__)
function_signature(void*, register_stack_allocation, void* basePtr, u64 size);
#define register_heap_allocation(...) define_alias_function_macro(register_heap_allocation, __VA_ARGS__)
function_signature(void*, register_heap_allocation, void* basePtr, u64 size);

#ifdef SAFE_MEMORY_DEBUG
#	define should_be_greater_than_word_size(...) define_alias_function_macro(should_be_greater_than_word_size, __VA_ARGS__)
	function_signature(u64, should_be_greater_than_word_size, u64 size);
#	define should_be_equal_to_word_size(...) define_alias_function_macro(should_be_equal_to_word_size, __VA_ARGS__)
	function_signature(u64, should_be_equal_to_word_size, u64 size);
#else
#	define should_be_greater_than_word_size(x) x
#	define should_be_equal_to_word_size(x) x
#endif