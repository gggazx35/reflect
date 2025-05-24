#include "GarbageCollector.h"
#include <iostream>
#include <thread>
#include "ThreadPool.h"
GarbageCollector::GarbageCollector()
{
	
	for (int i = 0; i < regions.size()-1; i++) {
		//pushUnused(i);
		unusedRegions.push_back(i);
		//regions[i].memory = new char[MAX_REGION_CAPACITY];
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
	//youngRegions.push_;back(eden);
	std::cout << "FFAS\n";
	for (auto ref : refs) {
		if (ref->ptr) {
			registerGray(ref->ptr, gray);
			//match[ref->ptr].push_back(&ref->ptr);
			referenceMatch[ref->ptr].push_back(ref);
		}
	}
	//eden = popUnused();
}

void GarbageCollector::markRef(void* _this) {
	if (!_this) return;
	GET_TAG(_this)->state = EGCState::BLACK;
	auto refs = GET_REFLECTOR(_this);

	//match.insert(std::make_pair(_this, nullptr));
	if (!refs) return;
	auto pointers = refs->pointers;
	for (auto ref : pointers) {
		void* val = *ref->As<void*>(_this);
		void** ptr = ref->As<void*>(_this);
		if (val) {
			//registerGray(val);
			if(GET_TAG(val)->state == EGCState::WHITE) markRef(val);
			//if (match.count(val) == 0) 
			//match[val].push_back(ptr);
		}
	}
	pushLive(_this);
	//gray.pop_front();
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
	//auto i = gray.size();{
	//onMarking = true;
	{
		ThreadPool threads(4);
		for (auto i : gray) {
			threads.EnqueueJob([this, i]() { this->markRef(i); });
			//gray.pop_front();
		}
		printf("im not\n");
	}
	gray.clear();
	onMarking = false;
	//if (gray.empty()) sweep2();
	//gray.clear();
}

void GarbageCollector::registerGray(void* val, std::deque<void*>& gray)
{
	EGCState state = GET_TAG(val)->state;
	int region = GET_TAG(val)->regionID;
	if (state == EGCState::WHITE) {
		gray.push_back(val);
		GET_TAG(val)->state = EGCState::GRAY;
	}
}

void GarbageCollector::startGC()
{

	time_t start, finish;
	//double duration;

	start = clock();
	eden = popUnused();
	do {

		if (onGC == false) {
			mark();
			onMarking = true;
			std::thread markThread([this]() { this->grayOut(); });
			markThread.detach();
		}
		else {
			while (onMarking == true);
			sweep();
			onGC = false;
			continue;
		}
	} while (0);
	onGC = true;

	finish = clock();

	std::cout << "gc taks " << finish - start << std::endl;
}

void GarbageCollector::pushUnused(int region)
{
	sweepRegions.erase(region);
	unusedRegions.push_back(region);
	regions[region].age = 0;
	regions[region].usedSize = 0;
	regions[region].liveNodes.clear();
}

void GarbageCollector::mainMark()
{
	std::thread markThread();
	while (!gray.empty()) {
		
		grayOut();
	}
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
	sweepRegions.insert(re);
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
void GarbageCollector::sweep2(SweepData& data) {
	auto liveList = regions[data.fromRegion].liveNodes.data();
	int i = regions[data.fromRegion].liveNodes.size() - 1;
	int age = regions[data.fromRegion].age;
	while (i >= 0) {
		void* ref = liveList[i];
		auto obj = GET_TAG(ref);
		if (obj->state == EGCState::BLACK) {
#ifdef _DEBUG 
			std::cout << (int)obj->age << " " << obj->size << " is alive\n";
#endif

			obj->state = EGCState::WHITE;
			move(obj, data.toRegion);

			//live.push_back(ref);

		}
#ifdef _DEBUG 
		else
			std::cout << obj << " has been deleted now size is " << obj->size << '\n';
#endif
		i--;
	}

	//pushUnused(fromRegion);


	//if (regions[data.toRegion].usedSize > 0) {
	//	regions[data.toRegion].age = age + 1;
	//	//youngRegions.push_back(toRegion);
	//	//sweepRegions.emplace(toRegion);
	//	
	//}
	//else {
	//	//isFailure = true;
	//	//pushUnused(toRegion);
	//	//sweepRegions.erase(toRegion);
	//	//std::cout << "region id: " << data.toRegion << " has been freed\n";
	//}

	
	regions[data.fromRegion].liveNodes.clear();
}

void GarbageCollector::sweep() { 

	//int i = youngRegions.size();
	std::deque<SweepData> dec;
	{
		//int sweepSize = sweepRegions.size();
		for (auto region : sweepRegions) {
			dec.push_back(SweepData(region, -1));
		}
		ThreadPool threads(4);
		for (auto& region : dec) {
			//int region = youngRegions.front();
			//youngRegions.pop_front();
			//SweepData* ptr = region;
			//region.toRegion = 2000;

			threads.EnqueueJob([this, &region]() {
				region.toRegion = popUnused();
				this->sweep2(region);
				});
			//sweep2(region, popUnused());

			//i--;
		}
	}

	for (auto region : dec) {
		pushUnused(region.fromRegion);
		if(regions[region.toRegion].usedSize <= 0)
			pushUnused(region.toRegion);
		else {
			//isFailure = true;
			pushUnused(region.toRegion);
			sweepRegions.erase(region.toRegion);
			std::cout << "region id: " << regions[region.toRegion].usedSize << " live\n";
		}
	}

	match.clear();
	referenceMatch.clear();
	//sweepRegions.clear();
	/*for (auto m : refs) {
		m->ptr = lv[m->ptr];
	}*/
}

void* GarbageCollector::Allocate(size_t _size) {
	int size = _size;
	if (size < DEFAULT_PADDING) size = DEFAULT_PADDING;

	/*if (onGC){
		if (!onMarking) {
			sweep();
			onGC = false;
		}
	}*/

	if ((regions[eden].usedSize + ACTUAL_SIZEOF(size)) >= MAX_REGION_CAPACITY) {
		startGC();
		//sweepRegions.clear();
	}

	auto v = reinterpret_cast<AllocObj*>(regions[eden].memory + (regions[eden].usedSize));
	v->size = ACTUAL_SIZEOF(size);
	v->state = EGCState::WHITE;
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

	//for (auto m : match) {
		//if (m.count(GET_OBJ(exAddr))) {
	auto& vlist = match[GET_OBJ(exAddr)];
	for (auto v : vlist) *v = GET_OBJ(newAddr);

	auto& refList = referenceMatch[GET_OBJ(exAddr)];
	for (auto v : refList) v->ptr = GET_OBJ(newAddr);
		//}
	//}
	
	//lv.emplace(GET_OBJ(exAddr), GET_OBJ(newAddr));

	//std::cout << "aged " <<  (int)(((AllocObj*)newAddr)->age) << '\n';

	return newAddr;
}
