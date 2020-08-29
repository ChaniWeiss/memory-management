#include "memoryAllocator.h"



int main()
{
	void* memoryPool = malloc(104);
	MemoryAllocator* allocator = MemoryAllocator_init(memoryPool, 104);
	
	
	void* alloc1 = MemoryAllocator_allocate(allocator, 1);
	void* alloc2 = MemoryAllocator_allocate(allocator, 40);
	void* alloc3 = MemoryAllocator_allocate(allocator, 1);
	printf("%p\n", alloc1);
	printf("%p\n", alloc2);
	printf("%p\n", alloc3);
	
	MemoryAllocator_free(allocator, alloc2);
	MemoryAllocator_free(allocator, alloc1);
	
	void* alloc4 = MemoryAllocator_allocate(allocator, 8);
	void* alloc5 = MemoryAllocator_allocate(allocator, 1);
	
	printf("%p\n", alloc4);
	printf("%p\n", alloc5);
	
	MemoryAllocator_release(allocator);
	free(memoryPool);
	return 1;
}
