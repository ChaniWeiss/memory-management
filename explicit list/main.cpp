#include "explicitList.h"


int main()
{
	Person p((char*)"chani", 1, 20);
	Person person(p);
	
	Person* p1 = new Person;
	*p1 = p;
	std::cout << "p1: " << p1 << std::endl;
	Person* p2 = new Person;
	std::cout << "p2: " << p2 << std::endl;
	delete p1;
	Person* p3 = new Person;
	std::cout << "p3: " << p3 << std::endl;
	delete p2;
	Person* p4 = new Person;
	std::cout << "p4: " << p4 << std::endl;
	
	return 1;
}
