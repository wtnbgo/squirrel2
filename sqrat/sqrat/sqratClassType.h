//
// SqratClassType: Type Translators
//

//
// Copyright (c) 2009 Brandon Jones
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//	1. The origin of this software must not be misrepresented; you must not
//	claim that you wrote the original software. If you use this software
//	in a product, an acknowledgment in the product documentation would be
//	appreciated but is not required.
//
//	2. Altered source versions must be plainly marked as such, and must not be
//	misrepresented as being the original software.
//
//	3. This notice may not be removed or altered from any source
//	distribution.
//

#if !defined(_SCRAT_CLASSTYPE_H_)
#define _SCRAT_CLASSTYPE_H_

#include <squirrel.h>

namespace Sqrat {

	//
	// ClassType
	//

	template<class C>
	struct ClassType {
		// Get the Squirrel Object for this Class
		static inline HSQOBJECT& ClassObject() {
			static HSQOBJECT classObj; 
			return classObj;
		}

		// Get the Get Table for this Class
		static inline HSQOBJECT& GetTable() {
			static HSQOBJECT getTable; 
			return getTable;
		}

		// Get the Set Table for this Class
		static inline HSQOBJECT& SetTable() {
			static HSQOBJECT setTable; 
			return setTable;
		}

		// Get the Copy Function for this Class
		typedef SQInteger (*COPYFUNC)(HSQUIRRELVM, SQInteger, const C*);

		static inline COPYFUNC& CopyFunc() {
			static COPYFUNC copyFunc; 
			return copyFunc;
		}

		typedef SQInteger (*INITFUNC)(HSQUIRRELVM, SQInteger, C*);
		
		static inline INITFUNC& InitFunc() {
			static INITFUNC initFunc; 
			return initFunc;
		}
		
		static inline bool& Initialized() {
			static bool initialized = false;
			return initialized;
		}

		static void PushInstance(HSQUIRRELVM vm, const C* ptr) {
			sq_pushobject(vm, ClassObject());
			if (SQ_SUCCEEDED(sq_createinstance(vm, -1))) {
				sq_remove(vm, -2);
				sq_setinstanceup(vm, -1, ptr);
			} else {
				sq_pop(vm, 1);
				sq_pushnull(vm);
			}
		}

		static void PushInstanceInit(HSQUIRRELVM vm, C* ptr) {
			sq_pushobject(vm, ClassObject());
			if (SQ_SUCCEEDED(sq_createinstance(vm, -1))) {
				sq_remove(vm, -2);
				InitFunc()(vm, -1, ptr);
			} else {
				delete ptr;
				sq_pop(vm, 1);
				sq_pushnull(vm);
			}
		}
		
		static void PushInstanceCopy(HSQUIRRELVM vm, C& value) {
			sq_pushobject(vm, ClassObject());
			if (SQ_SUCCEEDED(sq_createinstance(vm, -1))) {
				sq_remove(vm, -2);
				CopyFunc()(vm, -1, &value);
			} else {
				sq_pop(vm, 1);
				sq_pushnull(vm);
			}
		}

		static C* GetInstance(HSQUIRRELVM vm, SQInteger idx) {
			SQUserPointer typetag = NULL;
			sq_getobjtypetag(&ClassObject(),&typetag);
			SQUserPointer up;
			if (SQ_SUCCEEDED(sq_getinstanceup(vm, idx, &up, typetag))) {
				return (C*)up;
			}
			return NULL;
		}
	};
	
}

#endif
