#pragma once
#include "ObjectReflector.h"
#include <deque>
#include <array>
#include <unordered_set>
#include <atomic>
#include <mutex>
#define MAX_OBJECT_SIZE 25600000
#define MAX_REGION_CAPACITY (MAX_OBJECT_SIZE / 20)
#define GC_OBJECT_SIZE (MAX_OBJECT_SIZE - 800)
#define DEFAULT_PADDING 8

enum class EGCState : unsigned char {
	UNMARKED,
	MARKED,
	DEAD,
	WHITE,
	GRAY,
	BLACK
};

enum class EGCColor : unsigned char {
	WHITE,
	GRAY,
	BLACK
};

class AllocObj {
public:
	EGCState state;
	char age;
	unsigned int size;
	unsigned short regionID;

	ObjectReflector* reflector;
};

class SweepCell {
	int region;
};

#define ACTUAL_SIZEOF(size) (size + sizeof(AllocObj))
#define GET_TAG(x) reinterpret_cast<AllocObj*>(((char*)x) - sizeof(AllocObj))
#define GET_OBJ(x) reinterpret_cast<void*>(((char*)x) + sizeof(AllocObj))
#define GET_REFLECTOR(x) GET_TAG(x)->reflector




template<typename T>
class QPtr {
	T* ptr;

};

class Region {
public:
	char* memory;
	int usedSize;
	int age;
	//void** liveNodes;
	void** liveNodes;
	std::atomic<int> liveNodeSize;

	inline void pushLive(void* _live) {
		liveNodes[liveNodeSize.load()] = _live;
		liveNodeSize.fetch_add(1);
	}

	//std::vector<void*> grayNodes;
	//bool unused;
	////Region(char* _mem) : memory(_mem), usedSize(0) {
	////	//unused = true;
	////	age = 0;
	////}
};



class GarbageCollector {
private:
	GarbageCollector();
	GarbageCollector(const GarbageCollector& ref) {}
	GarbageCollector& operator=(const GarbageCollector& ref) {}
	~GarbageCollector() {
		free(memoryHanlde);
	}
	struct SweepData {
		int fromRegion;
		int toRegion;
		SweepData(int from, int to) : fromRegion(from), toRegion(to)  {

		}
	};
	/// WARNING: DON'T USE THIS FOR ALLOCATING MEMORY 
	char* memoryHanlde = new char[MAX_REGION_CAPACITY * 20];
public:/*
	char* oldGeneration = new char[MAX_OBJECT_SIZE];
	char* surviorGeneration = new char[MAX_OBJECT_SIZE];*/

	// INFO: regions are splited memoryHandle usually uses when allocating memory referencing liveNodes
	std::array<Region, 20> regions;
	
	// WARNING: NEVER USE THIS FOR SEARCHING NON-ALLOCATED REGIONS USE 'popUnused()' instead
	std::deque<int> unusedRegions;
	//std::deque<int> markedRegions;
	//size_t allocatedMemory = 0;
	//std::unordered_map<int, void**> liveNodes;
	
	// INFO: current eden's region id
	int eden;
	
	// (NOT IMPLMENTED) INFO: current survivor's region id
	int survivor;
	

	// INFO: this re-matching allocated objects from old address to new address only uses while compacting memory
	std::unordered_map<void*, std::vector<void**>> match;
	// INFO: this re-matching allocated objects in reference list from old address to new address only uses while compacting memory
	std::unordered_map<void*, std::vector<GCPointer*>> referenceMatch;


	std::deque<int> youngRegions;
	std::unordered_set<int> sweepRegions;
	std::list<GCPointer*> refs;
	std::deque<void*> gray;
	
	//std::deque<void*> live;
	//std::list<void*> black;
	//std::list<void*> live;
	//std::thread markingThread;

	bool onGC = false;
	bool onMarking = false;
	void mark();
	void markRef(void* _this, std::mutex& m);
	//void compactRef(void* _this);
	void sweep();
	void compact();
	void sweep2(SweepData& data);
	void grayOut();
	void registerGray(GCPointer* val);

	void startGC();

	void pushUnused(int region);
	void mainMark();
	int popUnused();


	void* Allocate(size_t size);
	void* move(AllocObj* tag, int toRegion);

	inline void* currentRegionAddress(int region) {
		return (regions[region].memory + regions[region].usedSize);
	}

	inline void pushLive(void* live) {
		int rid = GET_TAG(live)->regionID;
		regions[rid].pushLive(live);
		/*if (!sweepRegions.count(rid)) {
			sweepRegions.emplace(rid);
			youngRegions.push_back(rid);
		}*/

		std::cout << live << "is Alive\n";
	}


	static GarbageCollector* get() {
		static GarbageCollector instance;
		return &instance;
	}
};
//
//template<typename T>
//class GCMember : public GCPointer {
//public:
//	GCMember<T>() {
//		ptr = nullptr;
//	}
//
//	//inline GCMember<T>& operator=(const GCMember<T>& other) {
//	//	ptr = reinterpret_cast<void*>(other);
//	//	if (GarbageCollector::get()->onMarking) {
//	//		if (!remark) {
//	//			GarbageCollector::get()->registerGray(this);
//	//			std::cout << static_cast<int>(GET_TAG(this->get())->state) << '\n';
//	//		}
//	//	}
//	//	return *this;
//	//}
//
//	//inline GCMember<T>& operator=(T* other) {
//	//	ptr = reinterpret_cast<void*>(other);
//	//	if (GarbageCollector::get()->onMarking) {
//	//		if (!remark) {
//	//			GarbageCollector::get()->registerGray(this);
//	//			std::cout << static_cast<int>(GET_TAG(this->get())->state) << '\n';
//	//		}
//	//	}
//	//	return *this;
//	//}
//
//	inline T* operator->() {
//		return reinterpret_cast<T*>(ptr);
//	}
//};

template<typename T>
class GCPtr : public GCPointer {
public:
	GCPtr<T>(T* _ptr) {
		this->ptr = reinterpret_cast<void*>(_ptr);
		GarbageCollector::get()->refs.push_back(this);
	}

	~GCPtr<T>() {
		GarbageCollector::get()->refs.remove(this);
	}
	inline T* operator->() {
		return reinterpret_cast<T*>(ptr);
	}
};

