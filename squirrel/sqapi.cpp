/*
	see copyright notice in squirrel.h
*/
#include <stdarg.h>
#include "sqpcheader.h"
#include "sqvm.h"
#include "sqstring.h"
#include "sqtable.h"
#include "sqarray.h"
#include "sqfuncproto.h"
#include "sqclosure.h"
#include "squserdata.h"
#include "sqcompiler.h"
#include "sqfuncstate.h"
#include "sqclass.h"

bool sq_aux_gettypedarg(HSQUIRRELVM v,SQInteger idx,SQObjectType type,SQObjectPtr **o)
{
	*o = &stack_get(v,idx);
	if(sqtype(**o) != type){
		SQObjectPtr oval = v->PrintObjVal(**o);
		v->Raise_ErrorF(_SC("wrong argument type, expected '%s' got '%.50s'"),IdType2Name(type),_stringval(oval));
		return false;
	}
	return true;
}

#define _GETSAFE_OBJ(v,idx,type,o) { if(!sq_aux_gettypedarg(v,idx,type,&o)) return SQ_ERROR; }

#define sq_aux_paramscheck(v,count) \
{ \
	if(sq_gettop(v) < count){ v->Raise_Error(_SC("not enough params in the stack")); return SQ_ERROR; }\
}		

SQInteger sq_aux_throwobject(HSQUIRRELVM v,SQObjectPtr &e)
{
	v->Raise_Error(e);
	return SQ_ERROR;
}

SQInteger sq_aux_invalidtype(HSQUIRRELVM v,SQObjectType type)
{
	scsprintf(_ss(v)->GetScratchPad(100), _SC("unexpected type %s"), IdType2Name(type));
	return sq_throwerror(v, _ss(v)->GetScratchPad(-1));
}

HSQUIRRELVM sq_open(SQInteger initialstacksize)
{
	SQSharedState *ss;
	SQVM *v;
	sq_new(ss, SQSharedState);
	ss->Init();
	v = (SQVM *)SQ_MALLOC(sizeof(SQVM));
	new (v) SQVM(ss);
	ss->_root_vm = v;
	if(v->Init(NULL, initialstacksize)) {
		return v;
	} else {
		sq_delete(v, SQVM);
		return NULL;
	}
	return v;
}

HSQUIRRELVM sq_newthread(HSQUIRRELVM friendvm, SQInteger initialstacksize)
{
	SQSharedState *ss;
	SQVM *v;
	ss=_ss(friendvm);
	
	v= (SQVM *)SQ_MALLOC(sizeof(SQVM));
	new (v) SQVM(ss);
	
	if(v->Init(friendvm, initialstacksize)) {
		friendvm->Push(v);
		return v;
	} else {
		sq_delete(v, SQVM);
		return NULL;
	}
}

SQInteger sq_getvmstate(HSQUIRRELVM v)
{
	if(v->_suspended)
		return SQ_VMSTATE_SUSPENDED;
	else { 
		if(v->_callsstacksize != 0) return SQ_VMSTATE_RUNNING;
		else return SQ_VMSTATE_IDLE;
	}
}

void sq_seterrorhandler(HSQUIRRELVM v)
{
	SQObject o = stack_get(v, -1);
	if(sq_isclosure(o) || sq_isnativeclosure(o) || sq_isnull(o)) {
		v->_errorhandler = o;
		v->Pop();
	}
}

void sq_setdebughook(HSQUIRRELVM v)
{
	SQObject o = stack_get(v,-1);
	if(sq_isclosure(o) || sq_isnativeclosure(o) || sq_isnull(o)) {
		v->_debughook = o;
		v->Pop();
	}
}

void sq_close(HSQUIRRELVM v)
{
	SQSharedState *ss = _ss(v);
	_thread(ss->_root_vm)->Finalize();
	sq_delete(ss, SQSharedState);
}

SQRESULT sq_compile(HSQUIRRELVM v,SQLEXREADFUNC read,SQUserPointer p,const SQChar *sourcename,SQBool raiseerror)
{
	SQObjectPtr o;
	if(Compile(v, read, p, sourcename, o, raiseerror?true:false, _ss(v)->_debuginfo)) {
		v->Push(SQClosure::Create(_ss(v), _funcproto(o)));
		return SQ_OK;
	}
	return SQ_ERROR;
}

void sq_enabledebuginfo(HSQUIRRELVM v, SQBool enable)
{
	_ss(v)->_debuginfo = enable?true:false;
}

void sq_notifyallexceptions(HSQUIRRELVM v, SQBool enable)
{
	_ss(v)->_notifyallexceptions = enable?true:false;
}

void sq_addref(HSQUIRRELVM v,HSQOBJECT *po)
{
	if(!ISREFCOUNTED(sqtype(*po))) return;
#ifdef NO_GARBAGE_COLLECTOR
	__AddRef(po->_type,po->_unVal);
#else
	_ss(v)->_refs_table.AddRef(*po);
#endif
}

SQBool sq_release(HSQUIRRELVM v,HSQOBJECT *po)
{
	if(!ISREFCOUNTED(sqtype(*po))) return SQTrue;
#ifdef NO_GARBAGE_COLLECTOR
	__Release(po->_type,po->_unVal);
	return SQFalse; //the ret val doesn't work(and cannot be fixed)
#else
	return _ss(v)->_refs_table.Release(*po);
#endif
}

const SQChar *sq_objtostring(HSQOBJECT *o) 
{
	if(sq_type(*o) == OT_STRING) {
		return _stringval(*o);
	}
	return NULL;
}

SQInteger sq_objtointeger(HSQOBJECT *o) 
{
	if(sq_isnumeric(*o)) {
		return tointeger(*o);
	}
	return 0;
}

SQLong sq_objtolong(HSQOBJECT *o) 
{
	if(sq_isnumeric(*o)) {
		return tolong(*o);
	}
	return 0;
}

SQFloat sq_objtofloat(HSQOBJECT *o) 
{
	if(sq_isnumeric(*o)) {
		return tofloat(*o);
	}
	return 0;
}

SQBool sq_objtobool(HSQOBJECT *o) 
{
	if(sq_isbool(*o)) {
		return _integer(*o);
	}
	return SQFalse;
}

void sq_pushnull(HSQUIRRELVM v)
{
	v->Push(_null_);
}

void sq_pushstring(HSQUIRRELVM v,const SQChar *s,SQInteger len)
{
	if(s)
		v->Push(SQObjectPtr(SQString::Create(_ss(v), s, len)));
	else v->Push(_null_);
}

