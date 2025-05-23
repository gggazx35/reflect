#include "GarbageCollector.h"
#include <iostream>

GarbageCollector::GarbageCollector()
{
	
	for (int i = 0; i < regions.size(); i++) {
		pushUnused(i);
		regions[i].memory = ((char*)(memoryHanlde) + (i * MAX_REGION_CAPACITY));
	}

	eden = popUnused();
}

//void GarbageCollector::mark() {
//	for (auto ref : refs) {
//		if (ref->ptr){ 
//			markRef(ref->ptr);
//			GET_TAG(ref->ptr)->state = EGCState::LATER;
//		}
//	}
//}
// to impl set gray when ref
void GarbageCollector::mark() {
	//pushUnused(eden);
	//youngRegions.push_back(eden);
	for (auto ref : refs) {
		if (ref->ptr) {
			registerGray(ref->ptr);
		}
	}
	eden = popUnused();
}

void GarbageCollector::markRef(void* _this) {
	GET_TAG(_this)->state = EGCState::BLACK;
	auto refs = GET_REFLECTOR(_this);

	//match.insert(std::make_pair(_this, nullptr));
	for (auto ref : refs->pointers) {
		auto val = *ref->As<void*>(_this);
		auto ptr = ref->As<void*>(_this);
		if (val) {
			registerGray(val);

			if (match.count(val) == 0) 
				match.insert(std::make_pair(val, ptr));
		}
	}
	pushLive(_this);
	gray.pop_front();
}
//
//void GarbageCollector::markRef(void* _this) {
//	GET_TAG(_this)->state = EGCState::MARKED;
//	auto refs = GET_REFLECTOR(_this);
//
//	match.insert(std::make_pair(_this, nullptr));
//	for (auto ref : refs->pointers) {
//		auto val = *ref->As<void*>(_this);
//		if (val) {
//			markRef(val);
//			if (match.count(val) == 0)
//				match.insert(std::make_pair(val, nullptr));
//		}
//	}
//}

void GarbageCollector::compact() {
	for (auto ref : refs) {
		if (ref->ptr) compactRef(ref->ptr);
	}
}

void GarbageCollector::grayOut()
{
	auto i = gray.size();
	while (i != 0) {
		markRef(gray.front());
		i--;
	}
	//if (gray.empty()) sweep2();
	//gray.clear();
}

void GarbageCollector::registerGray(void* val)
{
	if (GET_TAG(val)->state == EGCState::WHITE) {
		gray.push_back(val);
		GET_TAG(val)->state = EGCState::GRAY;
	}
}

void GarbageCollector::pushUnused(int region)
{
	unusedRegions.push_back(region);
	regions[region].age = 0;
	regions[region].usedSize = 0;
	regions[region].liveNodes.clear();
}

//Region* GarbageCollector::newRegion()
//{
//	Region* newRegion = new Region;
//	unusedRegions[0];
//	allocatedMemory += MAX_REGION_CAPACITY;
//
//	return newRegion;
//}

int GarbageCollector::popUnused()
{
	//Region* re = nullptr;
	if (unusedRegions.empty()) {
		terminate();
	}
	auto re = unusedRegions.front();
	unusedRegions.pop_front();
	return re;
}

void GarbageCollector::compactRef(void* _this) {
	//auto refs = GET_REFLECTOR(_this);
	//for (auto ref : refs->pointers) {
	//	if (ref) {
	//		auto ptr = match[*ref->As<void*>(_this)];
	//		*ref->As<void*>(_this) = ptr;

	//		if (ptr) compactRef(ptr);
	//	}
	//}
	////std::cout << "*ref->As<void*>(this)" << '\n';
}
void GarbageCollector::sweep2(int fromRegion, int toRegion) {
	auto liveList = regions[fromRegion].liveNodes.data();
	int i = regions[fromRegion].liveNodes.size() - 1;
	int age = regions[fromRegion].age;
	while (i >= 0) {
		void* ref = liveList[i];
		auto obj = GET_TAG(ref);
		if (obj->state == EGCState::BLACK) {
#ifdef _DEBUG 
			std::cout << (int)obj->age << " " << obj->size << " is alive\n";
#endif

			obj->state = EGCState::WHITE;
			move(obj, toRegion);

			//live.push_back(ref);

		}
#ifdef _DEBUG 
		else
			std::cout << obj << " has been deleted now size is " << obj->size << '\n';
#endif
		i--;
	}

	if (age > 1) {
		age = 204;
	}

	if (regions[toRegion].usedSize > 0) {
		regions[toRegion].age = age + 1;
		youngRegions.push_back(toRegion);
		std::cout << "region ex id: " << fromRegion << ", now id" << toRegion << " aged\n";
	}
	else {
		pushUnused(toRegion);
		std::cout << "region id: " << toRegion << " has been freed\n";
	}
	pushUnused(fromRegion);



	//liveList.clear();
}

