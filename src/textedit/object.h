/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
** object.h: This file include the basic object defines.
**
** Create date: 2010/03/10
*/

#ifndef _MGUI_NCSCTRL_OBJECT_H
#define _MGUI_NCSCTRL_OBJECT_H

#ifdef __cplusplus
extern "C"{
#endif

#ifdef __MINIGUI_LIB__
#   define ncsInstanceOf _minigui_ncsInstanceOf
#   define ncsParseConstructParams _minigui_ncsParseConstructParams
#   define ncsSafeCast _minigui_ncsSafeCast
#   define ncsSafeCastClass _minigui_ncsSafeCastClass
#endif

#include <stdarg.h>

#ifndef MGNCS_EXPORT
#define MGNCS_EXPORT MG_EXPORT
#endif

/**
 * \defgroup Object mObject
 * @{
 */

#define MGNCS_INIT_CLASS(clss) \
	Class(clss).classConstructor((mObjectClass*)(void*)(&(Class(clss))))
/**
 * \def _c
 * \brief the marco get the class of a object
 */
#define _c(ths)                 (ths)->_class
#define ClassType(className)    className##Class
#define Class(className)        g##className##Class
#define CLASS(className)        Class(className)
#define ObjectType(className)   className

#ifdef  WIN32
#define _TYPEOF(x)  void* //__typeof(x)
#else 
#define _TYPEOF(x)  typeof(x)
#endif

#define BEGIN_MINI_CLASS(clss, superCls) \
static ClassType(clss) * clss##ClassConstructor(ClassType(clss)* _class); \
ClassType(clss) Class(clss) = { (PClassConstructor)clss##ClassConstructor }; \
static const char* clss##_type_name = #clss; \
static ClassType(clss) * clss##ClassConstructor(ClassType(clss)* _class) { \
	unsigned short * _pintfOffset = NULL; \
	_pintfOffset = (PVOID)((UINT_PTR)_pintfOffset ^ 0); /* VW: prevent unused-but-set-variable warning */ \
	_class = (ClassType(clss)*)((PClassConstructor)(Class(superCls).classConstructor))((mObjectClass*)_class); \
	_pintfOffset = &_class->intfOffset; \
	_class->super = &Class(superCls); \
	_class->typeName = clss##_type_name; \
	_class->objSize = sizeof(clss);

#define END_MINI_CLASS return _class; }

#define CLASS_METHOD_MAP(clss, name)  \
        _class->name = (void*)(clss##_##name);
/*   do {
        void **__r = (void**)&_class->name; \
        *__r =  (void*)(clss##_##name);}while(0); */
/*   _class->name = (_TYPEOF(_class->name))(clss##_##name);*/


#define NEED_CHECK

#ifdef NEED_CHECK
#define CHECK_METHOD(cls, mth)  ((cls) && (cls)->mth)
#else
#define CHECK_METHOD(cls, mth)  (1)
#endif


#define PUBLIC
#define PRIVATE
#define PROTECTED
#define STATIC

#define _M(pobj, method, ...)  _c(pobj)->method((pobj), ##__VA_ARGS__)

#define DECLARE_CLASS(clss, clssSuper) \
	typedef struct _##clss##Class clss##Class; \
	typedef struct _##clss clss; \
	struct _##clss##Class{ \
		clss##ClassHeader(clss, clssSuper) \
	}; \
	struct _##clss { \
		clss##Header(clss) \
	}; \
	MGNCS_EXPORT extern clss##Class CLASS(clss);

#define _SUPER(super,self,method, ...)  Class(super).method((super*)(self), ##__VA_ARGS__)

#define DECLARE_OBJECT(Clss)  \
	typedef struct _##Clss##Class Clss##Class;  \
	typedef struct _##Clss Clss;  \
	MGNCS_EXPORT extern Clss##Class CLASS(Clss);

#define DEFINE_OBJECT(Clss, ClssSuper)  \
	struct _##Clss##Class { Clss##ClassHeader(Clss, ClssSuper) }; \
	struct _##Clss { Clss##Header(Clss) };

////////////////////// interface support
#define INTERFACE(Interface)  \
	Interface##VTable * Interface##_; /*Interface vtable*/

#define DECLARE_INTERFACE(Interface)  \
	typedef struct _##Interface##VTable Interface##VTable; \
	typedef struct _##Interface Interface;

#define DEFINE_INTERFACE(Interface)          \
	struct _##Interface##VTable {                             \
		Interface##Header(Interface, Interface)         \
	};                                                        \
	struct _##Interface { Interface##VTable *_vtable; };

