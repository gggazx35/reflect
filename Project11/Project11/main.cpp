#include "Reflection.h"
#include <iostream>

unsigned char ObjectReflector::reflation[20][20] = {};
int ObjectReflector::refl_index = 0;
std::unordered_map<std::string, ObjectReflector*> TypeManager::objectReflections;

//#include <unordered_map>
//
//#define BIT(x) (1 << x)
//
//enum EMatch : unsigned char {
//	kNone = 0x0,
//	kSame = BIT(1),
//	kIsAChildOf = BIT(2),
//	kIsASuperOf = BIT(3),
//	kSuperOf = BIT(4),
//	kChildOf = BIT(5)
//};
//
//template<typename T>
//struct rep;
//
//template<typename R, typename T, typename... TARGS>
//struct rep<R(T::*)(TARGS...)> {
//	using methodType = R(T::*)(TARGS...);
//	auto convert(methodType func_) {
//		static methodType doubla = 0;
//		doubla = func_;
//		R(*deepshit)(T*, TARGS...) = [](T* self, TARGS... args) {
//			(self->*doubla)(args...);
//			};
//		return deepshit;
//	}
//
//};
//
////////??? index_sequence? ??? ?? ?? ??? ??
//////template<typename TP, size_t ... I>
//////void print_tuple_imp(const TP& tp, const std::index_sequence<I...>&)
//////{
//////	int x[] = { typeid(std::get<I...>(tp)).hash_code() };
//////
//////	for (auto& n : x)
//////		std::cout << n << ", ";
//////}
//////
//////template<typename TP>
//////void print_tuple(const TP& tp)
//////{
//////	// ?? ???? ??? index_sequence? ???? print_tuple_imp? ??
//////	print_tuple_imp(tp, std::make_index_sequence<std::tuple_size<TP>::value>());
//////}
//
//
//// practice (expression)... = repeat expression
//
//class reflectProperty {
//
//};
//class ObjectReflector;
//struct TypeManager {
//	static std::unordered_map<std::string, ObjectReflector*> objectReflections;
//	/*static char** map;
//
//	static void mapp() {
//		const auto size = objectReflections.size();
//
//		map = new char*[size];
//		for (int i = 0; i < size; ++i) {
//			auto v = new char[size];
//			int j = 0;
//
//			for (auto& item : objectReflections) {
//				v[j] = item.second->isChildOf();
//				j++;
//			}
//
//		}
//	}*/
//	static void registerObject(std::string name, ObjectReflector* refl) {
//		objectReflections.insert(std::make_pair(name, refl));
//	}
//};
//std::unordered_map<std::string, ObjectReflector*> TypeManager::objectReflections;
//
//class MethodReflector {
//public:
//	void* ptr;
//	size_t hash;
//	std::vector<size_t> parameters;
//
//	template<typename R, typename... TARGS>
//	MethodReflector(R(*func)(TARGS...)) {
//		ptr = reinterpret_cast<void*>(func);
//		hash = typeid(func).hash_code();
//		//for (int i = 0; i < sizeof...(TARGS...); i++) {
//		parameters = { typeid(TARGS).hash_code()... };
//
//		/*size_t x[] = { typeid(TARGS).hash_code()... };
//		parameters = x;
//		parameters = (size_t*)malloc(sizeof(x));
//		memcpy(parameters, x, sizeof(x));*/
//	}
//
//	bool match() {
//
//	}
//};
//
//class ObjectReflector {
//	size_t hash;
//	ObjectReflector* super;
//public:
//	std::unordered_map<std::string, MethodReflector*> methods;
//	std::string name;
//
//	unsigned int size;
//	unsigned int N;
//
//	static unsigned char reflation[20][20];
//	static int refl_index;
//	///*template<typename Tx>
//	//reflectObject(Tx x) {
//	//	hash = typeid(Tx).hash_code();
//	//}*/
//
//	ObjectReflector(void (*init)(ObjectReflector*)) {
//		init(this);
//		TypeManager::registerObject(name, this);
//		N = refl_index++;
//		reflation[N][N] = (EMatch::kSame);
//
//		super = nullptr;
//		/*methods = {
//			{ "", new reflectMethod(&reflectMethod::match) }
//		};*/
//	}
//
//	ObjectReflector(void (*init)(ObjectReflector*), ObjectReflector* _super) {
//		methods = _super->methods;
//		init(this);
//		super = _super;
//		N = refl_index++;
//
//		reflation[N][super->N] = (EMatch::kChildOf | EMatch::kIsAChildOf);
//		reflation[N][N] = EMatch::kSame;
//
//		reflation[super->N][N] = (EMatch::kSuperOf | EMatch::kIsASuperOf);
//		auto curr = super->super;
//		while (curr != nullptr) {
//			reflation[N][curr->N] = EMatch::kIsAChildOf;
//			reflation[curr->N][N] = EMatch::kIsASuperOf;
//			curr = curr->super;
//		}
//
//		TypeManager::registerObject(name, this);
//		/*methods = {
//			{ "", new reflectMethod(&reflectMethod::match) }
//		};*/
//	}
//
//	template<typename T>
//	void registerMethod(std::string str, T method) {
//		rep<T> func;
//		methods.insert(std::make_pair(str, new MethodReflector(func.convert(method))));
//	}
//
//	void registerFunction(std::string str, MethodReflector* method)
//	{
//		methods.insert(std::make_pair(str, method));
//	}
//
//	bool isA(ObjectReflector* _parent) {
//		return reflation[N][_parent->N] & (EMatch::kIsAChildOf | EMatch::kIsASuperOf | EMatch::kSame);
//	}
//
//	bool isSame(ObjectReflector* _other) {
//		return reflation[N][_other->N] & (EMatch::kSame);
//	}
//
//	bool isSuperOf(ObjectReflector* _other) {
//		return reflation[N][_other->N] & (EMatch::kSuperOf | EMatch::kSame);
//	}
//
//	bool isChildOf(ObjectReflector* _other) {
//		return reflation[N][_other->N] & (EMatch::kChildOf);
//	}
//
//	bool isAChildOf(ObjectReflector* _other) {
//		return reflation[N][_other->N] & (EMatch::kIsAChildOf | EMatch::kSame);
//	}
//
//	bool isASuperOf(ObjectReflector* _other) {
//		return reflation[N][_other->N] & (EMatch::kIsASuperOf);
//	}
//	/*template<typename T>
//	void registerProperty(std::string str, T method) {
//		rep<T> func;
//		methods.insert(std::make_pair(str, new reflectMethod(func.convert(method))));
//	}*/
//};
//unsigned char ObjectReflector::reflation[20][20] = {};
//int ObjectReflector::refl_index = 0;
//bool** map = nullptr;
//
//struct DefaultResolver
//{
//	template <typename T> static char func(decltype(&T::reflect));
//	template <typename T> static int func(...);
//	template <typename T>
//	struct IsReflected
//	{
//		enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
//	};
//
//	// This version is called if T has a static member named "Reflection":
//	template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
//	static reflectObject* get()
//	{
//		return &T::reflect;
//	}
//
//	// This version is called otherwise:
//	template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
//	static reflectObject* get()
//	{
//		return 0;
//		//return getPrimitiveDescriptor<T>();
//	}
//};
//
//template<class T>
//struct TypeResolver {
//	static ObjectReflector* get() {
//		return &T::reflect;
//	}
//};
//

