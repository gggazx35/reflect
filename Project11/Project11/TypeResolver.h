#pragma once
#include "ObjectReflector.h"
template<class T>
struct TypeResolver {
	static ObjectReflector* get() {
		return &T::reflect;
	}
};

