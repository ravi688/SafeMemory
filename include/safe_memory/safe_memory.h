
#pragma once

#include <defines.h>
#include <stdlib.h>
#include <calltrace.h>



/*User*/
#define safe_memory_init(...) define_alias_function_void_macro(safe_memory_init)
function_signature_void(void, safe_memory_init);

#define safe_memory_terminate(...) define_alias_function_void_macro(safe_memory_terminate)
function_signature_void(void, safe_memory_terminate);

#define safe_alloca(size) register_allocation(alloca(size), size)
#define safe_malloc(size) register_allocation(malloc(size), size)

#define safe_free(...) define_alias_function_macro(safe_free, __VA_ARGS__)
function_signature(void, safe_free, void* basePtr);

#define safe_check(...) define_alias_function_macro(safe_check, __VA_ARGS__)
function_signature(void*, safe_check, void* bytePtr, void* basePtr);


/*Internal*/
#define register_allocation(...) define_alias_function_macro(register_allocation, __VA_ARGS__)
function_signature(void*, register_allocation, void* basePtr, u64 size);