void sq_pushinteger(HSQUIRRELVM v,SQInteger n)
{
	v->Push(n);
}

void sq_pushlong(HSQUIRRELVM v,SQLong n)
{
	v->Push(n);
}

void sq_pushbool(HSQUIRRELVM v,SQBool b)
{
	v->Push(b?true:false);
}

void sq_pushfloat(HSQUIRRELVM v,SQFloat n)
{
	v->Push(n);
}

void sq_pushuserpointer(HSQUIRRELVM v,SQUserPointer p)
{
	v->Push(p);
}

SQUserPointer sq_newuserdata(HSQUIRRELVM v,SQUnsignedInteger size)
{
	SQUserData *ud = SQUserData::Create(_ss(v), size);
	v->Push(ud);
	return ud->_val;
}

void sq_newtable(HSQUIRRELVM v)
{
	v->Push(SQTable::Create(_ss(v), 0));	
}

void sq_newarray(HSQUIRRELVM v,SQInteger size)
{
	v->Push(SQArray::Create(_ss(v), size));	
}

SQRESULT sq_newclass(HSQUIRRELVM v,SQBool hasbase)
{
	SQClass *baseclass = NULL;
	if(hasbase) {
		SQObjectPtr &base = stack_get(v,-1);
		if(sqtype(base) != OT_CLASS)
			return sq_throwerror(v,_SC("invalid base type"));
		baseclass = _class(base);
	}
	SQClass *newclass = SQClass::Create(_ss(v), baseclass);
	if(baseclass) v->Pop();
	v->Push(newclass);	
	return SQ_OK;
}

SQBool sq_instanceof(HSQUIRRELVM v)
{
	SQObjectPtr &inst = stack_get(v,-1);
	SQObjectPtr &cl = stack_get(v,-2);
	if(sqtype(inst) != OT_INSTANCE || sqtype(cl) != OT_CLASS)
		return sq_throwerror(v,_SC("invalid param type"));
	return _instance(inst)->InstanceOf(_class(cl))?SQTrue:SQFalse;
}

SQRESULT sq_arrayappend(HSQUIRRELVM v,SQInteger idx)
{
	sq_aux_paramscheck(v,2);
	SQObjectPtr *arr;
	_GETSAFE_OBJ(v, idx, OT_ARRAY,arr);
	_array(*arr)->Append(v->GetUp(-1));
	v->Pop(1);
	return SQ_OK;
}

SQRESULT sq_arraypop(HSQUIRRELVM v,SQInteger idx,SQBool pushval)
{
	sq_aux_paramscheck(v, 1);
	SQObjectPtr *arr;
	_GETSAFE_OBJ(v, idx, OT_ARRAY,arr);
	if(_array(*arr)->Size() > 0) {
        if(pushval != 0){ v->Push(_array(*arr)->Top()); }
		_array(*arr)->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v, _SC("empty array"));
}

SQRESULT sq_arrayresize(HSQUIRRELVM v,SQInteger idx,SQInteger newsize)
{
	sq_aux_paramscheck(v,1);
	SQObjectPtr *arr;
	_GETSAFE_OBJ(v, idx, OT_ARRAY,arr);
	if(newsize >= 0) {
		_array(*arr)->Resize(newsize);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("negative size"));
}


SQRESULT sq_arrayreverse(HSQUIRRELVM v,SQInteger idx)
{
	sq_aux_paramscheck(v, 1);
	SQObjectPtr *o;
	_GETSAFE_OBJ(v, idx, OT_ARRAY,o);
	SQArray *arr = _array(*o);
	if(arr->Size() > 0) {
		SQObjectPtr t;
		SQInteger size = arr->Size();
		SQInteger n = size >> 1; size -= 1;
		for(SQInteger i = 0; i < n; i++) {
			t = arr->_values[i];
			arr->_values[i] = arr->_values[size-i];
			arr->_values[size-i] = t;
		}
		return SQ_OK;
	}
	return SQ_OK;
}

SQRESULT sq_arrayremove(HSQUIRRELVM v,SQInteger idx,SQInteger itemidx)
{
	sq_aux_paramscheck(v, 1); 
	SQObjectPtr *arr;
	_GETSAFE_OBJ(v, idx, OT_ARRAY,arr); 
	return _array(*arr)->Remove(itemidx) ? SQ_OK : sq_throwerror(v,_SC("index out of range")); 
}

SQRESULT sq_arrayinsert(HSQUIRRELVM v,SQInteger idx,SQInteger destpos)
{
	sq_aux_paramscheck(v, 1); 
	SQObjectPtr *arr;
	_GETSAFE_OBJ(v, idx, OT_ARRAY,arr);
	SQRESULT ret = _array(*arr)->Insert(destpos, v->GetUp(-1)) ? SQ_OK : sq_throwerror(v,_SC("index out of range"));
	v->Pop();
	return ret;
}

SQRESULT sq_arrayremovevalue(HSQUIRRELVM v, SQInteger idx, SQBool all)
{
	sq_aux_paramscheck(v, 2);
	SQObjectPtr *o;
	_GETSAFE_OBJ(v, idx, OT_ARRAY, o); 
	SQArray *arr = _array(*o);
	SQObjectPtr &value = v->GetUp(-1);
	SQInteger i = 0;
	SQInteger alen = arr->Size();
	while (i<alen){
		SQObjectPtr avalue;
		arr->Get(i,avalue);
		SQInteger res;
		v->ObjCmp(value,avalue,res);
		if (res == 0) {
			arr->Remove(i);
			alen--;
			if (!all) {
				v->Pop(1);
				return SQ_OK;
			}
		} else {
			i++;
		}
	}
	v->Pop(1);
	return SQ_OK;
}

void sq_newclosure(HSQUIRRELVM v,SQFUNCTION func,SQUnsignedInteger nfreevars)
{
	SQNativeClosure *nc = SQNativeClosure::Create(_ss(v), func);
	nc->_nparamscheck = 0;
	for(SQUnsignedInteger i = 0; i < nfreevars; i++) {
		nc->_outervalues.push_back(v->Top());
		v->Pop();
	}
	v->Push(SQObjectPtr(nc));	
}

