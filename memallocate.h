#ifndef memallocate_header
#define memallocate_header
#include <stddef.h>

#ifdef __DEBUG__
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

void* memallocate_debug
(
	size_t size, 
	const char* file, 
	size_t line,
	const char* func
);

int memfree_debug(void* ptr);
void memcheck_dump_leaks(void);

#define memallocate(size) memallocate_debug \
( \
	size, \
	__FILE__, \
	__LINE__, \
	__func__ \
) 

#define memfree(ptr) do { \
	int res = memfree_debug(ptr); \
	if (res == -2) { \
		fprintf(stderr, "[CRITICAL ERROR] Попытка " \
			"освободить невалидный указатель " \
			"или куча повреждена! Файл: %s\n" \
			"Строка: %d\nфункция: %s\n", \
			__FILE__, __LINE__, __func__); \
			assert(0); \
		} \
} while(0)

#else /*__RELEASE__*/
#include <stdlib.h>

#define memallocate(size) malloc(size)
#define memfree(ptr) free(ptr)
#define memcheck_dump_leaks() ((void)0)

#endif
#endif
