
#pragma once

#include <stdint.h>

typedef uint32_t u32;
typedef uint64_t u64;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t s32;
typedef int64_t s64;
typedef int16_t s16;
typedef int8_t s8;


#ifdef SAFE_MEMORY_STATIC_LIBRARY
#	define SAFE_MEMORY_API
#elif SAFE_MEMORY_DYNAMIC_LIBRARY
#	define SAFE_MEMORY_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define SAFE_MEMORY_API __declspec(dllexport)
#else
#	define SAFE_MEMORY_API
#endif