SQRESULT sq_getclosureinfo(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger *nparams,SQUnsignedInteger *nfreevars)
{
	SQObject o = stack_get(v, idx);
	if(sq_isclosure(o)) {
		SQClosure *c = _closure(o);
		SQFunctionProto *proto = _funcproto(c->_function);
		*nparams = (SQUnsignedInteger)proto->_nparameters;
        *nfreevars = (SQUnsignedInteger)c->_outervalues.size();
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("the object is not a closure"));
}

SQRESULT sq_setnativeclosurename(HSQUIRRELVM v,SQInteger idx,const SQChar *name)
{
	SQObject o = stack_get(v, idx);
	if(sq_isnativeclosure(o)) {
		SQNativeClosure *nc = _nativeclosure(o);
		nc->_name = SQString::Create(_ss(v),name);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("the object is not a nativeclosure"));
}

SQRESULT sq_setparamscheck(HSQUIRRELVM v,SQInteger nparamscheck,const SQChar *typemask)
{
	SQObject o = stack_get(v, -1);
	if(!sq_isnativeclosure(o))
		return sq_throwerror(v, _SC("native closure expected"));
	SQNativeClosure *nc = _nativeclosure(o);
	nc->_nparamscheck = nparamscheck;
	if(typemask) {
		SQIntVec res;
		if(!CompileTypemask(res, typemask))
			return sq_throwerror(v, _SC("invalid typemask"));
		nc->_typecheck.copy(res);
	}
	else {
		nc->_typecheck.resize(0);
	}
	if(nparamscheck == SQ_MATCHTYPEMASKSTRING) {
		nc->_nparamscheck = nc->_typecheck.size();
	}
	return SQ_OK;
}

SQRESULT sq_bindenv(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(!sq_isnativeclosure(o) &&
		!sq_isclosure(o))
		return sq_throwerror(v,_SC("the target is not a closure"));
    SQObjectPtr &env = stack_get(v,-1);
	if(!sq_istable(env) &&
		!sq_isclass(env) &&
		!sq_isinstance(env))
		return sq_throwerror(v,_SC("invalid environment"));
	SQObjectPtr w = _refcounted(env)->GetWeakRef(sqtype(env));
	SQObjectPtr ret;
	if(sq_isclosure(o)) {
		SQClosure *c = _closure(o)->Clone();
		c->_env = w;
		ret = c;
	}
	else { //then must be a native closure
		SQNativeClosure *c = _nativeclosure(o)->Clone();
		c->_env = w;
		ret = c;
	}
	v->Pop();
	v->Push(ret);
	return SQ_OK;
}

SQRESULT sq_getenv(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(!sq_isnativeclosure(o) &&
	   !sq_isclosure(o))
		return sq_throwerror(v,_SC("the target is not a closure"));
	SQObjectPtr ret;
	if (sq_isclosure(o)) {
		ret = _closure(o)->_env;
	} else { //then must be a native closure
		ret = _nativeclosure(o)->_env;
	}
	v->Push(ret);
	return SQ_OK;
}

SQBool sq_hasenv(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	return ((sq_isclosure(o) && sqtype(_closure(o)->_env) != OT_NULL) ||
			(sq_isnativeclosure(o) && sqtype(_nativeclosure(o)->_env) != OT_NULL)) ? SQTrue : SQFalse;
}

SQRESULT sq_clear(HSQUIRRELVM v,SQInteger idx)
{
	SQObject &o=stack_get(v,idx);
	switch(sqtype(o)) {
		case OT_TABLE: _table(o)->Clear();	break;
		case OT_ARRAY: _array(o)->Resize(0); break;
		default:
			return sq_throwerror(v, _SC("clear only works on table and array"));
		break;

	}
	return SQ_OK;
}

void sq_pushroottable(HSQUIRRELVM v)
{
	v->Push(v->_roottable);
}

void sq_pushregistrytable(HSQUIRRELVM v)
{
	v->Push(_ss(v)->_registry);
}

void sq_pushconsttable(HSQUIRRELVM v)
{
	v->Push(_ss(v)->_consts);
}

SQRESULT sq_setroottable(HSQUIRRELVM v)
{
	SQObject o = stack_get(v, -1);
	if(sq_istable(o) || sq_isnull(o)) {
		v->_roottable = o;
		v->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v, _SC("ivalid type"));
}

SQRESULT sq_setconsttable(HSQUIRRELVM v)
{
	SQObject o = stack_get(v, -1);
	if(sq_istable(o)) {
		_ss(v)->_consts = o;
		v->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v, _SC("ivalid type, expected table"));
}

SQRESULT sq_setexceptionclass(HSQUIRRELVM v)
{
	SQObject o = stack_get(v, -1);
	if(sq_isclass(o)) {
		_ss(v)->_exceptionclass = o;
		v->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v, _SC("ivalid type"));
}

void sq_setforeignptr(HSQUIRRELVM v,SQUserPointer p)
{
	v->_foreignptr = p;
}

SQUserPointer sq_getforeignptr(HSQUIRRELVM v)
{
	return v->_foreignptr;
}

void sq_push(HSQUIRRELVM v,SQInteger idx)
{
	v->Push(stack_get(v, idx));
}

SQObjectType sq_gettype(HSQUIRRELVM v,SQInteger idx)
{
	return sqtype(stack_get(v, idx));
}


void sq_tostring(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v, idx);
	SQObjectPtr res;
	v->ToString(o,res);
	v->Push(res);
}

void sq_tobool(HSQUIRRELVM v, SQInteger idx, SQBool *b)
{
	SQObjectPtr &o = stack_get(v, idx);
	*b = v->IsFalse(o)?SQFalse:SQTrue;
}

SQRESULT sq_getinteger(HSQUIRRELVM v,SQInteger idx,SQInteger *i)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isnumeric(o)) {
		*i = tointeger(o);
		return SQ_OK;
	} else if (sq_isbool(o)) {
		*i = _integer(o);
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_getlong(HSQUIRRELVM v,SQInteger idx,SQLong *i)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isnumeric(o)) {
		*i = static_cast<SQLong>(tolong(o));
		return SQ_OK;
	} else if (sq_isbool(o)) {
		*i = static_cast<SQLong>(_long(o));
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_getfloat(HSQUIRRELVM v,SQInteger idx,SQFloat *f)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isnumeric(o)) {
		*f = tofloat(o);
		return SQ_OK;
	} else if (sq_isbool(o)) {
		*f = (SQFloat)_integer(o);
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_getbool(HSQUIRRELVM v,SQInteger idx,SQBool *b)
{
	SQObjectPtr &o = stack_get(v, idx);
	if(sq_isbool(o)) {
		*b = _integer(o);
		return SQ_OK;
	} else if(sq_isnumeric(o)) {
		*b = tointeger(o) != 0 ? SQTrue : SQFalse;
		return SQ_OK;
	}
	return SQ_ERROR;
}

SQRESULT sq_getstring(HSQUIRRELVM v,SQInteger idx,const SQChar **c)
{
	SQObjectPtr *o = NULL;
	*c = NULL;
	_GETSAFE_OBJ(v, idx, OT_STRING,o);
	*c = _stringval(*o);
	return SQ_OK;
}

SQRESULT sq_getthread(HSQUIRRELVM v,SQInteger idx,HSQUIRRELVM *thread)
{
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, idx, OT_THREAD,o);
	*thread = _thread(*o);
	return SQ_OK;
}

