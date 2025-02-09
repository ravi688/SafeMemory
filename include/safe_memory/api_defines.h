#pragma once

#if (defined _WIN32 || defined __CYGWIN__) && defined(__GNUC__)
#	define SAFE_MEMORY_IMPORT_API __declspec(dllimport)
#	define SAFE_MEMORY_EXPORT_API __declspec(dllexport)
#else
#	define SAFE_MEMORY_IMPORT_API __attribute__((visibility("default")))
#	define SAFE_MEMORY_EXPORT_API __attribute__((visibility("default")))
#endif

#ifdef SAFE_MEMORY_BUILD_STATIC_LIBRARY
#	define SAFE_MEMORY_API
#elif defined(SAFE_MEMORY_BUILD_DYNAMIC_LIBRARY)
#	define SAFE_MEMORY_API SAFE_MEMORY_EXPORT_API
#elif defined(SAFE_MEMORY_USE_DYNAMIC_LIBRARY)
#	define SAFE_MEMORY_API SAFE_MEMORY_IMPORT_API
#elif defined(SAFE_MEMORY_USE_STATIC_LIBRARY)
#	define SAFE_MEMORY_API
#else
#	define SAFE_MEMORY_API
#endif
