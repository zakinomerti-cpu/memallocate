#include "memallocate.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef __DEBUG__

#define magic 0xABCDABCDABCD1111

typedef struct AllocationInfo {
	uint64_t	magic;
	size_t 		requested_size;
	
	const char* file;
	size_t		line;
	const char* func;
	
	struct AllocationInfo* prev;
	struct AllocationInfo* next;
	
	uint8_t padding[sizeof(void*) == 4 ? 0 : 8];
	
} AllocationInfo;

static AllocationInfo* last_alloc_info = NULL;

void* memallocate_debug
(
	size_t size, 
	const char* file, 
	size_t line,
	const char* func
) 
{
	size_t allocInfoHeader_size = sizeof(AllocationInfo);
	void* memory = malloc(size+allocInfoHeader_size);
	if(!memory) return NULL;
	
	AllocationInfo* info = (AllocationInfo*)memory;
	info->magic = magic;
	info->requested_size = size;
	
	info->file = file;
	info->line = line;
	info->func = func;
	
	info->prev = last_alloc_info;
	info->next = NULL;
	
	if(last_alloc_info) {
		last_alloc_info->next = info;
	}
	
	last_alloc_info = info;
	
	uint8_t* memory_pointer = (uint8_t*)memory;
	memory_pointer+=allocInfoHeader_size;
	return (void*)memory_pointer;
}

int memfree_debug(void* ptr) {
	if(!ptr) return -1;
	
	size_t allocInfoHeader_size = sizeof(AllocationInfo);
	uint8_t* mem_ptr = (uint8_t*)ptr;
	
	mem_ptr -= allocInfoHeader_size;
	AllocationInfo* info = (AllocationInfo*)mem_ptr;
	
	
	if(info->magic != magic) 
		return -2;
	
	if(info->prev)
		info->prev->next = info->next;
	if(info->next)
		info->next->prev = info->prev;
	else
		last_alloc_info = info->prev;
	
	free(info);
	return 0;
}

void memcheck_dump_leaks(void) {
	if (!last_alloc_info) {
		printf("\n==================================================\n");
		printf("[MEMCHECK] No memory leaks detected. Great job!\n");
		printf("==================================================\n");
		return;
	}

	printf("\n==================================================\n");
	printf("[MEMCHECK] WARNING! Memory leaks detected:\n");
	printf("==================================================\n");

	AllocationInfo* current = last_alloc_info;
	size_t total_leaked_bytes = 0;
	size_t total_leaked_blocks = 0;

	while (current) {
		printf(" -> Block: %p | Size: %zu bytes\n", (void*)current, current->requested_size);
		printf("	Allocated in: %s() -> %s:%zu\n", current->func, current->file, current->line);
		printf(" ------------------------------------------------\n");
		
		total_leaked_bytes += current->requested_size;
		total_leaked_blocks++;
		
		current = current->prev;
	}

	printf("[MEMCHECK] Summary: Found %zu leak(s), total lost: %zu bytes.\n", 
		   total_leaked_blocks, total_leaked_bytes);
	printf("==================================================\n\n");
}

#else


#endif