////
////#define REFLECT_START_SUPER(type) \
////ObjectReflector type::reflect { &type::initTypeDescriptor, TypeResolver<type::super>::get() }; \
////ObjectReflector* type::getReflector() { return &reflect; } \
////void type::initTypeDescriptor(reflectObject* _desc) { \
////using T = type; \
////_desc->name = #type; \
////_desc->size = sizeof(T);
////
////#define REFLECT_START(type) \
////ObjectReflector type::reflect { &type::initTypeDescriptor }; \
////ObjectReflector* type::getReflector() { return &reflect; } \
////void type::initTypeDescriptor(reflectObject* _desc) { \
////using T = type; \
////_desc->name = #type; \
////_desc->size = sizeof(T);
//// 
////
//////static void register_Reflect() { auto obj = new reflectObject(); TypeManager::registerObject(#type, obj);
////#define REFLECT_METHOD(method) _desc->registerMethod(#method, &T::method);
////#define REFLECT_FUNCTION(func) _desc->registerFunction(#func, new MethodReflector(&T::func));
////#define REFLECT_END }
////#define REFLECT  \
////static void initTypeDescriptor(reflectObject* obj); \
////static reflectObject reflect; \
////virtual reflectObject* getReflector(); 
////
////#define REFLECT_PARENT(sup) using super = sup; 

