#include "explicitList.h"

static const size_t poolSize = 20;


void* pop(void* &head)
{
	if(!head)
		throw std::bad_alloc();
	int* ptr = (int*)head;
	head = *((int**)head);
	return ptr;
}


void push(void* ptr, void* &head)
{
	*(int**)ptr = (int*)head;
	head = ptr;
}


void* Person::operator new(size_t size)
{
	return (pop(s_firstFree));
}


void Person::operator delete(void* person)
{
	push(person, s_firstFree);
}


void* createFreeList(void *pool)
{
	Person* ptr = (Person*)pool;
	
	for(unsigned char i = 0; i < poolSize-1; i++, ptr++)
	{
		*(Person**)ptr = ptr + 1;
	}
	
	*((Person**)ptr) = NULL;
	return pool;
}


char Person::s_pool[sizeof(Person) * 20];
void* Person::s_firstFree = createFreeList(Person::s_pool);

