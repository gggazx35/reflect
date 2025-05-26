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
		regions[i].liveNodes = new void*[MAX_REGION_CAPACITY / 16];
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
			registerGray(ref);
			//match[ref->ptr].push_back(&ref->ptr);
			referenceMatch[ref->ptr].push_back(ref);
		}
	}
	//eden = popUnused();
}

void GarbageCollector::markRef(void* _this, std::mutex& m) {
	void* self = _this;

	GET_TAG(self)->state = EGCState::BLACK;
	auto refs = GET_REFLECTOR(self);

	auto pointers = refs->pointers;

	for (auto ref : pointers) {
		void* val = *ref->As<void*>(self);
		void** ptr = ref->As<void*>(self);
		if (val) {
			if(GET_TAG(val)->state == EGCState::WHITE) markRef(val, m);
			m.lock();
			match[val].push_back(ptr);
			m.unlock();
		}
	}
	pushLive(self);
	//gray.pop_front();
}

////void GarbageCollector::compact() {
////	for (auto ref : refs) {
////		if (ref->ptr) compactRef(ref->ptr);
////	}
////}

void GarbageCollector::grayOut()
{
	//int graySize = _graySize;
	//auto i = gray.size();{
	//onMarking = true;
	{
		ThreadPool threads(4);
		int graySize = gray.size();
		//grayStart = graySize;
		std::mutex m;
		for (int i = 0; i < graySize; i++) {
			void* curr = gray[i];
			//if (curr->remark == true) {
			threads.EnqueueJob([this, curr, &m]() { this->markRef(curr, m); });
				//gray.pop_front();
			//}
		}
		printf("im not\n");
	}

	onMarking = false;
	gray.clear();
	//if (gray.empty()) sweep2();
	//gray.clear();
}

void GarbageCollector::registerGray(GCPointer* val)
{
	//EGCState state = GET_TAG(val)->state;
	//int region = GET_TAG(val)->regionID;
	//if (state == EGCState::WHITE) {
	gray.push_back(val->get());
	GET_TAG(val->get())->state = EGCState::GRAY;
	//}
}

void GarbageCollector::startGC()
{

	time_t start, finish;
	//double duration;

	start = clock();
	eden = popUnused();

	mark();
	
	grayOut();

	sweep();

	finish = clock();

	std::cout << "gc taks " << finish - start << std::endl;
}

void GarbageCollector::pushUnused(int region)
{
	sweepRegions.erase(region);
	unusedRegions.push_back(region);
	regions[region].age = 0;
	regions[region].usedSize = 0;
	regions[region].liveNodeSize.store(0);
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

void GarbageCollector::sweep2(SweepData& data) {
	auto liveList = regions[data.fromRegion].liveNodes;
	int i = regions[data.fromRegion].liveNodeSize.load();
	int age = regions[data.fromRegion].age;
	i--;
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

	regions[data.toRegion].age = age + 1;
	regions[data.fromRegion].liveNodeSize.store(0);
}

void GarbageCollector::sweep() { 

	//int i = youngRegions.size();
	std::deque<SweepData> dec;
	{
		//int sweepSize = sweepRegions.size();
		for (auto region : sweepRegions) {
			if(region != eden)
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
			//sweepRegions.erase(region.toRegion);
			
			std::cout << "region id: " << regions[region.toRegion].age << " live\n";
			//if (regions[region.toRegion].age > 10) abort();
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

	auto& vlist = match[GET_OBJ(exAddr)];
	for (auto v : vlist) *v = GET_OBJ(newAddr);

	auto& refList = referenceMatch[GET_OBJ(exAddr)];
	for (auto v : refList) v->ptr = GET_OBJ(newAddr);

	memcpy(newAddr, exAddr, size);
	regions[toRegion].usedSize += size;

	return newAddr;
}
