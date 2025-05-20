//#include "TypeManager.h"
#include "ObjectReflector.h"


//class TypeManager;

ObjectReflector::ObjectReflector(void (*init)(ObjectReflector*))
{
	 init(this);
	 //registerObject(name, this);
	 TypeManager::objectReflections.insert(std::make_pair(name, this));
	 N = refl_index++;
	 reflation[N][N] = (EMatch::kSame);

	 super = nullptr;
	 /*methods = {
		 { "", new reflectMethod(&reflectMethod::match) }
	 };*/
}

ObjectReflector::ObjectReflector(void (*init)(ObjectReflector*), ObjectReflector* _super) {
	methods = _super->methods;
	properties = _super->properties;


	init(this);
	super = _super;
	N = refl_index++;

	reflation[N][super->N] = (EMatch::kChildOf | EMatch::kIsAChildOf);
	reflation[N][N] = EMatch::kSame;

	reflation[super->N][N] = (EMatch::kSuperOf | EMatch::kIsASuperOf);
	auto curr = super->super;
	while (curr != nullptr) {
		reflation[N][curr->N] = EMatch::kIsAChildOf;
		reflation[curr->N][N] = EMatch::kIsASuperOf;
		curr = curr->super;
	}
	TypeManager::objectReflections.insert(std::make_pair(name, this));
	//registerObject(name, this);
	/*methods = {
		{ "", new reflectMethod(&reflectMethod::match) }
	};*/
}

