
#pragma once

#include <safe_memory/template_defintions.h>

#include <safe_memory/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

instantiate_declaration_safe_array(u8);
instantiate_declaration_safe_array(u16);
instantiate_declaration_safe_array(u32);
instantiate_declaration_safe_array(u64);
instantiate_declaration_safe_array(s8);
instantiate_declaration_safe_array(s16);
instantiate_declaration_safe_array(s32);
instantiate_declaration_safe_array(s64);
instantiate_declaration_safe_array(float);
instantiate_declaration_safe_array(double);
instantiate_declaration_safe_array(char);

#ifdef SAFE_MEMORY_IMPLEMENTATION
instantiate_implementation_safe_array(double);
instantiate_implementation_safe_array(u64);
instantiate_implementation_safe_array(s64);

float* safe_array(float)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	float* _buffer = (float*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, double);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

char* safe_array(char)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	char* _buffer = (char*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, s64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

u8* safe_array(u8)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	u8* _buffer = (u8*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, u64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}
u16* safe_array(u16)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	u16* _buffer = (u16*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, u64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

u32* safe_array(u32)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	u32* _buffer = (u32*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, u64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

s8* safe_array(s8)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	s8* _buffer = (s8*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, s64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

s16* safe_array(s16)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	s16* _buffer = (s16*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, s64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}

s32* safe_array(s32)(void* buffer, u64 count, ...)
{
	va_list args;
	va_start(args, count);
	s32* _buffer = (s32*)buffer;
	u64 i = 0;
	while(count > 0)
	{
		_buffer[i] = va_arg(args, s64);
		i++;
		count--;
	}
	va_end(args);
	return buffer;
}
#endif


#ifdef __cplusplus
}
#endif