SQRESULT sq_clone(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	v->Push(_null_);
	if(!v->Clone(o, stack_get(v, -1))){
		v->Pop();
		return sq_aux_invalidtype(v, sqtype(o));
	}
	return SQ_OK;
}

SQInteger sq_getsize(HSQUIRRELVM v, SQInteger idx)
{
	SQObjectPtr &o = stack_get(v, idx);
	SQObjectType type = sqtype(o);
	switch(type) {
	case OT_STRING:		return _string(o)->_len;
	case OT_TABLE:		return _table(o)->CountUsed();
	case OT_ARRAY:		return _array(o)->Size();
	case OT_USERDATA:	return _userdata(o)->_size;
	default:
		return sq_aux_invalidtype(v, type);
	}
}

SQRESULT sq_getuserdata(HSQUIRRELVM v,SQInteger idx,SQUserPointer *p,SQUserPointer *typetag)
{
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, idx, OT_USERDATA,o);
	(*p) = _userdataval(*o);
	if(typetag) *typetag = _userdata(*o)->_typetag;
	return SQ_OK;
}

SQRESULT sq_settypetag(HSQUIRRELVM v,SQInteger idx,SQUserPointer typetag)
{
	SQObjectPtr &o = stack_get(v,idx);
	switch(sqtype(o)) {
		case OT_USERDATA:	_userdata(o)->_typetag = typetag;	break;
		case OT_CLASS:		_class(o)->_typetag = typetag;		break;
		default:			return sq_throwerror(v,_SC("invalid object type"));
	}
	return SQ_OK;
}

SQRESULT sq_getobjtypetag(HSQOBJECT *o,SQUserPointer * typetag)
{
  switch(sqtype(*o)) {
    case OT_INSTANCE: *typetag = _instance(*o)->_class->_typetag; break;
    case OT_USERDATA: *typetag = _userdata(*o)->_typetag; break;
    case OT_CLASS:    *typetag = _class(*o)->_typetag; break;
    default: return SQ_ERROR;
  }
  return SQ_OK;
}

SQRESULT sq_gettypetag(HSQUIRRELVM v,SQInteger idx,SQUserPointer *typetag)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(SQ_FAILED(sq_getobjtypetag(&o,typetag)))
		return sq_throwerror(v,_SC("invalid object type"));
	return SQ_OK;
}

SQRESULT sq_getuserpointer(HSQUIRRELVM v, SQInteger idx, SQUserPointer *p)
{
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, idx, OT_USERPOINTER,o);
	(*p) = _userpointer(*o);
	return SQ_OK;
}

SQRESULT sq_setinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer p)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(sqtype(o) != OT_INSTANCE) return sq_throwerror(v,_SC("the object is not a class instance"));
	_instance(o)->_userpointer = p;
	return SQ_OK;
}

SQRESULT sq_setclassudsize(HSQUIRRELVM v, SQInteger idx, SQInteger udsize)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(sqtype(o) != OT_CLASS) return sq_throwerror(v,_SC("the object is not a class"));
	if(_class(o)->_locked) return sq_throwerror(v,_SC("the class is locked"));
	_class(o)->_udsize = udsize;
	return SQ_OK;
}


SQRESULT sq_getinstanceup(HSQUIRRELVM v, SQInteger idx, SQUserPointer *p,SQUserPointer typetag)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(sqtype(o) != OT_INSTANCE) return sq_throwerror(v,_SC("the object is not a class instance"));
	(*p) = _instance(o)->_userpointer;
	if(typetag != 0) {
		SQClass *cl = _instance(o)->_class;
		do{
			if(cl->_typetag == typetag)
				return SQ_OK;
			cl = cl->_base;
		}while(cl != NULL);
		return sq_throwerror(v,_SC("invalid type tag"));
	}
	return SQ_OK;
}

SQInteger sq_gettop(HSQUIRRELVM v)
{
	return (v->_top) - v->_stackbase;
}

void sq_settop(HSQUIRRELVM v, SQInteger newtop)
{
	SQInteger top = sq_gettop(v);
	if(top > newtop)
		sq_pop(v, top - newtop);
	else
		while(top++ < newtop) sq_pushnull(v);
}

void sq_pop(HSQUIRRELVM v, SQInteger nelemstopop)
{
	assert(v->_top >= nelemstopop);
	v->Pop(nelemstopop);
}

void sq_poptop(HSQUIRRELVM v)
{
	assert(v->_top >= 1);
    v->Pop();
}


void sq_remove(HSQUIRRELVM v, SQInteger idx)
{
	v->Remove(idx);
}

SQInteger sq_cmp(HSQUIRRELVM v)
{
	SQInteger res;
	v->ObjCmp(stack_get(v, -1), stack_get(v, -2),res);
	return res;
}

SQRESULT sq_newslot(HSQUIRRELVM v, SQInteger idx, SQBool bstatic)
{
	sq_aux_paramscheck(v, 3);
	SQObjectPtr &self = stack_get(v, idx);
	if(sqtype(self) == OT_TABLE || sqtype(self) == OT_CLASS) {
		SQObjectPtr &key = v->GetUp(-2);
		if(sqtype(key) == OT_NULL) {
			v->Pop(2);
			return sq_throwerror(v, _SC("null is not a valid key"));
		}
		v->NewSlot(self, key, v->GetUp(-1),bstatic?true:false);
		v->Pop(2);
	} else {
		v->Pop(2);
	}
	return SQ_OK;
}

