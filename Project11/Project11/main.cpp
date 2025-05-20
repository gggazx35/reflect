#include <iostream>
#include <vector>
#include <assert.h>
#include <tuple>
#include <memory>
#include <unordered_map>

template<typename T>
struct rep;

template<typename R, typename T, typename... TARGS>
struct rep<R(T::*)(TARGS...)> {
	using methodType = R(T::*)(TARGS...);
	auto convert(methodType func_) {
		static methodType doubla = 0;
		doubla = func_;
		R(*deepshit)(T*, TARGS...) = [](T* self, TARGS... args) {
			(self->*doubla)(args...);
			};
		return deepshit;
	}

};

//////??? index_sequence? ??? ?? ?? ??? ??
////template<typename TP, size_t ... I>
////void print_tuple_imp(const TP& tp, const std::index_sequence<I...>&)
////{
////	int x[] = { typeid(std::get<I...>(tp)).hash_code() };
////
////	for (auto& n : x)
////		std::cout << n << ", ";
////}
////
////template<typename TP>
////void print_tuple(const TP& tp)
////{
////	// ?? ???? ??? index_sequence? ???? print_tuple_imp? ??
////	print_tuple_imp(tp, std::make_index_sequence<std::tuple_size<TP>::value>());
////}


// practice (expression)... = repeat expression

class reflectProperty {

};
class reflectObject;
struct TypeManager {
	static std::unordered_map<std::string, reflectObject*> objectReflections;
	/*static char** map;

	static void mapp() {
		const auto size = objectReflections.size();

		map = new char*[size];
		for (int i = 0; i < size; ++i) {
			auto v = new char[size];
			int j = 0;

			for (auto& item : objectReflections) {
				v[j] = item.second->isChildOf();
				j++;
			}

		}
	}*/
	static void registerObject(std::string name, reflectObject* refl) {
		objectReflections.insert(std::make_pair(name, refl));
	}
};
std::unordered_map<std::string, reflectObject*> TypeManager::objectReflections;

class reflectMethod {
public:
	void* ptr;
	size_t hash;
	std::vector<size_t> parameters;

	template<typename R, typename... TARGS>
	reflectMethod(R(*func)(TARGS...)) {
		ptr = reinterpret_cast<void*>(func);
		hash = typeid(func).hash_code();
		//for (int i = 0; i < sizeof...(TARGS...); i++) {
		parameters = { typeid(TARGS).hash_code()... };

		/*size_t x[] = { typeid(TARGS).hash_code()... };
		parameters = x;
		parameters = (size_t*)malloc(sizeof(x));
		memcpy(parameters, x, sizeof(x));*/
	}

	bool match() {

	}
};

class reflectObject {
	size_t hash;
	size_t super;
public:
	std::unordered_map<std::string, reflectMethod*> methods;
	unsigned int size;
	std::string name;
	///*template<typename Tx>
	//reflectObject(Tx x) {
	//	hash = typeid(Tx).hash_code();
	//}*/

	reflectObject(void (*init)(reflectObject*)) {
		init(this);
		TypeManager::registerObject(name, this);
		/*methods = {
			{ "", new reflectMethod(&reflectMethod::match) }
		};*/
	}

	template<typename T>
	void init() {
		hash = typeid(T).hash_code();
		super = typeid(T::super).hash_code();
	}

	template<typename T>
	void registerMethod(std::string str, T method) {
		rep<T> func;
		methods.insert(std::make_pair(str, new reflectMethod(func.convert(method))));
	}

	void registerFunction(std::string str, reflectMethod* method)
	{
		methods.insert(std::make_pair(str, method));
	}

	bool isChildOf(reflectObject* _parent) {
		return super == _parent->hash;
	}

	/*template<typename T>
	void registerProperty(std::string str, T method) {
		rep<T> func;
		methods.insert(std::make_pair(str, new reflectMethod(func.convert(method))));
	}*/
};


//bool** map = nullptr;

struct DefaultResolver
{
	template <typename T> static char func(decltype(&T::Reflection));
	template <typename T> static int func(...);
	template <typename T>
	struct IsReflected
	{
		enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
	};

	// This version is called if T has a static member named "Reflection":
	template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
	static reflectObject* get()
	{
		return &T::reflect;
	}

	// This version is called otherwise:
	template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
	static reflectObject* get()
	{
		return getPrimitiveDescriptor<T>();
	}
};

struct TypeResolver {
};




#define REFLECT_START(type) \
reflectObject type::reflect { &type::initTypeDescriptor }; \
void type::initTypeDescriptor(reflectObject* _desc) { \
using T = type; \
_desc->name = #type; \
_desc->size = sizeof(T);
 

//static void register_Reflect() { auto obj = new reflectObject(); TypeManager::registerObject(#type, obj);
#define REFLECT_METHOD(method) _desc->registerMethod(#method, &T::method);
#define REFLECT_FUNCTION(func) _desc->registerFunction(#func, new reflectMethod(&T::func));
#define REFLECT_END }
#define REFLECT  \
static void initTypeDescriptor(reflectObject* obj); \
static reflectObject reflect; 

class poss {
public:
	using super = void;
	int truea;
	void one() {
		std::cout << "you dont havfa capture";
	}
private:
	virtual void overrid(int a) {

	}
	static void ugh();

public:
	poss() {}
	static poss* New() {
		auto a = (new (poss)());
		return new poss();
	}

	REFLECT
	/*REFLECT_START(poss)
		REFLECT_METHOD(overrid)
		REFLECT_FUNCTION(New)
	REFLECT_END*/
};
//reflectObject poss::reflect { &poss::initTypeDescriptor }
REFLECT_START(poss)
	REFLECT_METHOD(overrid)
	REFLECT_FUNCTION(New)
REFLECT_END


template<size_t g>
struct TypeID {

};

class OK : public poss {
public:
	using super = poss;
	void overrid(int a) override {
		std::cout << "overrided\n";
	}
};



template<typename R, typename... TARGS>
R call(reflectMethod* vPtr, TARGS... args) {
	using func = R(*)(TARGS...);
	assert(typeid(func).hash_code() == vPtr->hash);
	
	if constexpr (std::is_void_v<R> == true) (reinterpret_cast<R(*)(TARGS...)>(vPtr->ptr))(args...);
	else return (reinterpret_cast<R(*)(TARGS...)>(vPtr->ptr))(args...);
	
	//(reinterpret_cast<R(*)(TARGS...)>(vPtr->ptr))(args...);
}



typedef poss stupid;
int main() {
	//rep<decltype(&poss::overrid)> sans;
	//auto cons = &poss::poss;

	//reflectMethod r(sans.convert(&poss::overrid));
	//std::cout << "\n\ni have no" << r.parameters[1] << '\n';

	poss* newPoss = new OK();
	poss newxPoss;
	
	poss::register_Reflect();

	call<void>(TypeManager::objectReflections["poss"]->methods["overrid"], newPoss, 20);
	poss* nextLevel = call<poss*>(TypeManager::objectReflections["poss"]->methods["New"]);
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
