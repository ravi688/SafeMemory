
#pragma once

#include <template_system.h>
#include <safe_memory/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

#define safe_array(T) c_template(safe_array, T)
#define instantiate_declaration_safe_array(T) SAFE_MEMORY_API T* safe_array(T)(void* buffer, u64 count, ...)
#define instantiate_implementation_safe_array(T) SAFE_MEMORY_API T* safe_array(T)(void* buffer, u64 count, ...)\
{\
	va_list args;\
	va_start(args, count);\
	T* _buffer = (T*)buffer;\
	u64 i = 0;\
	while(count > 0)\
	{\
		_buffer[i] = va_arg(args, T);\
		i++;\
		count--;\
	}\
	va_end(args);\
	return buffer;\
}

#ifdef __cplusplus
}
#endif
