//
// SqratAllocator: Custom Class Allocation/Deallocation
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

#if !defined(_SCRAT_ALLOCATOR_H_)
#define _SCRAT_ALLOCATOR_H_

#include <squirrel.h>
#include <string.h>

#include "sqratObject.h"

namespace Sqrat {
	
	//
	// DefaultAllocator
	//

	template<class C>
	class DefaultAllocator {
	public:
		static SQInteger New(HSQUIRRELVM vm) {
			C* instance = new C();
			sq_setinstanceup(vm, 1, instance);
			sq_setreleasehook(vm, 1, &Delete);
			return 0;
		}

		static SQInteger Copy(HSQUIRRELVM vm, SQInteger idx, const C* value) {
			C* instance = new C(*value);
			sq_setinstanceup(vm, idx, instance);
			sq_setreleasehook(vm, idx, &Delete);
			return 0;
		}

		static SQInteger Init(HSQUIRRELVM vm, SQInteger idx, C* instance) {
			sq_setinstanceup(vm, idx, instance);
			sq_setreleasehook(vm, idx, &Delete);
			return 0;
		}

		static SQInteger Delete(SQUserPointer ptr, SQInteger size) {
			C* instance = reinterpret_cast<C*>(ptr);
			delete instance;
			return 0;
		}
	};

	//
	// NoConstructorAllocator
	//

	template<class C>
	class NoConstructor {
	public:
		static SQInteger New(HSQUIRRELVM) { 
			return 0; 
		}
		static SQInteger Copy(HSQUIRRELVM, SQInteger, const C*) {
			return 0;
		}
		static SQInteger Init(HSQUIRRELVM, SQInteger, C*) {
			return 0;
		}
		static SQInteger Delete(SQUserPointer, SQInteger) {
			return 0;
		}
	};

	//
	// CopyOnly
	//

	template<class C>
	class CopyOnly {
	public:
		static SQInteger New(HSQUIRRELVM) { 
			return 0; 
		}
		static SQInteger Copy(HSQUIRRELVM vm, SQInteger idx, const C* value) {
			C* instance = new C(*value);
			sq_setinstanceup(vm, idx, instance);
			sq_setreleasehook(vm, idx, &Delete);
			return 0;
		}
		static SQInteger Init(HSQUIRRELVM vm, SQInteger idx, C* instance) {
			sq_setinstanceup(vm, idx, instance);
			sq_setreleasehook(vm, idx, &Delete);
			return 0;
		}
		static SQInteger Delete(SQUserPointer ptr, SQInteger size) {
			C* instance = reinterpret_cast<C*>(ptr);
			delete instance;
			return 0;
		}
	};


	//
	// NoCopy
	//

	template<class C>
	class NoCopy {
	public:
		static SQInteger New(HSQUIRRELVM vm) {
			C* instance = new C();
			sq_setinstanceup(vm, 1, instance);
			sq_setreleasehook(vm, 1, &Delete);
			return 0;
		}

		static SQInteger Copy(HSQUIRRELVM vm, SQInteger idx, const C* value) {
			return 0;
		}

		static SQInteger Init(HSQUIRRELVM vm, SQInteger idx, C* instance) {
			sq_setinstanceup(vm, idx, instance);
			sq_setreleasehook(vm, idx, &Delete);
			return 0;
		}
		
		static SQInteger Delete(SQUserPointer ptr, SQInteger size) {
			C* instance = reinterpret_cast<C*>(ptr);
			delete instance;
			return 0;
		}
	};

	//
	// factory style
	//

	template <class C>
	class Factory {
	public:
		static SQRESULT New(HSQUIRRELVM vm) {
			C *instance = NULL;
			SQRESULT ret = C::factory(vm, &instance);
			if (SQ_SUCCEEDED(ret)) {
				sq_setinstanceup(vm, 1, instance);
				sq_setreleasehook(vm, 1, &Delete);
				return SQ_OK;
			} else {
				return ret;
			}
		}
		
		static SQInteger Copy(HSQUIRRELVM vm, SQInteger idx, const C* value) {
			return SQ_ERROR;
		}

		static SQInteger Init(HSQUIRRELVM vm, SQInteger idx, C* instance) {
			if (instance) {
				sq_setinstanceup(vm, idx, instance);
				sq_setreleasehook(vm, idx, &Delete);
				return SQ_OK;
			} else {
				return SQ_ERROR;
			}
		}
		
		static SQInteger Delete(SQUserPointer ptr, SQInteger size) {
			C* instance = reinterpret_cast<C*>(ptr);
			delete instance;
			return SQ_OK;
		}
	};

// constructor with arg
template <class C, typename MethodT>
class ArgsConstructor {
 public:
  // No Arg
  static C* Create(HSQUIRRELVM vm, void (C::*)()) { return new C(); }

  // Arg Count 1
  template <class A1>
  static C* Create(HSQUIRRELVM vm, void (C::*)(A1)) {
    return new C(Var<A1>(vm, 2).value);
  }

  // Arg Count 2
  template <class A1, class A2>
  static C* Create(HSQUIRRELVM vm, void (C::*)(A1, A2)) {
    return new C(Var<A1>(vm, 2).value, Var<A2>(vm, 3).value);
  }

  // Arg Count 3
  template <class A1, class A2, class A3>
  static C* Create(HSQUIRRELVM vm, void (C::*)(A1, A2, A3)) {
    return new C(Var<A1>(vm, 2).value, Var<A2>(vm, 3).value,
                 Var<A3>(vm, 4).value);
  }

  static SQInteger New(HSQUIRRELVM vm) {
    MethodT ptr = nullptr;
    C* instance = Create(vm, ptr);
    if (instance) {
      sq_setinstanceup(vm, 1, instance);
      sq_setreleasehook(vm, 1, &Delete);
      return SQ_OK;
    } else {
      return SQ_ERROR;
    }
  }

  static SQInteger Copy(HSQUIRRELVM vm, SQInteger idx, const C* value) {
    return SQ_ERROR;
  }

  static SQInteger Init(HSQUIRRELVM vm, SQInteger idx, C* instance) {
    sq_setinstanceup(vm, idx, instance);
    sq_setreleasehook(vm, idx, &Delete);
    return SQ_OK;
  }

  static SQInteger Delete(SQUserPointer ptr, SQInteger size) {
    C* instance = reinterpret_cast<C*>(ptr);
    delete instance;
    return SQ_OK;
  }
};


}

#endif
