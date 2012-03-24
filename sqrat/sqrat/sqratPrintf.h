//
// SqratPrintf: Print formatted string
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

#if !defined(_SCRAT_PRINTF_H_)
#define _SCRAT_PRINTF_H_

#include <squirrel.h>
#include <sqstdstring.h>

#include "sqratTypes.h"

namespace Sqrat {

	template <class DUMMY>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format) {
		sq_pushstring(vm, format, -1);
		return sqstd_printf(vm, 0);
	}

	template <class A1>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		return sqstd_printf(vm, 1);
	}

	template <class A1, class A2>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1, A2 a2) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		PushVar(vm, a2);
		return sqstd_printf(vm, 2);
	}

	template <class A1, class A2, class A3>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1, A2 a2, A3 a3) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		PushVar(vm, a2);
		PushVar(vm, a3);
		return sqstd_printf(vm, 3);
	}

	template <class A1, class A2, class A3, class A4>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1, A2 a2, A3 a3, A4 a4) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		PushVar(vm, a2);
		PushVar(vm, a3);
		PushVar(vm, a4);
		return sqstd_printf(vm, 4);
	}

	template <class A1, class A2, class A3, class A4, class A5>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		PushVar(vm, a2);
		PushVar(vm, a3);
		PushVar(vm, a4);
		PushVar(vm, a5);
		return sqstd_printf(vm, 5);
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		PushVar(vm, a2);
		PushVar(vm, a3);
		PushVar(vm, a4);
		PushVar(vm, a5);
		PushVar(vm, a6);
		return sqstd_printf(vm, 6);
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		PushVar(vm, a2);
		PushVar(vm, a3);
		PushVar(vm, a4);
		PushVar(vm, a5);
		PushVar(vm, a6);
		PushVar(vm, a7);
		return sqstd_printf(vm, 7);
	}

	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		PushVar(vm, a2);
		PushVar(vm, a3);
		PushVar(vm, a4);
		PushVar(vm, a5);
		PushVar(vm, a6);
		PushVar(vm, a7);
		PushVar(vm, a8);
		return sqstd_printf(vm, 8);
	}
	
	template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	inline SQInteger printf(HSQUIRRELVM vm, const SQChar *format, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) {
		sq_pushstring(vm, format, -1);
		PushVar(vm, a1);
		PushVar(vm, a2);
		PushVar(vm, a3);
		PushVar(vm, a4);
		PushVar(vm, a5);
		PushVar(vm, a6);
		PushVar(vm, a7);
		PushVar(vm, a8);
		PushVar(vm, a9);
		return sqstd_printf(vm, 9);
	}
}

#endif
