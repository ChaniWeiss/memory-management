#ifndef __EXPLICITLIST_H__
#define __EXPLICITLIST_H__

#include <cstring>
#include <iostream>


class Person{
public:
	Person(){}
	Person(char[32], unsigned int id, unsigned char age);
	Person(const Person& rv);
	Person& operator = (const Person& rv);
	
	void* operator new(size_t size);
	void operator delete(void*);
	
private:
	char m_fullname[32];
	unsigned int m_id;
	unsigned char m_age;
	static char s_pool[];
	static void* s_firstFree;
	void* operator new[](size_t size);
	void operator delete[](void*);
};


inline Person::Person(char fullname[32], unsigned int id, unsigned char age) : m_id(id), m_age(age)
{
	strcpy(m_fullname, fullname);
}


inline Person::Person(const Person& rv) : m_id(rv.m_id), m_age(rv.m_age)
{
	strcpy(m_fullname, rv.m_fullname);
}


inline Person& Person::operator = (const Person& rv)
{
	if(this != &rv)
	{
		strcpy(m_fullname, rv.m_fullname);
		m_id = rv.m_id;
		m_age = rv.m_age;
	}
	
	return *this;
}





#endif
