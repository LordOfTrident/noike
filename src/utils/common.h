#ifndef COMMON_H_HEADER_GUARD
#define COMMON_H_HEADER_GUARD

#include <stdlib.h>  /* size_t, malloc, realloc, free, exit, EXIT_FAILURE */
#include <stdio.h>   /* fprintf, FILE, fopen, fclose */
#include <assert.h>  /* assert */
#include <stdint.h>  /* uint8_t */
#include <string.h>  /* memset, memcpy, strlen, strcat, strcpy */
#include <stdarg.h>  /* va_list, va_start, va_end, va_arg */
#include <stdbool.h> /* bool, true, false */
#include <time.h>    /* struct tm, time, localtime, strftime,  */

#include <SDL2/SDL.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#	define PLATFORM_WINDOWS
#elif defined(__APPLE__)
#	define PLATFORM_APPLE
#elif defined(__linux__) || defined(__gnu_linux__) || defined(linux)
#	define PLATFORM_LINUX
#elif defined(__unix__) || defined(unix)
#	define PLATFORM_UNIX
#endif

#ifdef PLATFORM_WINDOWS
#	define PATH_SEP "\\"
#else
#	define PATH_SEP "/"
#endif

#ifdef PLATFORM_WINDOWS
#	ifndef MAX_PATH
#		define PATH_MAX 1024
#	else
#		define PATH_MAX MAX_PATH
#	endif
#else
#	ifndef PATH_MAX
#		define PATH_MAX 1024
#	endif
#endif

#define fsJoinPath(...) fsJoinPath_(__VA_ARGS__, NULL)

char       *fsJoinPath_(const char *base, ...);
const char *fsGetExecFolder(void);

int  Log_setup(const char *path, bool append);
void Log_cleanup(void);

typedef enum {
	LogInfo = 0,
	LogWarn,
	LogError,
} LogType;

#define Log_info(...)  Log_from(LogInfo,  __FILE__, __LINE__, __VA_ARGS__)
#define Log_warn(...)  Log_from(LogWarn, __FILE__, __LINE__, __VA_ARGS__)
#define Log_error(...) Log_from(LogError, __FILE__, __LINE__, __VA_ARGS__)

void Log_from(LogType type, const char *path, int line, const char *fmt, ...);

extern int    argc;
extern char **argv;

#define fatal(...)    fatalFrom(__FILE__, __LINE__, __VA_ARGS__)
#define unreachable() (fatal("Unreachable"), exit(EXIT_FAILURE)) /* Silence the compiler */

#define arraySize(X)  (sizeof(X) / sizeof(*(X)))
#define zeroStruct(X) memset(X, 0, sizeof(*(X)))
#define zeroArray(X, COUNT) memset(X, 0, sizeof(*(X)) * COUNT)

#define new(TYPE, COUNT) (TYPE*)xalloc(COUNT * sizeof(TYPE))
#define resize(PTR, COUNT) xrealloc(PTR, COUNT * sizeof(*(PTR)))
#define swap(A, B)                      \
	do {                                \
		assert(sizeof(A) == sizeof(B)); \
		uint8_t tmp_[sizeof(A)];        \
		memcpy(tmp_, &(A), sizeof(A));  \
		A = B;                          \
		memcpy(&(B), tmp_, sizeof(B));  \
	} while (0)

void fatalFrom(const char *path, int line, const char *fmt, ...);

void *xalloc  (size_t bytes);
void *xrealloc(void *ptr, size_t bytes);
void  xfree   (void *ptr);
char *xstrdup (const char *str);

#endif
