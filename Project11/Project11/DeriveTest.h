#pragma once
#include "TestClass.h"

class DeriveTest : public TestClass
{
	using super = TestClass;
	void overrid(int a) override;
public:
	REFLECT
};

class Derive2Test : public DeriveTest
{
	using super = DeriveTest;
	void overrid(int a) override;
public:
	REFLECT
};

class DTest : public DeriveTest
{
	using super = DeriveTest;
	void overrid(int a) override;
public:
	REFLECT
};

class TTest : public TestClass
{
	using super = TestClass;
	void overrid(int a) override;
public:
	REFLECT
};

class ITest
{
	using super = void;
public:
	REFLECT_INTERFACE
};
