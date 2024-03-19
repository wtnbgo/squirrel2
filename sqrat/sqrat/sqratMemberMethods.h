//
// SqratMemberMethods: Member Methods
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

#if !defined(_SCRAT_MEMBER_METHODS_H_)
#define _SCRAT_MEMBER_METHODS_H_

#include <squirrel.h>
#include "sqratTypes.h"

#define FUNCBEGIN SQUserPointer up;if (SQ_SUCCEEDED(sq_getinstanceup(vm, 1, &up, NULL)) && up) { C* ptr = (C*)up; if (SQ_SUCCEEDED(sq_getuserdata(vm, -1, &up, NULL)) && up) { M method = *(M*)up
#define FUNCEND }}return ERROR_BADINSTANCE(vm)

namespace Sqrat {

	static inline SQRESULT ERROR_BADINSTANCE(HSQUIRRELVM vm) {
		return sq_throwerror(vm, _SC("bad instance"));
	}

	//
	// Squirrel Global Functions
	//

	template <class C, class R>
	class SqMember {
	public:	

		// Arg Count 0
		static SQInteger Func0(HSQUIRRELVM vm) {
			typedef R (C::*M)();
			FUNCBEGIN;
			R ret = (ptr->*method)();
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		static SQInteger Func0C(HSQUIRRELVM vm) {
			typedef R (C::*M)() const;
			FUNCBEGIN;
			R ret = (ptr->*method)();
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}

		// Arg Count 1
		template <class A1>
		static SQInteger Func1(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1);
			FUNCBEGIN;
			R ret = (ptr->*method)(
				Var<A1>(vm, 2).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		template <class A1>
		static SQInteger Func1C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1) const;
			FUNCBEGIN;
			R ret = (ptr->*method)(
				Var<A1>(vm, 2).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}

		// Arg Count 2
		template <class A1, class A2>
		static SQInteger Func2(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2);
			FUNCBEGIN;
			R ret = (ptr->*method)(
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		template <class A1, class A2>
		static SQInteger Func2C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2) const;
			FUNCBEGIN;
			R ret = (ptr->*method)(
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
			typedef R (C::*M)(A1, A2, A3);
			FUNCBEGIN;
			R ret = (ptr->*method)(
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		template <class A1, class A2, class A3>
		static SQInteger Func3C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2, A3) const;
			FUNCBEGIN;
			R ret = (ptr->*method)(
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
			typedef R (C::*M)(A1, A2, A3, A4);
			FUNCBEGIN;
			R ret = (ptr->*method)(
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value
				);
			PushVar(vm, ret);
			return 1;
			FUNCEND;
		}
		
		template <class A1, class A2, class A3, class A4>
		static SQInteger Func4C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2, A3, A4) const;
			FUNCBEGIN;
			R ret = (ptr->*method)(
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
			typedef R (C::*M)(A1, A2, A3, A4, A5);
			FUNCBEGIN;
			R ret = (ptr->*method)(
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
		
		template <class A1, class A2, class A3, class A4, class A5>
		static SQInteger Func5C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2, A3, A4, A5) const;
			FUNCBEGIN;
			R ret = (ptr->*method)(
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
			typedef R (C::*M)(A1, A2, A3, A4, A5, A6);
			FUNCBEGIN;
			R ret = (ptr->*method)(
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
		
		template <class A1, class A2, class A3, class A4, class A5, class A6>
		static SQInteger Func6C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2, A3, A4, A5, A6) const;
			FUNCBEGIN;
					R ret = (ptr->*method)(
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
			typedef R (C::*M)(A1, A2, A3, A4, A5, A6, A7);
			FUNCBEGIN;
			R ret = (ptr->*method)(
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

		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
		static SQInteger Func7C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2, A3, A4, A5, A6, A7) const;
			FUNCBEGIN;
			R ret = (ptr->*method)(
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
			typedef R (C::*M)(A1, A2, A3, A4, A5, A6, A7, A8);
			FUNCBEGIN;
			R ret = (ptr->*method)(
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

		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
		static SQInteger Func8C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2, A3, A4, A5, A6, A7, A8) const;
			FUNCBEGIN;
			R ret = (ptr->*method)(
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
			typedef R (C::*M)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
			FUNCBEGIN;
			R ret = (ptr->*method)(
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

		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
		static SQInteger Func9C(HSQUIRRELVM vm) {
			typedef R (C::*M)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const;
			FUNCBEGIN;
			R ret = (ptr->*method)(
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

	template <class C>
	class SqMember<C, void> {
	public:		
		// Arg Count 0
		static SQInteger Func0(HSQUIRRELVM vm) {
			typedef void (C::*M)();
			FUNCBEGIN;
			(ptr->*method)();
			return 0;
			FUNCEND;
		}

		static SQInteger Func0C(HSQUIRRELVM vm) {
			typedef void (C::*M)() const;
			FUNCBEGIN;
			(ptr->*method)();
			return 0;
			FUNCEND;
		}

		// Arg Count 1
		template <class A1>
		static SQInteger Func1(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1);
			FUNCBEGIN;
			(ptr->*method)(
				Var<A1>(vm, 2).value
				);
			return 0;
			FUNCEND;
		}

		template <class A1>
		static SQInteger Func1C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1) const;
			FUNCBEGIN;
			(ptr->*method)(
				Var<A1>(vm, 2).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 2
		template <class A1, class A2>
		static SQInteger Func2(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2);
			FUNCBEGIN;
			(ptr->*method)(
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value
				);
			return 0;
			FUNCEND;
		}

		template <class A1, class A2>
		static SQInteger Func2C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2) const;
			FUNCBEGIN;
			(ptr->*method)(
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value
				);
			return 0;
			FUNCEND;
		}

		// Arg Count 3
		template <class A1, class A2, class A3>
		static SQInteger Func3(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2, A3);
			FUNCBEGIN;
			(ptr->*method)(
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value
				);
			return 0;
			FUNCEND;
		}

		template <class A1, class A2, class A3>
		static SQInteger Func3C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2, A3) const;
			FUNCBEGIN;
			(ptr->*method)(
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
			typedef void (C::*M)(A1, A2, A3, A4);
			FUNCBEGIN;
			(ptr->*method)(
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value
				);
			return 0;
			FUNCEND;
		}

		template <class A1, class A2, class A3, class A4>
		static SQInteger Func4C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2, A3, A4) const;
			FUNCBEGIN;
			(ptr->*method)(
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
			typedef void (C::*M)(A1, A2, A3, A4, A5);
			FUNCBEGIN;
			(ptr->*method)(
				Var<A1>(vm, 2).value,
				Var<A2>(vm, 3).value,
				Var<A3>(vm, 4).value,
				Var<A4>(vm, 5).value,
				Var<A5>(vm, 6).value
				);
			return 0;
			FUNCEND;
		}

		template <class A1, class A2, class A3, class A4, class A5>
		static SQInteger Func5C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2, A3, A4, A5) const;
			FUNCBEGIN;
			(ptr->*method)(
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
			typedef void (C::*M)(A1, A2, A3, A4, A5, A6);
			FUNCBEGIN;
			(ptr->*method)(
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

		template <class A1, class A2, class A3, class A4, class A5, class A6>
		static SQInteger Func6C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2, A3, A4, A5, A6) const;
			FUNCBEGIN;
			(ptr->*method)(
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
			typedef void (C::*M)(A1, A2, A3, A4, A5, A6, A7);
			FUNCBEGIN;
			(ptr->*method)(
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

		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
		static SQInteger Func7C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2, A3, A4, A5, A6, A7) const;
			FUNCBEGIN;
			(ptr->*method)(
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
			typedef void (C::*M)(A1, A2, A3, A4, A5, A6, A7, A8);
			FUNCBEGIN;
			(ptr->*method)(
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

		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
		static SQInteger Func8C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2, A3, A4, A5, A6, A7, A8) const;
			FUNCBEGIN;
			(ptr->*method)(
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
			typedef void (C::*M)(A1, A2, A3, A4, A5, A6, A7, A8, A9);
			FUNCBEGIN;
			(ptr->*method)(
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

		template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
		static SQInteger Func9C(HSQUIRRELVM vm) {
			typedef void (C::*M)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const;
			FUNCBEGIN;
			(ptr->*method)(
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
	template <class C, class R>
	inline SQFUNCTION SqMemberFunc(R (C::*method)()) {
		return &SqMember<C, R>::Func0;
	}

	template <class C, class R>
	inline SQFUNCTION SqMemberFunc(R (C::*method)() const) {
		return &SqMember<C, R>::Func0C;
	}

	// Arg Count 1
	template <class C, class R, class A1>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1)) {
		return &SqMember<C, R>::template Func1<A1>;
	}

	template <class C, class R, class A1>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1) const) {
		return &SqMember<C, R>::template Func1C<A1>;
	}

	// Arg Count 2
	template <class C, class R, class A1, class A2>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2)) {
		return &SqMember<C, R>::template Func2<A1, A2>;
	}

	template <class C, class R, class A1, class A2>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2) const) {
		return &SqMember<C, R>::template Func2C<A1, A2>;
	}

	// Arg Count 3
	template <class C, class R, class A1, class A2, class A3>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3)) {
		return &SqMember<C, R>::template Func3<A1, A2, A3>;
	}

