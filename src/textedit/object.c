/*
** $Id: object.c 13411 2010-09-01 07:37:37Z wanzheng $
**
** object.c: The implemenation of mObject class for the new textedit module.
**
** Copyright (C) 2010 Feynman Software
**
** All rights reserved by Feynman Software.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "common.h"
#include "minigui.h"

#include "object.h"

#if defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL) || defined(__MGNCS_TEXTEDITOR)

static void mObject_construct(mObject* self, DWORD addData)
{
	//do nothing
	//to avoid NULL pointer
}

static void mObject_destroy(mObject* self)
{
/*	ncsRemoveObjectBindProps(self);
	ncsRemoveEventSource(self);
	ncsRemoveEventListener(self);
*/	
}

static DWORD mObject_hash(mObject *self)
{
	return (DWORD)self;
}

static const char* mObject_toString(mObject *self, char* str, int max)
{
	if(!str)
		return NULL;

	snprintf(str, max, "NCS Object %s[@%p]", TYPENAME(self),self);
	return str;
}


static mObjectClass* mObjectClassConstructor(mObjectClass* _class)
{
	_class->super = NULL;
	_class->typeName = "mObject";
	_class->objSize = sizeof(mObject);

	CLASS_METHOD_MAP(mObject, construct)
	CLASS_METHOD_MAP(mObject, destroy)
	CLASS_METHOD_MAP(mObject, hash)
	CLASS_METHOD_MAP(mObject, toString)
	return _class;
}

mObjectClass Class(mObject) = {
	(PClassConstructor)mObjectClassConstructor
};

mObject * mg_newObject(mObjectClass *_class)
{
	mObject * obj;

	if(_class == NULL)
		return NULL;

	obj = (mObject*)calloc(1, _class->objSize);

	if(!obj)
		return NULL;

	return initObject(obj, _class);

}

mObject * mgNewObject(mObjectClass *_class,DWORD addData){
	mObject * obj = newObject(_class);
	if(!obj)
		return NULL;

	_class->construct(obj, addData);
	return obj;
}

mObject * mgNewObjectArgs(mObjectClass* _class, ...)
{
	va_list va;
	mObject * obj;
	va_start(va, _class);
	obj = newObject(_class);
	if(obj)
		((PCONSTRUCT_VALIST)(_class->construct))(obj, va);
	va_end(va);
	return obj;
}


void mgDeleteObject(mObject *obj)
{
	if(obj == NULL || obj->_class == NULL)
		return;

	_c(obj)->destroy(obj);

	free(obj);
}

mObject* mgInitObject(mObject* pobj, mObjectClass* _class) {
	IInterface* piobj;
	IInterfaceVTable* _ivtable;
	int next_intf_offset ;
	pobj->_class = _class;

	next_intf_offset = _class->intfOffset;
	while(next_intf_offset > 0)
	{
		_ivtable = (IInterfaceVTable*)((unsigned char*)_class + next_intf_offset);
		piobj = (IInterface*)((unsigned char*)pobj + _ivtable->_obj_offset);
		piobj->_vtable = _ivtable;	
		next_intf_offset = _ivtable->_next_offset;
	}

	return pobj;
}

mObject* mg_initObject(mObject* pobj, mObjectClass* _class,DWORD param) 
{
	memset(pobj, 0, _class->objSize);
	initObject(pobj, _class);
	_class->construct(pobj, param);
	return pobj;
}


mObject* mgInitObjectArgs(mObject* pobj, mObjectClass* _class, ...)
{
	va_list va;
	va_start(va, _class);
	memset(pobj, 0, _class->objSize);
	initObject(pobj, _class);
	((PCONSTRUCT_VALIST)(_class->construct))(pobj, va);
	va_end(va);
	return pobj;
}


/////////////////////////////////////////////////////
//
//

mObjectClass *ncsSafeCastClass(mObjectClass* clss, mObjectClass* castCls)
{
	mObjectClass * clssSuper;
	if(clss == castCls)
		return NULL;

	clssSuper = clss;
	while(clssSuper && clssSuper != castCls)
		clssSuper = clssSuper->super;

	return clssSuper?clss:NULL;
}

mObject* ncsSafeCast(mObject* obj, mObjectClass *clss)
{
	mObjectClass * objClass;
	if(obj == NULL || clss == NULL)
		return NULL;

	objClass = _c(obj);

	while(objClass && objClass != clss)
		objClass = objClass->super;

	return objClass?obj:NULL;
}

BOOL ncsInstanceOf(mObject *object, mObjectClass* clss)
{
	mObjectClass* objClss;
	if(object == NULL || clss == NULL)
		return FALSE;

	objClss = _c(object);

	while(objClss && clss != objClss){
		objClss = objClss->super;
	}

	return objClss != NULL;
}


int ncsParseConstructParams(va_list args, const char* signature, ...)
{
	va_list params;
	int argc;
	int i;

	if(GET_ARG_COUNT(args) <= 0)
		return 0;

	argc = va_arg(args, int);
    if(argc <= 0)
        return 0;

	va_start(params, signature);

	i = 0;
	while(i < argc)
	{
		switch(signature[i])
		{
		case 'd': //double
			*(va_arg(params, double*)) = va_arg(args, double);
			break;
		case 'f': //float

/* FIXME gcc says:
 *  warning: 'float' is promoted to 'double' when passed through '...'
 *  warning: (so you should pass 'double' not 'float' to 'va_arg')
 *  note: if this code is reached, the program will abort.
 */
			*(va_arg(params, float*)) = va_arg(args, double);
			break;
		case 'i': //integer
			*(va_arg(params, int*)) = va_arg(args, int);
			break;
		case 's': //const char*
			*(va_arg(params, const char**)) = va_arg(args, const char*);
			break;
		case 'u': //unsigned int
			*(va_arg(params, unsigned int*)) = va_arg(args, unsigned int);
			break;
		case 'p': //void *
			*(va_arg(params, void**)) = va_arg(args, void*);
			break;
		default:
			*(va_arg(params, void**)) = va_arg(args, void*);
		}
		
		i ++;
	}

	va_end(params);

	return i;
}

#endif /* defined(_MGCTRL_TEXTEDIT_USE_NEW_IMPL) || defined(__MGNCS_TEXTEDITOR) */
