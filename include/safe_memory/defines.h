
#pragma once

#include <common/defines.h>

#ifdef SAFE_MEMORY_STATIC_LIBRARY
#	define SAFE_MEMORY_API
#elif SAFE_MEMORY_DYNAMIC_LIBRARY
#	define SAFE_MEMORY_API __declspec(dllimport)
#elif BUILD_DYNAMIC_LIBRARY
#	define SAFE_MEMORY_API __declspec(dllexport)
#else
#	define SAFE_MEMORY_API
#endif

