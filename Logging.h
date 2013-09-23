//----------------------------------------------
// Oculus + OpenGL + SDL 2 minimal demo. (With distortion filter).
// 
// Author : Andrew Copland.
//----------------------------------------------

#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <cstdio>

namespace {
	void trace(const char* format, ...) 
	{
		char temp[1024];
		va_list	args;
		va_start(args, format);
		vsprintf_s(temp, sizeof(temp), format, args);
		va_end(args);
		OutputDebugStringA(temp);
		OutputDebugStringA("\n");
		printf("%s\n", temp);
	}

	void debug_break() 
	{
		__asm int 3; 
	}
};

#define TRACE(A, ...)  { trace("[TRACE] %s", A, __VA_ARGS__); }
#define WARN(A, ...)   { trace("[WARN]  %s", A, __VA_ARGS__); }
#define FATAL(A, ...)  { trace("[FATAL] %s", A, __VA_ARGS__); debug_break(); }
#define ASSERT(A)	   { if(!(A)) { trace("[ASSERT] %s", #A); debug_break(); } }

#endif // _LOGGING_H_