SQRESULT sq_deleteslot(HSQUIRRELVM v,SQInteger idx,SQBool pushval)
{
	sq_aux_paramscheck(v, 2);
	SQObjectPtr *self;
	_GETSAFE_OBJ(v, idx, OT_TABLE,self);
	SQObjectPtr &key = v->GetUp(-1);
	if(sqtype(key) == OT_NULL) return sq_throwerror(v, _SC("null is not a valid key"));
	SQObjectPtr res;
	if(!v->DeleteSlot(*self, key, res)){
		return SQ_ERROR;
	}
	if(pushval)	v->GetUp(-1) = res;
	else v->Pop(1);
	return SQ_OK;
}

SQRESULT sq_set(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self = stack_get(v, idx);
	if(v->Set(self, v->GetUp(-2), v->GetUp(-1),false)) {
		v->Pop(2);
		return SQ_OK;
	}
	v->Raise_IdxError(v->GetUp(-2));
	v->Pop(2);
	return SQ_ERROR;
}

SQRESULT sq_rawset(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self = stack_get(v, idx);
	if(sqtype(v->GetUp(-2)) == OT_NULL) {
		v->Pop(2);
		return sq_throwerror(v, _SC("null key"));
	}
	switch(sqtype(self)) {
	case OT_TABLE:
		_table(self)->NewSlot(v->GetUp(-2), v->GetUp(-1));
		v->Pop(2);
		return SQ_OK;
	break;
	case OT_CLASS:
		_class(self)->NewSlot(_ss(v), v->GetUp(-2), v->GetUp(-1),false);
		v->Pop(2);
		return SQ_OK;
	break;
	case OT_INSTANCE:
		if(_instance(self)->Set(v->GetUp(-2), v->GetUp(-1))) {
			v->Pop(2);
			return SQ_OK;
		}
	break;
	case OT_ARRAY:
		if(v->Set(self, v->GetUp(-2), v->GetUp(-1),false)) {
			v->Pop(2);
			return SQ_OK;
		}
	break;
	default:
		v->Pop(2);
		return sq_throwerror(v, _SC("rawset works only on array/table/class and instance"));
	}
	v->Raise_IdxError(v->GetUp(-2));
	v->Pop(2);
	return SQ_ERROR;
}

SQRESULT sq_setdelegate(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self = stack_get(v, idx);
	SQObjectPtr &mt = v->GetUp(-1);
	SQObjectType type = sqtype(self);
	switch(type) {
	case OT_TABLE:
		if(sqtype(mt) == OT_TABLE) {
			if(!_table(self)->SetDelegate(_table(mt))) return sq_throwerror(v, _SC("delagate cycle")); v->Pop();}
		else if(sqtype(mt)==OT_NULL) {
			_table(self)->SetDelegate(NULL); v->Pop(); }
		else return sq_aux_invalidtype(v,type);
		break;
	case OT_USERDATA:
		if(sqtype(mt)==OT_TABLE) {
			_userdata(self)->SetDelegate(_table(mt)); v->Pop(); }
		else if(sqtype(mt)==OT_NULL) {
			_userdata(self)->SetDelegate(NULL); v->Pop(); }
		else return sq_aux_invalidtype(v, type);
		break;
	default:
			return sq_aux_invalidtype(v, type);
		break;
	}
	return SQ_OK;
}

SQRESULT sq_rawdeleteslot(HSQUIRRELVM v,SQInteger idx,SQBool pushval)
{
	sq_aux_paramscheck(v, 2);
	SQObjectPtr *self;
	_GETSAFE_OBJ(v, idx, OT_TABLE,self);
	SQObjectPtr &key = v->GetUp(-1);
	SQObjectPtr t;
	if(_table(*self)->Get(key,t)) {
		_table(*self)->Remove(key);
	}
	if(pushval != 0)
		if(pushval)	v->GetUp(-1) = t;
	else
		v->Pop(1);
	return SQ_OK;
}

SQRESULT sq_getdelegate(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self=stack_get(v,idx);
	switch(sqtype(self)){
	case OT_TABLE:
	case OT_USERDATA:
		if(!_delegable(self)->_delegate){
			v->Push(_null_);
			break;
		}
		v->Push(SQObjectPtr(_delegable(self)->_delegate));
		break;
	default: return sq_throwerror(v,_SC("wrong type")); break;
	}
	return SQ_OK;
	
}

SQRESULT sq_get(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self=stack_get(v,idx);
	if(v->Get(self,v->GetUp(-1),v->GetUp(-1),false,false))
		return SQ_OK;
	v->Pop(1);
	return sq_throwerror(v,_SC("the index doesn't exist"));
}

SQBool sq_exists(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self=stack_get(v,idx);
	if(v->Get(self,v->GetUp(-1),v->GetUp(-1),true,false)) {
		v->Pop(1);
		return SQTrue;
	} else {
		v->Pop(1);
		return SQFalse;
	}
}

SQRESULT sq_rawget(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &self=stack_get(v,idx);
	switch(sqtype(self)) {
	case OT_TABLE:
		if(_table(self)->Get(v->GetUp(-1),v->GetUp(-1)))
			return SQ_OK;
		break;
	case OT_CLASS:
		if(_class(self)->Get(v->GetUp(-1),v->GetUp(-1)))
			return SQ_OK;
		break;
	case OT_INSTANCE:
		if(_instance(self)->Get(v->GetUp(-1),v->GetUp(-1)))
			return SQ_OK;
		break;
	case OT_ARRAY:
		if(v->Get(self,v->GetUp(-1),v->GetUp(-1),false,false))
			return SQ_OK;
		break;
	default:
		v->Pop(1);
		return sq_throwerror(v,_SC("rawget works only on array/table/instance and class"));
	}	
	v->Pop(1);
	return sq_throwerror(v,_SC("the index doesn't exist"));
}

SQRESULT sq_getstackobj(HSQUIRRELVM v,SQInteger idx,HSQOBJECT *po)
{
	*po=stack_get(v,idx);
	return SQ_OK;
}

const SQChar *sq_getlocal(HSQUIRRELVM v,SQUnsignedInteger level,SQUnsignedInteger idx)
{
	SQUnsignedInteger cstksize=v->_callsstacksize;
	SQUnsignedInteger lvl=(cstksize-level)-1;
	SQInteger stackbase=v->_stackbase;
	if(lvl<cstksize){
		for(SQUnsignedInteger i=0;i<level;i++){
			SQVM::CallInfo &ci=v->_callsstack[(cstksize-i)-1];
			stackbase-=ci._prevstkbase;
		}
		SQVM::CallInfo &ci=v->_callsstack[lvl];
		if(sqtype(ci._closure)!=OT_CLOSURE)
			return NULL;
		SQClosure *c=_closure(ci._closure);
		SQFunctionProto *func=_funcproto(c->_function);
		if(func->_noutervalues > (SQInteger)idx) {
			v->Push(c->_outervalues[idx]);
			return _stringval(func->_outervalues[idx]._name);
		}
		idx -= func->_noutervalues;
		return func->GetLocal(v,stackbase,idx,(SQInteger)(ci._ip-func->_instructions)-1);
	}
	return NULL;
}

