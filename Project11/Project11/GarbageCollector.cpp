#include "GarbageCollector.h"
#include <iostream>

void GarbageCollector::mark() {
	for (auto ref : refs) {
		if (ref->ptr) markRef(ref->ptr);
	}
}

void GarbageCollector::markRef(void* _this) {
	GET_TAG(_this)->state = EGCState::MARKED;
	auto refs = GET_REFLECTOR(_this);

	match.insert(std::make_pair(_this, nullptr));
	for (auto ref : refs->pointers) {
		auto val = *ref->As<void*>(_this);
		if (val) {
			markRef(val);
			if (match.count(val) == 0)
				match.insert(std::make_pair(val, nullptr));
		}
	}
}

void GarbageCollector::compact() {
	for (auto ref : refs) {
		if (ref->ptr) compactRef(ref->ptr);
	}
}

void GarbageCollector::compactRef(void* _this) {
	auto refs = GET_REFLECTOR(_this);
	for (auto ref : refs->pointers) {
		if (ref) {
			auto ptr = match[*ref->As<void*>(_this)];
			*ref->As<void*>(_this) = ptr;

			if (ptr) compactRef(ptr);
		}
	}
	//std::cout << "*ref->As<void*>(this)" << '\n';
}

void GarbageCollector::sweep() {
	char* buffer = new char[MAX_OBJECT_SIZE];


	size_t bufferSize = 0;
	//auto it = objects.begin();
	for (size_t i = 0; i < allocatedMemory;) {
		auto obj = reinterpret_cast<AllocObj*>(memoryHanlde + i);
		if (obj->state == EGCState::MARKED) {
#ifdef _DEBUG 
			std::cout << obj << " " << obj->size << " is alive\n";
#endif
			obj->state = EGCState::UNMARKED;
			if (match.count(GET_OBJ(obj)))
				match.at(GET_OBJ(obj)) = GET_OBJ(buffer + bufferSize);
			memcpy(buffer + bufferSize, memoryHanlde + i, obj->size);
			bufferSize += obj->size;

		}
		else {
#ifdef _DEBUG 
			std::cout << obj << " has been deleted now size is " << obj->size << '\n';
#endif

			//delete GET_OBJ(obj);
		}
		i += obj->size;
	}
	allocatedMemory = bufferSize;
	delete[] memoryHanlde;
	memoryHanlde = buffer;

	for (auto it = refs.begin(); it != refs.end(); it++) {
		//*(it) = (GCObject*)match.at(*it);
		(*it)->ptr = (void*)match.at((*it)->ptr);
	}


}

void* GarbageCollector::Allocate(size_t _size) {
	int size = _size;
	if (size < DEFAULT_PADDING) size = DEFAULT_PADDING;

	if ((allocatedMemory + size) >= MAX_OBJECT_SIZE) {
		mark();
		sweep();
		compact();
	}

	auto v = reinterpret_cast<AllocObj*>(memoryHanlde + (allocatedMemory));
	v->size = ACTUAL_SIZEOF(size);
	v->state = EGCState::UNMARKED;
#ifdef _DEBUG
	std::cout << v << " " << v->size << '\n';
#endif
	allocatedMemory += ACTUAL_SIZEOF(size);
	return reinterpret_cast<void*>(((char*)v) + sizeof(AllocObj));
}