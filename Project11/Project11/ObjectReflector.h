#pragma once
#include <string>
#include <unordered_map>
#include <tuple>
#include "MethodReflector.h"

#define BIT(x) (1 << x)

enum EMatch : unsigned char {
	kNone = 0x0,
	kSame = BIT(1),
	kIsAChildOf = BIT(2),
	kIsASuperOf = BIT(3),
	kSuperOf = BIT(4),
	kChildOf = BIT(5)
};


class ObjectReflector {
	size_t hash;
	ObjectReflector* super;
public:
	std::unordered_map<std::string, MethodReflector*> methods;
	std::string name;

	unsigned int size;
	unsigned int N;


	static unsigned char reflation[20][20];
	static int refl_index;
	///*template<typename Tx>
	//reflectObject(Tx x) {
	//	hash = typeid(Tx).hash_code();
	//}*/

	ObjectReflector(void (*init)(ObjectReflector*));
	ObjectReflector(void (*init)(ObjectReflector*), ObjectReflector* _super);


	/*static void registerObject(std::string name, ObjectReflector* refl) {
		objectReflections.insert(std::make_pair(name, refl));
	}*/

	template<typename T>
	void registerMethod(std::string str, T method) {
		MethodConverter<T> func;
		methods.insert(std::make_pair(str, new MethodReflector(func.convert(method))));
	}

	void registerFunction(std::string str, MethodReflector* method)
	{
		methods.insert(std::make_pair(str, method));
	}

	inline bool isA(ObjectReflector* _parent) const {
		return reflation[N][_parent->N] & (EMatch::kIsAChildOf | EMatch::kIsASuperOf | EMatch::kSame);
	}

	inline bool isSame(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kSame);
	}

	inline bool isSuperOf(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kSuperOf | EMatch::kSame);
	}

	inline bool isChildOf(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kChildOf);
	}

	inline bool isAChildOf(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kIsAChildOf | EMatch::kSame);
	}

	inline bool isASuperOf(ObjectReflector* _other) const {
		return reflation[N][_other->N] & (EMatch::kIsASuperOf);
	}
	/*template<typename T>
	void registerProperty(std::string str, T method) {
		rep<T> func;
		methods.insert(std::make_pair(str, new reflectMethod(func.convert(method))));
	}*/
};

class TypeManager {
public:
	static std::unordered_map<std::string, class ObjectReflector*> objectReflections;
};