void sq_pushobject(HSQUIRRELVM v,HSQOBJECT obj)
{
	v->Push(SQObjectPtr(obj));
}

void sq_resetobject(HSQOBJECT *po)
{
	po->_unVal.pUserPointer=NULL;po->_type=OT_NULL;
}

SQRESULT sq_throwerror(HSQUIRRELVM v,const SQChar *err, ...)
{
	va_list vl;
	va_start(vl, err);
	v->Raise_ErrorV(err, vl);
	va_end(vl);
	return SQ_ERROR;
}

SQUIRREL_API SQRESULT sq_throwobj(HSQUIRRELVM v,HSQOBJECT obj)
{
	SQObjectPtr o(obj);
	v->Raise_Error(o);
	return SQ_ERROR;
}

void sq_reseterror(HSQUIRRELVM v)
{
	v->_lasterror = _null_;
}

void sq_getlasterror(HSQUIRRELVM v)
{
	v->Push(v->_lasterror);
}

void sq_reservestack(HSQUIRRELVM v,SQInteger nsize)
{
	if (((SQUnsignedInteger)v->_top + nsize) > v->_stack.size()) {
		v->_stack.resize(v->_stack.size() + ((v->_top + nsize) - v->_stack.size()));
	}
}

SQRESULT sq_resume(HSQUIRRELVM v,SQBool retval,SQBool raiseerror)
{
	if(sqtype(v->GetUp(-1))==OT_GENERATOR){
		v->Push(_null_); //retval
		if(!v->Execute(v->GetUp(-2),v->_top,0,v->_top,v->GetUp(-1),raiseerror,SQVM::ET_RESUME_GENERATOR))
		{return SQ_ERROR;}
		if(!retval)
			v->Pop();
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("only generators can be resumed"));
}

SQRESULT sq_call(HSQUIRRELVM v,SQInteger params,SQBool retval,SQBool raiseerror)
{
	SQObjectPtr res;
	if(v->Call(v->GetUp(-(params+1)),params,v->_top-params,res,raiseerror?true:false)){
		if(!v->_suspended) {
		v->Pop(params);//pop closure and args
		}
		if(retval){
			v->Push(res); return SQ_OK;
		}
		return SQ_OK;
	}
	else {
		v->Pop(params);
		return SQ_ERROR;
	}
	if(!v->_suspended)
		v->Pop(params);
	return sq_throwerror(v,_SC("call failed"));
}

SQRESULT sq_suspendvm(HSQUIRRELVM v)
{
	return v->Suspend();
}

SQRESULT sq_wakeupvm(HSQUIRRELVM v,SQBool wakeupret,SQBool retval,SQBool raiseerror,SQBool throwerror)
{
	SQObjectPtr ret;
	if(!v->_suspended)
		return sq_throwerror(v,_SC("cannot resume a vm that is not running any code"));
	if(wakeupret) {
		v->GetAt(v->_stackbase+v->_suspended_target)=v->GetUp(-1); //retval
		v->Pop();
	} else v->GetAt(v->_stackbase+v->_suspended_target)=_null_;
	if(!v->Execute(_null_,v->_top,-1,-1,ret,raiseerror,throwerror?SQVM::ET_RESUME_THROW_VM : SQVM::ET_RESUME_VM))
		return SQ_ERROR;
	if(sq_getvmstate(v) == SQ_VMSTATE_IDLE) {
		while (v->_top > 1) v->_stack[--v->_top] = _null_;
	}
	if(retval)
		v->Push(ret);
	return SQ_OK;
}

void sq_setreleasehook(HSQUIRRELVM v,SQInteger idx,SQRELEASEHOOK hook)
{
	if(sq_gettop(v) >= 1){
		SQObjectPtr &ud=stack_get(v,idx);
		switch( sqtype(ud) ) {
		case OT_USERDATA:	_userdata(ud)->_hook = hook;	break;
		case OT_INSTANCE:	_instance(ud)->_hook = hook;	break;
		case OT_CLASS:		_class(ud)->_hook = hook;		break;
		default: break; //shutup compiler
		}
	}
}

void sq_setcompilererrorhandler(HSQUIRRELVM v,SQCOMPILERERROR f)
{
	_ss(v)->_compilererrorhandler = f;
}

SQRESULT sq_writeclosure(HSQUIRRELVM v,SQWRITEFUNC w,SQUserPointer up, SQInteger endian)
{
#ifdef __BIG_ENDIAN__
	bool reverseByte = (endian == SQ_LITTLE_ENDIAN);
#else
	bool reverseByte = (endian == SQ_BIG_ENDIAN);
#endif
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, -1, OT_CLOSURE,o);
	unsigned short tag = SQ_BYTECODE_STREAM_TAG;
	if(w(up,&tag,2) != 2)
		return sq_throwerror(v,_SC("io error"));
	if(!_closure(*o)->Save(v,up,w,reverseByte))
		return SQ_ERROR;
	return SQ_OK;
}

SQRESULT sq_readclosure(HSQUIRRELVM v,SQREADFUNC r,SQUserPointer up)
{
	SQObjectPtr closure;
	
	unsigned short tag;
	if(r(up,&tag,2) != 2)
		return sq_throwerror(v,_SC("io error"));
	if(tag != SQ_BYTECODE_STREAM_TAG)
		return sq_throwerror(v,_SC("invalid stream"));
	if(!SQClosure::Load(v,up,r,closure))
		return SQ_ERROR;
	v->Push(closure);
	return SQ_OK;
}

SQChar *sq_getscratchpad(HSQUIRRELVM v,SQInteger minsize)
{
	return _ss(v)->GetScratchPad(minsize);
}

SQInteger sq_collectgarbage(HSQUIRRELVM v)
{
#ifndef NO_GARBAGE_COLLECTOR
	return _ss(v)->CollectGarbage(v);
#else
	return -1;
#endif
}