#define IMPLEMENT(Clss,Interface) \
	_class->_##Interface##_obj_offset = (UINT_PTR)(void*)&(((Clss*)0)->Interface##_); \
	_class->_##Interface##_next_offset = 0;                                                \
	*_pintfOffset =  (unsigned short)(UINT_PTR)(void*)&(((ClassType(Clss)*)0)->_##Interface##_obj_offset); \
	_pintfOffset = &_class->_##Interface##_next_offset;

#define INTERFACE_CAST(Interface, pobj)  \
	((Interface*)((unsigned char*)(pobj) + (_c(pobj)->_##Interface##_obj_offset)))

//the basic interface
DECLARE_INTERFACE(IInterface)
struct _IInterfaceVTable{
	unsigned short _obj_offset;
	unsigned short _next_offset;
};

struct _IInterface {
	IInterfaceVTable * _vtable;
};

#define IInterfaceHeader(Interface, ClssImpl)  \
	unsigned short _##Interface##_obj_offset; \
	unsigned short _##Interface##_next_offset; \

/**
 * \define INTERFACE_ADJUST
 * \breif adjust the pointer as the real pointer
 */
#define INTEFACE_ADJUST(piobj)  (_TYPEOF(piobj))((unsigned char*)(piobj) -  \
		((IInterfaceVTable*)((piobj))->_vtable)->_obj_offset)

#define INTERFACE2OBJECT(type, piobj)  (type*)((unsigned char*)(piobj) - ((IInterfaceVTable*)((piobj))->_vtable)->_obj_offset)

/**
 * \def _IM 
 * \brief call the interface object
 */
#define _IM(piobj, method, ...) \
	(piobj)->_vtable->method(INTEFACE_ADJUST(piobj), ##__VA_ARGS__)
#define _I(piobj)  (piobj)->_vtable




typedef struct _mObjectClass mObjectClass;
typedef struct _mObject mObject;

/**
 * \typedef mObjectClass* (*PClassConstructor)(mObjectClass * cls);
 * \brief the Class initialize callback
 *
 * \param cls - the class pointer to initialize
 *
 * \return mObjectClass* - the initialized class pointer, equal param cls
 */
typedef mObjectClass* (*PClassConstructor)(mObjectClass *);
typedef void (*PCONSTRUCT_VALIST)(mObject* self, va_list va);

#define mObjectClassHeader(clss, superCls) \
	PClassConstructor classConstructor; \
	ClassType(superCls) * super; \
	const char* typeName; \
	unsigned short objSize; \
	unsigned short intfOffset; /** define the interface offset*/ \
	void (*construct)(clss *self, DWORD addData); \
	void (*destroy)(clss *self); \
	DWORD (*hash)(clss *self); \
	const char* (*toString)(clss *self, char* str, int max);  \

/**
 * \struct mObjectClass
 * \brief the basic object class of NCS
 *
 *  - PClassConstruct \b classConstructor \n
 *    the class intialize callback
 *
 *  - mObjectClass * \b super \n
 *    the super class pointer
 *
 *  - const char* \b typeName \n
 *    the class name, for example, "Object"
 *
 *  - unsigned int \b objSize \n
 *    the size of corresponding object, used to create a object
 *
 *  - void \b construct(mObject *self, DWORD addData);\n
 *    the construct method of mObject\n
 *    \param self - this pointer
 *    \param addData - the initialize data, depend on derived class
 *  - void \b destroy (mObject* self);\n
 *    Destroy a mObject object\n
 *    \param self - this pointer
 *
 *
 * \sa mObject
 *
 */

struct _mObjectClass {
	mObjectClassHeader(mObject, mObject)
};

/**
 * \var g_stmObjectCls
 * \brief the singleton of mObjectClass, represent a class object
 */
MGNCS_EXPORT extern mObjectClass Class(mObject);

#define mObjectHeader(clss) \
	ClassType(clss) * _class; 

/**
 * \struct mObject
 * \brief the Object struct
 *
 *  - mObjectClass * \a _class  \n
 *   the class pointer of this Object
 *
 *  \sa mObjectClass
 */

struct _mObject {
	mObjectHeader(mObject)
};

MGNCS_EXPORT mObject* mgInitObject(mObject* pobj, mObjectClass* _class);
#define initObject mgInitObject

MGNCS_EXPORT mObject* mg_initObject(mObject* pobj, mObjectClass* _class,DWORD param);
#define _initObject mg_initObject

//first param is the construct param count
MGNCS_EXPORT mObject* mgInitObjectArgs(mObject* pobj, mObjectClass* _class, ...);
#define initObjectArgs mgInitObjectArgs


#define INIT_OBJEX(Clss, pobj, param)  ((Clss* )_initObject((mObject*)((void *)(pobj)), \
                (mObjectClass*)((void*)&(Class(Clss))), param))
#define INIT_OBJ(Clss, pobj)  INIT_OBJEX(Clss, pobj, 0)
#define INIT_OBJV(Clss, pobj, ...) ((Clss* )initObjectArgs((mObject*)((void*)(pobj)), \
                (mObjectClass*)((void*)(&(Class(Clss)))), ##__VA_ARGS__))

/*
** the implementation of GET_ARG_COUNT is bad, because some systems
** define va_list as a pointer, and others define it as an array of 
** pointers (of length 1).
static inline int MGGET_ARG_COUNT(va_list va)
{
	union {
		va_list va;
		DWORD   dva;
	} _va;

	_va.va = va;
	if(_va.dva == 0)
		return 0;
	//return va_arg(va, int); 
    //DON'T call va_arg, because, va_arg can change va's value
    //this is a inline function, in some compiler, the change would
    //be pass to its caller
    return 1;
}
#define GET_ARG_COUNT MGGET_ARG_COUNT
*/

#define UNIT_OBJ(pobj)  (_c(pobj)->destroy(pobj))


/** @} end of Object define */

/**
 * \addtogroup global_defines Global defines
 * @{
 */

 /**
  * \defgroup gobal_object_functions Global Object Functions
  * @{
  */

/**
* \fn mObject * newObject(mObjectClass *_class);
* \brief new a object instance, like \b new operator in C++
*
* \param _class - the class of the object
*
* \return the new pointer of object
*
*/
MGNCS_EXPORT mObject * mg_newObject(mObjectClass *_class);
#define newObject mg_newObject
MGNCS_EXPORT mObject * mgNewObject(mObjectClass *_class,DWORD addData);
#define ncsNewObject mgNewObject


#define NEWEX(classType, addData)  (classType*)ncsNewObject((mObjectClass*)(void *)(&Class(classType)), addData)
#define NEW(classType)  NEWEX(classType, 0)

MGNCS_EXPORT mObject * mgNewObjectArgs(mObjectClass* _class, ...);
#define ncsNewObjectArgs  mgNewObjectArgs

#define NEWV(Clss, ...)  ((Clss*)ncsNewObjectArgs((mObjectClass*)((void*)(&(Class(Clss)))), \
                ##__VA_ARGS__))

/**
 * \fn void deleteObject(mObject *obj);
 * \brief delete a object intance, like \b delete operator in C++
 *
 * \param obj - the object want to delete
 *
 */
MGNCS_EXPORT void mgDeleteObject(mObject *obj);
#define deleteObject mgDeleteObject
#define DELETE(obj)   deleteObject((mObject*)(obj))

/**
 * \def TYPENAME
 * \brief Get the class name form a Object pointer
 */
#define TYPENAME(obj)  ((obj)?(((obj)->_class)?((obj)->_class)->typeName:""):"")

///////////////////////

/**
 * \fn BOOL ncsInstanceOf(mObject* object, mObjectClass* clss);
 * \brief check an object is the class instance or not, same as \b instanceof operator in Java
 *
 * \param object - the pointer of object being to test
 * \param clss - the pointer of class for test
 *
 * \return TRUE - object is instance of clss, FALSE - not
 */
MGNCS_EXPORT BOOL ncsInstanceOf(mObject* object, mObjectClass* clss);

/**
 * \def INSTANCEOF
 * \brief A wrapper of \ref ncsInstanceOf, just like java's instanceof
 *
 * Example:
 * 	Test a object is mComponentClass:
 * 	INSTANCEOF(obj, mComponent)
 */
#define INSTANCEOF(obj, clss)  ncsInstanceOf((mObject*)(obj), (mObjectClass*)(void*)(&Class(clss)))

/**
 * \fn static inline mObject*  ncsIsValidObj(mObject* obj);
 * \brief Check a pointer is a valid mObject or not
 *
 * \param obj - the excpeted object pointer
 *
 * \return mObject * the pointer of obj or other NULL if obj is an invalid mObject pointer
 */
static inline mObject*  ncsIsValidObj(mObject* obj){
	return (INSTANCEOF(obj, mObject)?(obj):NULL);
}

/**
 * \def CHECKOBJ
 * \brief the wrapper of ncsIsValidObj
 *
 * \sa ncsIsValidObj
 */
#define CHECKOBJ(obj)  ncsIsValidObj((mObject*)obj)

/**
 * \fn mObject* ncsSafeCast(mObject* obj, mObjectClass *clss);
 * \brief safe type cast function, like the \b dynamic_cast operator in C++
 *
 * \param obj - the mObject pointer being casted
 * \param clss - the target type to cast
 *
 * \return mObject * - the object pointer if cast safe, NULL otherwise
 *
 * \sa ncsInstanceOf
 */
MGNCS_EXPORT mObject* ncsSafeCast(mObject* obj, mObjectClass *clss);

/**
 * \def TYPE_CAST
 * \brief unsafe cast. donot check the type of class
 */
#define TYPE_CAST(Type, obj)  ((Type*)(obj))
/**
 * \def SAFE_CAST
 * \brief wrapper of ncsSafeCast, check the class type before cast.
 *
 * \note this macro is same as \b dynamic_cast in C++
 *
 * \sa ncsSafeCast
 */
#define SAFE_CAST(Type, obj)  TYPE_CAST(Type, ncsSafeCast((mObject*)obj,(mObjectClass*)(void*)(&(Class(Type)))))

/**
 * \fn mObjectClass *ncsSafeCastClass(mObjectClass* clss, mObjectClass* castCls);
 * \brief cast from a supper class to derived class type safely
 *
 * \param clss - the supper type class pointer
 * \param castCls - the derived class type pointer
 *
 * \note use \ref SAFE_CAST_CLASS
 *
 * \return mObjectClass * - the pointer of clss if cast safe, otherwise NULL
 */
MGNCS_EXPORT mObjectClass *ncsSafeCastClass(mObjectClass* clss, mObjectClass* castCls);
/**
 * \def SAFE_CAST_CLASS
 * \brief the wrapper of ncsSafeCastClass
 */
#define SAFE_CAST_CLASS(Clss, ClssCast) ((ClassType(ClssCast)*)(ncsSafeCastClass((mObjectClass*)Clss, (mObjectClass*)(void*)(&Class(ClssCast)))))

int ncsParseConstructParams(va_list args, const char* signature, ...);


/** @} end of global_object_funtions */

/** @} endof global_defines */

#ifdef __cplusplus
}
#endif


#endif

