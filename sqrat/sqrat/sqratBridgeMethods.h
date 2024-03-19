//
// SqratBridgeMethods: Member Methods
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

#if !defined(_SCRAT_BRIDGE_METHODS_H_)
#define _SCRAT_BRIDGE_METHODS_H_

#include <squirrel.h>
#include "sqratTypes.h"

#define FUNCBEGIN SQUserPointer up;if (SQ_SUCCEEDED(sq_getinstanceup(vm, 1, &up, NULL)) && up) { C* ptr = static_cast<C*>(*(SrcT*)up); if (SQ_SUCCEEDED(sq_getuserdata(vm, -1, &up, NULL)) && up) { M method = *(M*)up
#define FUNCEND }}return ERROR_BADINSTANCE(vm)

namespace Sqrat {

	//
	// Squirrel bridge Functions
	// SrcT に operator C* () が実装されてる想定で
	// R (*method)(C*, args...) のメソッドを登録する
	//

	template <class SrcT, class C, class R>
	class SqBridge {
	public:	

		// Arg Count 0
		static SQInteger Func0(HSQUIRRELVM vm) {
			typedef R (*M)(C*);
			FUNCBEGIN;
			R ret = (*method)(ptr);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		// Arg Count 1
		template <class A1>
		static SQInteger Func1(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		// Arg Count 2
		template <class A1, class A2>
		static SQInteger Func2(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1, A2);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
				
		// Arg Count 3
		template <class A1, class A2, class A3>
		static SQInteger Func3(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1, A2, A3);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		// Arg Count 4
		template <class A1, class A2, class A3, class A4>
		static SQInteger Func4(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1, A2, A3, A4);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		// Arg Count 5
		template <class A1, class A2, class A3, class A4, class A5>
		static SQInteger Func5(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1, A2, A3, A4, A5);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		// Arg Count 6
		template <class A1, class A2, class A3, class A4, class A5, class A6>
		static SQInteger Func6(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1, A2, A3, A4, A5, A6);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value,
				Var<A6>(vm, 7).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		// Arg Count 7
		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
		static SQInteger Func7(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1, A2, A3, A4, A5, A6, A7);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value,
				Var<A6>(vm, 7).value,
				Var<A7>(vm, 8).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}

		// Arg Count 8
		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
		static SQInteger Func8(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1, A2, A3, A4, A5, A6, A7, A8);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value,
				Var<A6>(vm, 7).value,
				Var<A7>(vm, 8).value,
				Var<A8>(vm, 9).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}

		// Arg Count 9
		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
		static SQInteger Func9(HSQUIRRELVM vm) {
			typedef R (*M)(C*,A1, A2, A3, A4, A5, A6, A7, A8, A9);
			FUNCBEGIN;
			R ret = (*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value,
				Var<A6>(vm, 7).value,
				Var<A7>(vm, 8).value,
				Var<A8>(vm, 9).value,
				Var<A9>(vm, 10).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
	};

	//
	// void return specialization
	//

	template <class SrcT, class C>
	class SqBridge<SrcT, C, void> {
	public:		
		// Arg Count 0
		static SQInteger Func0(HSQUIRRELVM vm) {
			typedef void (*M)(C*);
			FUNCBEGIN;
			(*method)(ptr);
			return 0;
			FUNCEND;
		}

		// Arg Count 1
		template <class A1>
		static SQInteger Func1(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 2
		template <class A1, class A2>
		static SQInteger Func2(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1, A2);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 3
		template <class A1, class A2, class A3>
		static SQInteger Func3(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1, A2, A3);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 4
		template <class A1, class A2, class A3, class A4>
		static SQInteger Func4(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1, A2, A3, A4);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 5
		template <class A1, class A2, class A3, class A4, class A5>
		static SQInteger Func5(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1, A2, A3, A4, A5);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 6
		template <class A1, class A2, class A3, class A4, class A5, class A6>
		static SQInteger Func6(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1, A2, A3, A4, A5, A6);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value,
				Var<A6>(vm, 7).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 7
		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
		static SQInteger Func7(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1, A2, A3, A4, A5, A6, A7);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value,
				Var<A6>(vm, 7).value,
				Var<A7>(vm, 8).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 8
		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
		static SQInteger Func8(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1, A2, A3, A4, A5, A6, A7, A8);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value,
				Var<A6>(vm, 7).value,
				Var<A7>(vm, 8).value,
				Var<A8>(vm, 9).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 9
		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
		static SQInteger Func9(HSQUIRRELVM vm) {
			typedef void (*M)(C*,A1, A2, A3, A4, A5, A6, A7, A8, A9);
			FUNCBEGIN;
			(*method)(ptr, 
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value,
				Var<A6>(vm, 7).value,
				Var<A7>(vm, 8).value,
				Var<A8>(vm, 9).value,
				Var<A9>(vm, 10).value
				);
			return 0;
			FUNCEND;
		}
	};


	//
	// Member Function Resolvers
	//

	// Arg Count 0
	template <class SrcT, class C, class R>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*)) {
		return &SqBridge<SrcT, C, R>::Func0;
	}

	// Arg Count 1
	template <class SrcT, class C, class R, class A1>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1)) {
		return &SqBridge<SrcT, C, R>::template Func1<A1>;
	}

	// Arg Count 2
	template <class SrcT, class C, class R, class A1, class A2>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1, A2)) {
		return &SqBridge<SrcT, C, R>::template Func2<A1, A2>;
	}

	// Arg Count 3
	template <class SrcT, class C, class R, class A1, class A2, class A3>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1, A2, A3)) {
		return &SqBridge<SrcT, C, R>::template Func3<A1, A2, A3>;
	}

	// Arg Count 4
	template <class SrcT, class C, class R, class A1, class A2, class A3, class A4>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1, A2, A3, A4)) {
		return &SqBridge<SrcT, C, R>::template Func4<A1, A2, A3, A4>;
	}

	// Arg Count 5
	template <class SrcT, class C, class R, class A1, class A2, class A3, class A4, class A5>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1, A2, A3, A4, A5)) {
		return &SqBridge<SrcT, C, R>::template Func5<A1, A2, A3, A4, A5>;
	}

	// Arg Count 6
	template <class SrcT, class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1, A2, A3, A4, A5, A6)) {
		return &SqBridge<SrcT, C, R>::template Func6<A1, A2, A3, A4, A5, A6>;
	}

	// Arg Count 7
	template <class SrcT, class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1, A2, A3, A4, A5, A6, A7)) {
		return &SqBridge<SrcT, C, R>::template Func7<A1, A2, A3, A4, A5, A6, A7>;
	}

	// Arg Count 8
	template <class SrcT, class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1, A2, A3, A4, A5, A6, A7, A8)) {
		return &SqBridge<SrcT, C, R>::template Func8<A1, A2, A3, A4, A5, A6, A7, A8>;
	}

	// Arg Count 9
	template <class SrcT, class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	inline SQFUNCTION SqBridgeFunc(R (*method)(C*, A1, A2, A3, A4, A5, A6, A7, A8, A9)) {
		return &SqBridge<SrcT, C, R>::template Func9<A1, A2, A3, A4, A5, A6, A7, A8, A9>;
	}
}

#undef FUNCBEGIN
#undef FUNCEND

#endif
