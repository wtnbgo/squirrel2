//
// SqratTypes: Type Translators
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

#if !defined(_SCRAT_TYPES_H_)
#define _SCRAT_TYPES_H_

#include <squirrel.h>
#include <sqobjectinfo.h>
#include <string>
#include <vector>
#include <map>

#include "sqratClassType.h"

namespace Sqrat {

	//
	// Variable Accessors
	//

	// Generic classes
	template<class T>
	struct Var {
		T value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			value = *ClassType<T>::GetInstance(vm, idx);
		}
		static void push(HSQUIRRELVM vm, T value) {
			ClassType<T>::PushInstanceCopy(vm, value);
		}
	};

	template<class T>
	struct Var<T&> {
		T& value;
		Var(HSQUIRRELVM vm, SQInteger idx) : value(*ClassType<T>::GetInstance(vm, idx)) {
		}
		static void push(HSQUIRRELVM vm, T value) {
			ClassType<T>::PushInstanceCopy(vm, value);
		}
	};

	template<class T>
	struct Var<T*> {
		T* value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			value = ClassType<T>::GetInstance(vm, idx);
		}
		static void push(HSQUIRRELVM vm, T* value) {
			ClassType<T>::PushInstanceInit(vm, value);
		}
	};
	
	template<class T>
	struct Var<const T&> {
		const T& value;
		Var(HSQUIRRELVM vm, SQInteger idx) : value(*ClassType<T>::GetInstance(vm, idx)) {
		}
		static void push(HSQUIRRELVM vm, T value) {
			ClassType<T>::PushInstanceCopy(vm, value);
		}
	};

	template<class T>
	struct Var<const T*> {
		const T* value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			value = ClassType<T>::GetInstance(vm, idx);
		}
		static void push(HSQUIRRELVM vm, const T* value) {
			ClassType<T>::PushInstance(vm, value);
		}
	};

	// Integer Types
	#define SCRAT_INTEGER( type ) \
	template<> \
	struct Var<type> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQInteger sqValue = 0; \
			sq_getinteger(vm, idx, &sqValue); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, type& value) { \
			sq_pushinteger(vm, static_cast<SQInteger>(value)); \
		} \
	};\
	\
	template<> \
	struct Var<const type> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQInteger sqValue = 0; \
			sq_getinteger(vm, idx, &sqValue); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, const type& value) { \
			sq_pushinteger(vm, static_cast<SQInteger>(value)); \
		} \
	}; \
	\
	template<> \
	struct Var<const type&> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQInteger sqValue = 0; \
			sq_getinteger(vm, idx, &sqValue); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, const type& value) { \
			sq_pushinteger(vm, static_cast<SQInteger>(value)); \
		} \
	};

	SCRAT_INTEGER(unsigned long long)
	SCRAT_INTEGER(signed long long)
	SCRAT_INTEGER(unsigned int)
	SCRAT_INTEGER(signed int)
	SCRAT_INTEGER(unsigned long)
	SCRAT_INTEGER(signed long)
	SCRAT_INTEGER(unsigned short)
	SCRAT_INTEGER(signed short)
	SCRAT_INTEGER(unsigned char)
	SCRAT_INTEGER(signed char)

#if defined(__int64)
	SCRAT_INTEGER(unsigned __int64)
	SCRAT_INTEGER(signed __int64)