	template <class C, class R, class A1, class A2, class A3>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3) const) {
		return &SqMember<C, R>::template Func3C<A1, A2, A3>;
	}

	// Arg Count 4
	template <class C, class R, class A1, class A2, class A3, class A4>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4)) {
		return &SqMember<C, R>::template Func4<A1, A2, A3, A4>;
	}

	template <class C, class R, class A1, class A2, class A3, class A4>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4) const) {
		return &SqMember<C, R>::template Func4C<A1, A2, A3, A4>;
	}

	// Arg Count 5
	template <class C, class R, class A1, class A2, class A3, class A4, class A5>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5)) {
		return &SqMember<C, R>::template Func5<A1, A2, A3, A4, A5>;
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5) const) {
		return &SqMember<C, R>::template Func5C<A1, A2, A3, A4, A5>;
	}

	// Arg Count 6
	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5, A6)) {
		return &SqMember<C, R>::template Func6<A1, A2, A3, A4, A5, A6>;
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5, A6) const) {
		return &SqMember<C, R>::template Func6C<A1, A2, A3, A4, A5, A6>;
	}

	// Arg Count 7
	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5, A6, A7)) {
		return &SqMember<C, R>::template Func7<A1, A2, A3, A4, A5, A6, A7>;
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5, A6, A7) const) {
		return &SqMember<C, R>::template Func7C<A1, A2, A3, A4, A5, A6, A7>;
	}

	// Arg Count 8
	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8)) {
		return &SqMember<C, R>::template Func8<A1, A2, A3, A4, A5, A6, A7, A8>;
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8) const) {
		return &SqMember<C, R>::template Func8C<A1, A2, A3, A4, A5, A6, A7, A8>;
	}

	// Arg Count 9
	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9)) {
		return &SqMember<C, R>::template Func9<A1, A2, A3, A4, A5, A6, A7, A8, A9>;
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	inline SQFUNCTION SqMemberFunc(R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const) {
		return &SqMember<C, R>::template Func9C<A1, A2, A3, A4, A5, A6, A7, A8, A9>;
	}

	//
	// Variable Get
	//

	template <class C, class V>
	inline SQInteger sqDefaultGet(HSQUIRRELVM vm) {
		SQUserPointer up;
		if (SQ_SUCCEEDED(sq_getinstanceup(vm, 1, &up, NULL)) && up) {
			C* ptr = (C*)up;
			if (SQ_SUCCEEDED(sq_getuserdata(vm, -1, &up, NULL)) && up) { // Get Member...
				typedef V C::*M;
				M member = *(M*)up;
				PushVar(vm, ptr->*member);
				return 1;
			}
		}
		return ERROR_BADINSTANCE(vm);
	}

	inline SQInteger sqVarGet(HSQUIRRELVM vm) {
		// Find the get method in the get table
		sq_push(vm, 2);
		if (SQ_FAILED( sq_get(vm,-2) )) {
			return sq_throwerror(vm,_SC("Member Variable not found"));
		}

		// push 'this'
		sq_push(vm, 1);

		// Call the getter
		sq_call(vm, 1, true, ErrorHandling::IsEnabled());
		return 1;
	}

	//
	// Variable Set
	//

	template <class C, class V>
	inline SQInteger sqDefaultSet(HSQUIRRELVM vm) {
		SQUserPointer up;
		if (SQ_SUCCEEDED(sq_getinstanceup(vm, 1, &up, NULL))) {
			C* ptr = (C*)up;
			typedef V C::*M;
			if (SQ_SUCCEEDED(sq_getuserdata(vm, -1, &up, NULL)) && up) { // Get Member...
				M member = *(M*)up;
				ptr->*member = Var<V>(vm, 2).value;
				return 0;
			}
		}
		return ERROR_BADINSTANCE(vm);
	}

	inline SQInteger sqVarSet(HSQUIRRELVM vm) {
		// Find the set method in the set table
		sq_push(vm, 2);
		if (SQ_FAILED( sq_get(vm,-2) )) {
			return sq_throwerror(vm,_SC("Member Variable not found"));
		}

		// push 'this'
		sq_push(vm, 1);
		sq_push(vm, 3);

		// Call the setter
		sq_call(vm, 2, false, ErrorHandling::IsEnabled());

		return 0;
	}

	// SQFUNCTION style method
	template <class C>
	class SqVarArgMember {
	public:
		static SQInteger Func(HSQUIRRELVM vm) {
			typedef int (C::*M)(HSQUIRRELVM);
			SQUserPointer up;
			if (SQ_SUCCEEDED(sq_getinstanceup(vm, 1, &up, NULL)) && up) {
				C* ptr = (C*)up;;
				if (SQ_SUCCEEDED(sq_getuserdata(vm, -1, &up, NULL)) && up) {
					M method = *(M*)up;
					// clear method local value
					sq_pop(vm,1);
					// call method
					return (ptr->*method)(vm);
				}
			}
			return ERROR_BADINSTANCE(vm);
		}
	};

	// SQFUNCTION style method
	template <class C>
	inline SQFUNCTION SqVarArgMemberFunc(SQRESULT (C::*method)(HSQUIRRELVM)) {
		return &SqVarArgMember<C>::Func;
	};
}

#undef FUNCBEGIN
#undef FUNCEND

#endif
