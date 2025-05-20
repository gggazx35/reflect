#pragma once
#include "TypeResolver.h"


#define REFLECT_START_SUPER(type) \
ObjectReflector type::reflect { &type::initTypeDescriptor, TypeResolver<type::super>::get() }; \
ObjectReflector* type::getReflector() { return &reflect; } \
void type::initTypeDescriptor(ObjectReflector* _desc) { \
using T = type; \
_desc->name = #type; \
_desc->size = sizeof(T);

#define REFLECT_START(type) \
ObjectReflector type::reflect { &type::initTypeDescriptor }; \
ObjectReflector* type::getReflector() { return &type::reflect; } \
void type::initTypeDescriptor(ObjectReflector* _desc) { \
using T = type; \
_desc->name = #type; \
_desc->size = sizeof(T);


//static void register_Reflect() { auto obj = new reflectObject(); TypeManager::registerObject(#type, obj);
#define REFLECT_METHOD(method) _desc->registerMethod(#method, &T::method);
#define REFLECT_FUNCTION(func) _desc->registerFunction(#func, new MethodReflector(&T::func));
#define REFLECT_PROPERTY(prop) _desc->registerProperty(#prop, new PropertyReflector(offsetof(T, prop)));
#define REFLECT_END }
#define REFLECT  \
static void initTypeDescriptor(ObjectReflector* obj); \
static ObjectReflector reflect; \
virtual ObjectReflector* getReflector(); 