#endif

	// Float Types
	#define SCRAT_FLOAT( type ) \
	template<> \
	struct Var<type> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQFloat sqValue = 0; \
			sq_getfloat(vm, idx, &sqValue); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, type& value) { \
			sq_pushfloat(vm, static_cast<SQFloat>(value)); \
		} \
	}; \
	\
	template<> \
	struct Var<const type> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQFloat sqValue = 0; \
			sq_getfloat(vm, idx, &sqValue); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, const type& value) { \
			sq_pushfloat(vm, static_cast<SQFloat>(value)); \
		} \
	}; \
	template<> \
	struct Var<const type&> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQFloat sqValue = 0; \
			sq_getfloat(vm, idx, &sqValue); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, const type& value) { \
			sq_pushfloat(vm, static_cast<SQFloat>(value)); \
		} \
	};

	SCRAT_FLOAT(float)
	SCRAT_FLOAT(double)

	// Boolean Types
	template<>
	struct Var<bool> {
		bool value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQBool sqValue;
			sq_tobool(vm, idx, &sqValue);
			value = (sqValue != 0);
		}
		static void push(HSQUIRRELVM vm, bool& value) {
			sq_pushbool(vm, static_cast<SQBool>(value));
		}
	};

	template<>
	struct Var<const bool> {
		bool value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQBool sqValue;
			sq_tobool(vm, idx, &sqValue);
			value = (sqValue != 0);
		}
		static void push(HSQUIRRELVM vm, const bool& value) {
			sq_pushbool(vm, static_cast<SQBool>(value));
		}
	};

	template<>
	struct Var<const bool&> {
		bool value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQBool sqValue;
			sq_tobool(vm, idx, &sqValue);
			value = (sqValue != 0);
		}
		static void push(HSQUIRRELVM vm, const bool& value) {
			sq_pushbool(vm, static_cast<SQBool>(value));
		}
	};

	// String Types
	typedef std::basic_string<SQChar> string;

	template<>
	struct Var<SQChar*> {
		SQChar* value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sq_tostring(vm, idx);
			sq_getstring(vm, -1, (const SQChar**)&value);
			sq_pop(vm,1);
		}
		static void push(HSQUIRRELVM vm, SQChar* value) {
			sq_pushstring(vm, value, -1);
		}
	};

	template<>
	struct Var<const SQChar*> {
		const SQChar* value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sq_tostring(vm, idx);
			sq_getstring(vm, -1, &value);
			sq_pop(vm,1);
		}
		static void push(HSQUIRRELVM vm, const SQChar* value) {
			sq_pushstring(vm, value, -1);
		}
	};

	template<>
	struct Var<string> {
		string value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			const SQChar* ret;
			sq_tostring(vm, idx);
			sq_getstring(vm, -1, &ret);
			value = string(ret);
			sq_pop(vm,1);
		}
		static void push(HSQUIRRELVM vm, string value) {
			sq_pushstring(vm, value.c_str(), -1);
		}
	};

	template<>
	struct Var<string&> {
		string value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			const SQChar* ret;
			sq_tostring(vm, idx);
			sq_getstring(vm, -1, &ret);
			value = string(ret);
			sq_pop(vm,1);
		}
		static void push(HSQUIRRELVM vm, string value) {
			sq_pushstring(vm, value.c_str(), -1);
		}
	};

	template<>
	struct Var<const string&> {
		string value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			const SQChar* ret;
			sq_tostring(vm, idx);
			sq_getstring(vm, -1, &ret);
			value = string(ret);
			sq_pop(vm,1);
		}
		static void push(HSQUIRRELVM vm, string value) {
			sq_pushstring(vm, value.c_str(), -1);
		}
	};

	// UserPointer
	template<>
	struct Var<SQUserPointer> {
		SQUserPointer value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sq_getuserpointer(vm, idx, &value);
		}
		static void push(HSQUIRRELVM vm, SQUserPointer& value) {
			sq_pushuserpointer(vm, value);
		}
	};

	//
	// ObjectInfo
	//

	template<>
	struct Var<sqobject::ObjectInfo> {
		sqobject::ObjectInfo value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			value.getStack(vm, idx);
		}
		static void push(HSQUIRRELVM vm, sqobject::ObjectInfo value) {
			value.push(vm);
		}
	};
	
	template<>
	struct Var<sqobject::ObjectInfo&> {
		sqobject::ObjectInfo value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			value.getStack(vm, idx);
		}
		static void push(HSQUIRRELVM vm, sqobject::ObjectInfo &value) {
			value.push(vm);
		}
	};

	template<>
	struct Var<const sqobject::ObjectInfo&> {
		sqobject::ObjectInfo value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			value.getStack(vm, idx);
		}
		static void push(HSQUIRRELVM vm, const sqobject::ObjectInfo &value) {
			value.push(vm);
		}
	};
	
	//
	// Vector
	//
	
	template<class T>
	struct Var<std::vector<T> > {
		typedef std::vector<T> V;
		V value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sqobject::ObjectInfo array(vm, idx);
			if (array.isArray()) {
				int n = array.len();
				for (int i=0;i<n;i++) {
					value.push_back((T)array[i]);
				}
			}
		}
	};

	template<class T>
	struct Var<std::vector<T>&> {
		typedef std::vector<T> V;
		V value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sqobject::ObjectInfo array(vm, idx);
			if (array.isArray()) {
				int n = array.len();
				for (int i=0;i<n;i++) {
					value.push_back((T)array[i]);
				}
			}
		}
	};

	template<class T>
	struct Var<const std::vector<T>&> {
		typedef std::vector<T> V;
		V value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sqobject::ObjectInfo array(vm, idx);
			if (array.isArray()) {
				int n = array.len();
				for (int i=0;i<n;i++) {
					value.push_back((T)array[i]);
				}
			}
		}
	};


	//
	// Map
	//
	
	template<class K, class T>
	struct Var<std::map<K,T> > {
		typedef std::map<K,T> M;
		M value;
		void operator()(K key, T v) {
			value[key] = v;
		}
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sqobject::ObjectInfo table(vm, idx);
			if (table.isTable()) {
				table.foreach(*this);
			}
		}
	};

	template<class K, class T>
	struct Var<std::map<K,T>&> {
		typedef std::map<K,T> M;
		M value;
		void operator()(K key, T v) {
			value[key] = v;
		}
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sqobject::ObjectInfo table(vm, idx);
			if (table.isTable()) {
				table.foreach(*this);
			}
		}
	};

	template<class K, class T>
	struct Var<const std::map<K,T>&> {
		typedef std::map<K,T> M;
		M value;
		void operator()(K key, T v) {
			value[key] = v;
		}
		Var(HSQUIRRELVM vm, SQInteger idx) {
			sqobject::ObjectInfo table(vm, idx);
			if (table.isTable()) {
				table.foreach(*this);
			}
		}
	};
	
	//
	// Variable Accessors
	//

	// Push
	template<class T>
	inline void PushVar(HSQUIRRELVM vm, T value) {
		Var<T>::push(vm, value);
	}
}

#endif
