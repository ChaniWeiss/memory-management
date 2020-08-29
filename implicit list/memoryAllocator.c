#include <assert.h>
#include "memoryAllocator.h"


#define alignment 8


struct MemoryAllocator{
	void* 	memoryPool;
	size_t 	size;
};


typedef union MetaData{
	size_t 	size;
	size_t 	isFree : 1; /*1 means the block is free*/
}MetaData;


static const size_t MetaSize = sizeof(MetaData);


/*--------------------------- AUXILIARY FUNCTIONS ---------------------------*/
size_t align(size_t size)
{
	return (size % alignment)? size + (alignment - (size % alignment)) : size;
}


void InitAllocatorFields(MemoryAllocator* const allocator, void* memoryPool, size_t size)
{
	allocator -> memoryPool = memoryPool;
	allocator -> size = size;
}


void InitMetaData(size_t size, size_t isFree, MetaData* metaData)
{
	metaData -> size = size;
	metaData -> isFree = isFree;
}


char MemoryLeft(MemoryAllocator* const allocator, size_t offset, size_t size)
{
	return offset + size + 2*MetaSize <= allocator -> size;
}


char IsFreeBlock(void* payload)
{
	return ((MetaData*)payload - 1) -> isFree;
}


size_t BlockSize(void* ptr)
{
	return ((((MetaData*)ptr - 1)) -> size) / 2 * 2;
}


void GoToNextBlock(MemoryAllocator* const allocator, size_t* offset)
{
	*offset += ((MetaData*)((char*)(allocator -> memoryPool) + *offset)) -> size + 2*MetaSize;
}


void* PayloadAddress(MemoryAllocator* const allocator, size_t offset)
{
	return &((char*)(allocator -> memoryPool))[offset + MetaSize];
}


void PrepareBlock(MemoryAllocator* const allocator, size_t offset, MetaData metaData)
{
	*(MetaData*)((char*)(allocator -> memoryPool)+ offset) = metaData;
	*((MetaData*)((char*)PayloadAddress(allocator, offset) + (metaData.size/2)*2)) = metaData;
}


void SplitBlock(MemoryAllocator* const allocator, size_t offset, size_t size)
{
	size_t freeBlockSize = BlockSize((char*)(allocator -> memoryPool) + offset + MetaSize) - size -2*MetaSize;
	MetaData metaData;
	
	if(size != BlockSize((char*)(allocator -> memoryPool) + offset + MetaSize))
		{
			InitMetaData(freeBlockSize, 1, &metaData);
			PrepareBlock(allocator, offset + size + 2*MetaSize, metaData);	
		}
	
	InitMetaData(size, 0, &metaData);
	PrepareBlock(allocator, offset, metaData);
}


void* Payload(MemoryAllocator* const allocator, size_t offset, size_t size)
{	
	SplitBlock(allocator, offset, size);
	return PayloadAddress(allocator, offset);
}


void* PrevBlock(void* ptr, MemoryAllocator* const allocator)
{
	void* prev = (char*)ptr - 2*MetaSize - BlockSize((char*)ptr - MetaSize);
	return prev < allocator -> memoryPool? NULL : prev;
}


void* NextBlock(void* ptr, MemoryAllocator* const allocator)
{
	void* next = (char*)ptr + BlockSize(ptr) + 2*MetaSize;
	return (char*)next > ((char*)(allocator -> memoryPool)) + (allocator -> size)? NULL : next;
}


void FreeBlock(void* ptr)
{
	((MetaData*)ptr - 1) -> isFree = 1;
	ptr = (char*)ptr + BlockSize(ptr);
	((MetaData*)(ptr))  -> isFree = 1;
}


void JoinWithNextBlock(void* ptr, MemoryAllocator* const allocator)
{
	size_t nextSize = BlockSize(NextBlock(ptr, allocator));
	void* EndOfNextBlock = ((char*)(NextBlock(ptr, allocator))) + nextSize;
	size_t currSize = BlockSize(ptr);
	ptr = (char*)ptr - MetaSize;
	((MetaData*)ptr) -> size = ((MetaData*)EndOfNextBlock) -> size = currSize + nextSize;
	FreeBlock((char*)ptr + MetaSize);
}


void* JoinWithPrevBlock(void* ptr, MemoryAllocator* const allocator)
{
	void* prevBlock = PrevBlock(ptr, allocator);
	
	if(prevBlock && IsFreeBlock(prevBlock))
	{
		JoinWithNextBlock(prevBlock, allocator);
		return prevBlock;
	}
	
	return ptr;
}


/*--------------------------- API FUNCTIONS ---------------------------*/


MemoryAllocator* MemoryAllocator_init(void* memoryPool, size_t size)
{
	MemoryAllocator* memoryAllocater = malloc(sizeof(MemoryAllocator));
	MetaData metaData;
	
	assert(memoryPool);
	
	InitAllocatorFields(memoryAllocater, memoryPool, align(size));
	InitMetaData(align(size) - 2*MetaSize, 1, &metaData);
	PrepareBlock(memoryAllocater, 0, metaData);
	return memoryAllocater;
}


/*might be better to get a ptr to ptr, to enable NULL asignment to allocator*/ 
void* MemoryAllocator_release(MemoryAllocator* const allocator)
{
	void* memPool = allocator -> memoryPool;
	free(allocator);
	return memPool;
}



void* MemoryAllocator_allocate(MemoryAllocator* const allocator, size_t size)
{
	size_t offset = 0;
	size = align(size);
	
	while(MemoryLeft(allocator, offset, size))
	{
		if(!IsFreeBlock(((char*)(allocator->memoryPool))+offset+MetaSize) || BlockSize(((char*)(allocator -> memoryPool))+offset+MetaSize) < size)
		{
			GoToNextBlock(allocator, &offset);
		}		
		else return Payload(allocator, offset, size);			
	}
	return NULL;
}


/* Merge the next adjacent block is free */
void MemoryAllocator_free(MemoryAllocator* const allocator, void* ptr)
{
	if(!ptr)
		return;
		
	FreeBlock(ptr);
	ptr = JoinWithPrevBlock(ptr, allocator);
	
	if(!NextBlock(ptr, allocator))
		return;
	
	if(IsFreeBlock(NextBlock(ptr, allocator)))
	{
		JoinWithNextBlock(ptr, allocator);
	}
}