const SQChar *sq_getfreevariable(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger nval)
{
	SQObjectPtr &self = stack_get(v,idx);
	const SQChar *name = NULL;
	if(sqtype(self) == OT_CLOSURE) {
		if(_closure(self)->_outervalues.size()>nval) {
			v->Push(_closure(self)->_outervalues[nval]);
			SQFunctionProto *fp = _funcproto(_closure(self)->_function);
			SQOuterVar &ov = fp->_outervalues[nval];
			name = _stringval(ov._name);
		}
	}
	return name;
}

SQRESULT sq_setfreevariable(HSQUIRRELVM v,SQInteger idx,SQUnsignedInteger nval)
{
	SQObjectPtr &self=stack_get(v,idx);
	switch(sqtype(self))
	{
	case OT_CLOSURE:
		if(_closure(self)->_outervalues.size()>nval){
			_closure(self)->_outervalues[nval]=stack_get(v,-1);
		}
		else return sq_throwerror(v,_SC("invalid free var index"));
		break;
	case OT_NATIVECLOSURE:
		if(_nativeclosure(self)->_outervalues.size()>nval){
			_nativeclosure(self)->_outervalues[nval]=stack_get(v,-1);
		}
		else return sq_throwerror(v,_SC("invalid free var index"));
		break;
	default:
		return sq_aux_invalidtype(v,sqtype(self));
	}
	v->Pop(1);
	return SQ_OK;
}

SQRESULT sq_setattributes(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, idx, OT_CLASS,o);
	SQObjectPtr &key = stack_get(v,-2);
	SQObjectPtr &val = stack_get(v,-1);
	SQObjectPtr attrs;
	if(sqtype(key) == OT_NULL) {
		attrs = _class(*o)->_attributes;
		_class(*o)->_attributes = val;
		v->Pop(2);
		v->Push(attrs);
		return SQ_OK;
	}else if(_class(*o)->GetAttributes(key,attrs)) {
		_class(*o)->SetAttributes(key,val);
		v->Pop(2);
		v->Push(attrs);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("wrong index"));
}

SQRESULT sq_getattributes(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, idx, OT_CLASS,o);
	SQObjectPtr &key = stack_get(v,-1);
	SQObjectPtr attrs;
	if(sqtype(key) == OT_NULL) {
		attrs = _class(*o)->_attributes;
		v->Pop();
		v->Push(attrs); 
		return SQ_OK;
	}
	else if(_class(*o)->GetAttributes(key,attrs)) {
		v->Pop();
		v->Push(attrs);
		return SQ_OK;
	}
	return sq_throwerror(v,_SC("wrong index"));
}

SQRESULT sq_getbase(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, idx, OT_CLASS,o);
	if(_class(*o)->_base)
		v->Push(SQObjectPtr(_class(*o)->_base));
	else
		v->Push(_null_);
	return SQ_OK;
}

SQRESULT sq_getclass(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, idx, OT_INSTANCE,o);
	v->Push(SQObjectPtr(_instance(*o)->_class));
	return SQ_OK;
}

SQRESULT sq_createinstance(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr *o = NULL;
	_GETSAFE_OBJ(v, idx, OT_CLASS,o);
	v->Push(_class(*o)->CreateInstance());
	return SQ_OK;
}

void sq_weakref(HSQUIRRELVM v,SQInteger idx)
{
	SQObject &o=stack_get(v,idx);
	if(ISREFCOUNTED(sqtype(o))) {
		v->Push(_refcounted(o)->GetWeakRef(sqtype(o)));
		return;
	}
	v->Push(o);
}

SQRESULT sq_getweakrefval(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr &o = stack_get(v,idx);
	if(sqtype(o) != OT_WEAKREF) {
		return sq_throwerror(v,_SC("the object must be a weakref"));
	}
	v->Push(_weakref(o)->_obj);
	return SQ_OK;
}

SQRESULT sq_getdefaultdelegate(HSQUIRRELVM v,SQObjectType t)
{
	SQSharedState *ss = _ss(v);
	switch(t) {
	case OT_TABLE: v->Push(ss->_table_default_delegate); break;
	case OT_ARRAY: v->Push(ss->_array_default_delegate); break;
	case OT_STRING: v->Push(ss->_string_default_delegate); break;
	case OT_INTEGER: case OT_FLOAT: v->Push(ss->_number_default_delegate); break;
	case OT_GENERATOR: v->Push(ss->_generator_default_delegate); break;
	case OT_CLOSURE: case OT_NATIVECLOSURE: v->Push(ss->_closure_default_delegate); break;
	case OT_THREAD: v->Push(ss->_thread_default_delegate); break;
	case OT_CLASS: v->Push(ss->_class_default_delegate); break;
	case OT_INSTANCE: v->Push(ss->_instance_default_delegate); break;
	case OT_WEAKREF: v->Push(ss->_weakref_default_delegate); break;
	default: return sq_throwerror(v,_SC("the type doesn't have a default delegate"));
	}
	return SQ_OK;
}

SQRESULT sq_next(HSQUIRRELVM v,SQInteger idx)
{
	SQObjectPtr o=stack_get(v,idx),&refpos = stack_get(v,-1),realkey,val;
	if(sqtype(o) == OT_GENERATOR) {
		return sq_throwerror(v,_SC("cannot iterate a generator"));
	}
	int faketojump;
	if(!v->FOREACH_OP(o,realkey,val,refpos,0,666,faketojump))
		return SQ_ERROR;
	if(faketojump != 666) {
		v->Push(realkey);
		v->Push(val);
		return SQ_OK;
	}
	return SQ_ERROR;
}

struct BufState{
	const SQChar *buf;
	SQInteger ptr;
	SQInteger size;
};

SQInteger buf_lexfeed(SQUserPointer file)
{
	BufState *buf=(BufState*)file;
	if(buf->size<(buf->ptr+1))
		return 0;
	return buf->buf[buf->ptr++];
}

SQRESULT sq_compilebuffer(HSQUIRRELVM v,const SQChar *s,SQInteger size,const SQChar *sourcename,SQBool raiseerror) {
	BufState buf;
	buf.buf = s;
	buf.size = size;
	buf.ptr = 0;
	return sq_compile(v, buf_lexfeed, &buf, sourcename, raiseerror);
}

void sq_move(HSQUIRRELVM dest,HSQUIRRELVM src,SQInteger idx)
{
	dest->Push(stack_get(src,idx));
}

