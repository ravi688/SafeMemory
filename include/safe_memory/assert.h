
#pragma once

#include <safe_memory/defines.h>
#include <common/assert.h>

#define safe_memory_assert(...) __safe_memory_assert(__LINE__, __FUNCTION__, __FILE__, __VA_ARGS__)
void __safe_memory_assert(u32 line, const char* function, const char* file, u64 assertion, ...);

/* override the ASSERT() defined in common/assert.h */
#ifdef ASSERT
#	undef ASSERT
#endif /* ASSERT*/

#define ASSERT(...) safe_memory_assert(__VA_ARGS__)

/* override the assert() defined in common/assert.h */
#ifdef assert
#	undef assert
#endif /* assert */

#ifdef GLOBAL_DEBUG
#	define assert(...) ASSERT(__VA_ARGS__)
#else
#	define assert(...) /* nothing */
#endif /* GLOBAL_DEBUG */
