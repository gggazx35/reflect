#include "TestClass.h"



void TestClass::overrid(int a)
{
	std::cout << "base poss\n";
}

TestClass* TestClass::New()
{
	auto a = (new (TestClass)());
	return new TestClass();
}