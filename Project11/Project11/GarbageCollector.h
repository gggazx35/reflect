#pragma once
#include "ObjectReflector.h"
#define MAX_OBJECT_SIZE 12800
#define GC_OBJECT_SIZE (MAX_OBJECT_SIZE - 800)
#define DEFAULT_PADDING 8

enum class EGCState : unsigned char {
	UNMARKED,
	MARKED,
	DEAD
};

class AllocObj {
public:
	EGCState state;
	unsigned int size;
	ObjectReflector* reflector;
};

#define ACTUAL_SIZEOF(size) (size + sizeof(AllocObj))
#define GET_TAG(x) reinterpret_cast<AllocObj*>(((char*)x) - sizeof(AllocObj))
#define GET_OBJ(x) reinterpret_cast<void*>(((char*)x) + sizeof(AllocObj))
#define GET_REFLECTOR(x) GET_TAG(x)->reflector



class GCPointer {
public:
	void* ptr;
};
class GarbageCollector {
private:
	GarbageCollector() {}
	GarbageCollector(const GarbageCollector& ref) {}
	GarbageCollector& operator=(const GarbageCollector& ref) {}
	~GarbageCollector() {}
public:
	char* memoryHanlde = new char[MAX_OBJECT_SIZE];
	size_t allocatedMemory = 0;
	
	std::unordered_map<void*, void*> match;
	std::list<GCPointer*> refs;
	void mark();
	void markRef(void* _this);
	void compactRef(void* _this);
	void sweep();
	void compact();
	void* Allocate(size_t size);
	static GarbageCollector* get() {
		static GarbageCollector instance;
		return &instance;
	}
};

template<typename T>
class GCPtr : public GCPointer {
public:
	GCPtr<T>(T* _ptr) {
		ptr = _ptr;
		GarbageCollector::get()->refs.push_back(this);
	}

	~GCPtr<T>() {
		GarbageCollector::get()->refs.remove(this);
	}
	T* operator->() {
		return reinterpret_cast<T*>(ptr);
	}
};