class poss {
public:
	using super = void;
	int truea;
	void one() {
		std::cout << "y cccccc";
	}
private:
	virtual void overrid(int a) {
		std::cout << "base poss\n";
	}
	static void ugh();

public:
	poss() {}
	static poss* New() {
		auto a = (new (poss)());
		return new poss();
	}

	REFLECT
};
REFLECT_START(poss)
	REFLECT_METHOD(overrid)
	REFLECT_FUNCTION(New)
REFLECT_END

class OK : public poss {
public:
	using super = poss;
	void overrid(int a) override {
		std::cout << "overrided OK\n";
	}
	REFLECT
};

REFLECT_START_SUPER(OK)
REFLECT_END

class OKDobule : public OK {
public:
	using super = OK;
	void overrid(int a) override {
		std::cout << "overrided Dobule\n";
	}
	void zep(int jan, OK* other) {
		std::cout << jan << " zaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaap\n";
	}
	REFLECT
};

REFLECT_START_SUPER(OKDobule)
REFLECT_METHOD(zep)
REFLECT_END
//
//template<typename R, typename... TARGS>
//R call(MethodReflector* vPtr, TARGS... args) {
//	using func = R(*)(TARGS...);
//	//assert(typeid(func).hash_code() == vPtr->hash);
//	
//	return (reinterpret_cast<R(*)(TARGS...)>(vPtr->ptr))(args...);
//	
//	//(reinterpret_cast<R(*)(TARGS...)>(vPtr->ptr))(args...);
//}

template<typename T>
bool isCastable(T* _a, ObjectReflector* _b) {
	return _a->getReflector()->isAChildOf(_b);
}


template<typename T, typename E>
bool isCastable(T* _a, E* _b) {
	return _a->getReflector()->isAChildOf(_b->getReflector());
}

template<typename T, typename E>
E* cast(T* out, E* other) {
	return isCastable(out, other) ? reinterpret_cast<E*>(out) : nullptr;
}

template<typename E, typename T>
E* cast(T* out) {
	return (isCastable(out, TypeResolver<E>::get()) ? reinterpret_cast<E*>(out) : nullptr);
}

typedef poss stupid;
int main() {
	//rep<decltype(&poss::overrid)> sans;
	//auto cons = &poss::poss;

	//reflectMethod r(sans.convert(&poss::overrid));
	//std::cout << "\n\ni have no" << r.parameters[1] << '\n';

	poss* newPoss = new OK();
	
	call<void>(TypeManager::objectReflections["poss"]->methods["overrid"], newPoss, 20);
	std::cout << isCastable(newPoss, TypeManager::objectReflections["OKDobule"]) << '\n';

	poss* nextLevel = new OKDobule();
	std::cout << isCastable(nextLevel, TypeManager::objectReflections["OKDobule"]) << '\n';

	if (isCastable(nextLevel, TypeManager::objectReflections["OKDobule"])) {
		auto z = cast<OKDobule>(nextLevel);
		z->overrid(20);
		auto f = TypeManager::objectReflections["OKDobule"]->methods["zep"];
		call<void>(TypeManager::objectReflections["OKDobule"]->methods["zep"], z, 300, (OK*)nullptr);
	}

	//poss::register_Reflect();

	/*call<void>(TypeManager::objectReflections["poss"]->methods["overrid"], newPoss, 20);
	poss* nextLevel = new OKDobule();
	std::cout << isCastable(nextLevel, TypeManager::objectReflections["OKDobule"]) << '\n';
	
	if (isCastable(nextLevel, TypeManager::objectReflections["OKDobule"])) {
		auto z = cast<OKDobule>(nextLevel);
		z->overrid(20);
		auto f = TypeManager::objectReflections["OKDobule"]->methods["zep"];
		call<void>(TypeManager::objectReflections["OKDobule"]->methods["zep"], z, 300, (OK*)nullptr);
	}*/
	
	//std::cout << TypeResolver<poss>::get()->name << '\n';
	//std::cout << TypeResolver<OK>::get()->isChildOf(TypeResolver<poss>::get()) << '\n';
	//std::cout << TypeResolver<OK>::get()->isSuperOf(TypeResolver<poss>::get()) << '\n';
	//std::cout << TypeResolver<OK>::get()->isSame(TypeResolver<OK>::get()) << '\n';
	//r(newPoss);
	//typeid(poss).hash_code();
	//call<void>(r, newPoss);
	//std::vector<int> ap(20, 20);
	//auto e = new poss;
	//int (*thig)(poss*, int io2) = [](poss* self, int io2) {
		//self->one();

	//typedef int r;
	return 0;
}