void sq_setprintfunc(HSQUIRRELVM v, SQPRINTFUNCTION printfunc)
{
	_ss(v)->_printfunc = printfunc;
}

SQPRINTFUNCTION sq_getprintfunc(HSQUIRRELVM v)
{
	return _ss(v)->_printfunc;
}

void sq_setprinterrfunc(HSQUIRRELVM v, SQPRINTFUNCTION printfunc)
{
	_ss(v)->_printerrfunc = printfunc;
}

SQPRINTFUNCTION sq_getprinterrfunc(HSQUIRRELVM v)
{
	return _ss(v)->_printerrfunc;
}

void *sq_malloc(SQUnsignedInteger size)
{
	return SQ_MALLOC(size);
}

void *sq_realloc(void* p,SQUnsignedInteger oldsize,SQUnsignedInteger newsize)
{
	return SQ_REALLOC(p,oldsize,newsize);
}

void sq_free(void *p,SQUnsignedInteger size)
{
	SQ_FREE(p,size);
}

/*
 * 直接スクリプト実行
 * @param v VM
 * @param src スクリプトデータ
 * @param size スクリプト長さ
 * @param contextIdx 実行コンテキストのスタック位置(0だとなし)
 * @param errorIdx エラー値のスタック位置(0だとなし)
 * 
 * 結果をスタックに積みます
 */
SQRESULT sq_execscript(HSQUIRRELVM v, const SQChar *src, SQInteger size, SQInteger contextIdx, SQInteger errorIdx)
{
	const SQChar *name=_SC("execscript");
	SQRESULT ret = sq_compilebuffer(v,src,size,name,SQTrue);
	if (SQ_SUCCEEDED(ret)) {
		// この時点でfunc がスタックにある
		if (contextIdx) {
			// context をbindしてから実行
			sq_push(v, contextIdx);
			ret = sq_bindenv(v, -2);
			if (SQ_SUCCEEDED(ret)) {
				sq_push(v,1); // this
				ret = sq_call(v, 1, SQTrue, SQTrue);
			} else {
				sq_pop(v,1);
			}
		} else {
			sq_push(v,1); // this
			ret = sq_call(v, 1, SQTrue, SQTrue);
		}
	}
	if (SQ_SUCCEEDED(ret)){
		// 結果が入ってる
		return 1;
	}
	if (errorIdx) {
		// error を返す
		sq_push(v,errorIdx);
		return 1;
	}
	// エラー
	return ret;

}


static bool isIdentFirst(SQChar ch) {
	return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static bool isIdent(SQChar ch) {
	return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9');
}

/*
 * 文字列テンプレート処理
 * @param v VM
 * @param text 変数テンプレート文字列インデックス $$ $識別子 ${式} をうめこむことができる
 * @param contextidx 評価コンテキストインデックス(0)だと現在のコンテキストで処理される
 * @param errorIdx 式が評価エラーの場合にうめこむ値のインデックス。デフォルトは空文字
 *
 * 結果をスタックに積みます
 */
SQRESULT sq_template(HSQUIRRELVM v,const SQChar *text, SQInteger size, SQInteger contextidx, SQInteger errorIdx)
{
	SQPRINTFUNCTION printerr = sq_getprinterrfunc(v);
	SQInteger top = sq_gettop(v);
	const SQChar *EMPTY = _SC("");

	// eval中でつかわれてしまう可能性があるので処理の最中はスクラッチパッドは使えない
	SQInteger allocated = (scstrlen(text) + 1) * sizeof(SQChar);
	SQChar *dest = (SQChar*)SQ_MALLOC(allocated);

	SQChar exp[200+1+7];	
	SQInteger maxexp = 207;
	SQInteger explen = 7;
	scstrcpy(exp, _SC("return "));

	SQInteger n = 0;
	SQInteger i = 0;
	while (n < size && text[n] != '\0') {

		if (text[n] != '$') {
			assert(i < allocated);
			dest[i++] = text[n];
			n++;
			continue;
		}

		if (text[n+1] == '$') { // $$ を処理
			dest[i++] = '$';
			n += 2; 
			continue;
		}

		explen = 7;

		if (n+1 < size && isIdentFirst(text[n+1])) { // $識別子
			SQChar *p = &exp[explen];
			n++;
			while (n < size && isIdent(text[n])) {
				if (explen <= maxexp) {
					*p++ = text[n++];
				} else {
					n++;
				}
				explen++;
			}
			*p = '\0';
		}
		else if (n+1 < size && text[n+1] == '{') {  // ${式}
			SQChar *p = &exp[explen];
			n += 2;
			while (n < size && text[n] != '\0') {
			    if (text[n] == '}') {
					n++;
					goto end;
				} else {
					if (explen <= maxexp) {
						*p++ = text[n++];
					} else {
						n++;
					}
					explen++;
				}
			}
		end:
			*p = '\0';
		} else {
			dest[i++] = '$';
			n += 1;
			continue;
		} 

		if (explen > maxexp) {
			if (printerr) {
				printerr(v, _SC("too long expression:%s\n"), maxexp, exp+7);
			}
			if (errorIdx) {
				sq_push(v, errorIdx);
			} else {
				sq_pushstring(v, EMPTY, 0);
			}
		} else if (explen > 7) {

			SQRESULT ret = sq_execscript(v, exp, explen, contextidx, 0);
			if (SQ_FAILED(ret)) {
				// エラー表示
				if (printerr) {
					sq_getlasterror(v);
					const SQChar *error; 
					sq_getstring(v, -1, &error);
					if (error) {
						printerr(v, error);
					}
					else {
						printerr(v, _SC("null error"));
					}
					sq_settop(v, top);
				}
				if (errorIdx) {
					sq_push(v, errorIdx);
				} else {
					sq_pushstring(v, EMPTY, 0);
				}
			} else if (ret == 0) {
				sq_pushstring(v, EMPTY, 0);
			}
		} else {
			sq_pushstring(v, EMPTY, 0);
		}

		{
			sq_tostring(v, -1);
			const SQChar *data;
			sq_getstring(v, -1, &data);
			SQInteger len = sq_getsize(v, -1);
			SQInteger add = len * sizeof(SQChar); 
			dest = (SQChar*)SQ_REALLOC(dest, allocated, allocated + add);
			allocated += add;
			scstrncpy(&dest[i],data,len);
			sq_settop(v, top);
			i += len;
		}
	}

	sq_settop(v,top);
	sq_pushstring(v, dest, i);
	SQ_FREE(dest, allocated);
	return 1;
}