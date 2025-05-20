#pragma once
#include "Reflection.h"
#include <iostream>


class TestClass {
public:
	using super = void;
	int truea;
	void one() {
		std::cout << "y cccccc";
	}
private:
	virtual void overrid(int a);
	static void ugh();

public:
	TestClass() {}
	static TestClass* New();

	REFLECT
};
REFLECT_START(TestClass)
	REFLECT_METHOD(overrid)
	REFLECT_FUNCTION(New)
REFLECT_END