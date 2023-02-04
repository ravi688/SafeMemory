#include <safe_memory/assert.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void __safe_memory_assert(u32 line, const char* function, const char* file, u64 assertion, ...)
{
	if(assertion & 1ULL) return;
	va_list args;
	va_start(args, assertion);
	const char* format = "";
	if(assertion & (1ULL << 16)) format = va_arg(args, const char*);
	debug_logv("[Assertion Failed] [Safe Memory] ", line, function, file, format, args);
	va_end(args);
	exit(0);
}
