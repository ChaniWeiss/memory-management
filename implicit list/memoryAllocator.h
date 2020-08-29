#ifndef __MEMORY_ALLOCATOR_H__
#define __MEMORY_ALLOCATOR_H__

#include <stdlib.h>
#include <stdio.h>

typedef struct MemoryAllocator MemoryAllocator;

MemoryAllocator* 	MemoryAllocator_init(void* memoryPool, size_t size);
void* 				MemoryAllocator_release(MemoryAllocator* const allocator);
void* 				MemoryAllocator_allocate(MemoryAllocator* const allocator, size_t size);
void 				MemoryAllocator_free(MemoryAllocator* const allocator, void* ptr);

#endif