void GarbageCollector::sweep() {
//	size_t bufferSize = 0;
//	//auto it = objects.begin();
//	for (size_t i = 0; i < eden->;) {
//		auto obj = reinterpret_cast<AllocObj*>(memoryHanlde + i);
//		if (obj->state == EGCState::BLACK) {
//#ifdef _DEBUG 
//			std::cout << obj << " " << obj->size << " is alive\n";
//#endif
//			obj->state = EGCState::DEAD;
//			//if (match.count(GET_OBJ(obj)))
//			//	match.at(GET_OBJ(obj)) = GET_OBJ(buffer + bufferSize);
//			//memcpy(buffer + bufferSize, memoryHanlde + i, obj->size);
//			//bufferSize += obj->size;
//
//		}
//		else if (obj->state == EGCState::WHITE) {
//#ifdef _DEBUG 
//			std::cout << obj << " has been deleted now size is " << obj->size << '\n';
//#endif
//
//			//delete GET_OBJ(obj);
//		}
//		i += obj->size;
//	}
//	allocatedMemory = 0;
//	//delete[] memoryHanlde;
//
//	for (auto it = refs.begin(); it != refs.end(); it++) {
//		//*(it) = (GCObject*)match.at(*it);
//		(*it)->ptr = (void*)match.at((*it)->ptr);
//	}


}
////
////void GarbageCollector::sweep() {
////	char* buffer = new char[MAX_OBJECT_SIZE];
////
////
////	size_t bufferSize = 0;
////	//auto it = objects.begin();
////	for (size_t i = 0; i < allocatedMemory;) {
////		auto obj = reinterpret_cast<AllocObj*>(memoryHanlde + i);
////		if (obj->state == EGCState::MARKED) {
////#ifdef _DEBUG 
////			std::cout << obj << " " << obj->size << " is alive\n";
////#endif
////			obj->state = EGCState::UNMARKED;
////			if (match.count(GET_OBJ(obj)))
////				match.at(GET_OBJ(obj)) = GET_OBJ(buffer + bufferSize);
////			memcpy(buffer + bufferSize, memoryHanlde + i, obj->size);
////			bufferSize += obj->size;
////
////		}
////		else {
////#ifdef _DEBUG 
////			std::cout << obj << " has been deleted now size is " << obj->size << '\n';
////#endif
////
////			//delete GET_OBJ(obj);
////		}
////		i += obj->size;
////	}
////	allocatedMemory = 0;
////	delete[] memoryHanlde;
////
////	for (auto it = refs.begin(); it != refs.end(); it++) {
////		//*(it) = (GCObject*)match.at(*it);
////		(*it)->ptr = (void*)match.at((*it)->ptr);
////	}
////
////
////}

void* GarbageCollector::Allocate(size_t _size) {
	int size = _size;
	//if (size < DEFAULT_PADDING) size = DEFAULT_PADDING;

	if (!onGC && (regions[eden].usedSize + ACTUAL_SIZEOF(size)) >= MAX_REGION_CAPACITY) {

		time_t start, finish;
		//double duration;

		start = clock();

		int num = eden;
		onGC = true;
		mark();
		//grayOut();
		while (!gray.empty()) {
			grayOut();
		}
		int i = youngRegions.size();
		while (i != 0) {
			int region = youngRegions.front();
			youngRegions.pop_front();
			sweep2(region, popUnused());
			i--;
		}
		
		//std::cout << "fas" << '\n';
		onGC = false;

		finish = clock();

		std::cout << "gc taks " << finish - start << std::endl;

		sweepRegions.clear();
	}

	auto v = reinterpret_cast<AllocObj*>(memoryHanlde + (regions[eden].usedSize));
	v->size = ACTUAL_SIZEOF(size);
	v->state =/* onGC ? EGCState::GRAY : */EGCState::WHITE;
	v->regionID = eden;
	v->age = 0;
#ifdef _DEBUG
	//std::cout << v << " " << v->size << '\n';
#endif
	regions[eden].usedSize += ACTUAL_SIZEOF(size);
	return reinterpret_cast<void*>(((char*)v) + sizeof(AllocObj));
}

void* GarbageCollector::move(AllocObj* tag, int toRegion)
{

	unsigned int size = tag->size;
	tag->regionID = toRegion;
	tag->age++;
	
	void* exAddr = tag;
	void* newAddr = currentRegionAddress(toRegion);
	memcpy(newAddr, exAddr, size);
	regions[toRegion].usedSize += size;

	if(match.count(GET_OBJ(exAddr)))
		*match[GET_OBJ(exAddr)] = GET_OBJ(newAddr);

	std::cout << "aged " <<  (int)(((AllocObj*)newAddr)->age) << '\n';

	return newAddr;
}
