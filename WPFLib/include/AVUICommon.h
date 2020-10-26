#pragma once

#pragma warning(disable:4512)
#pragma warning(disable:4100)
#pragma warning(disable:4355)

#define VERBOSE_ERRORS 1

typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character

namespace AVUI {

void GlobalError(const char* szFile, int line, const char* szMessage, ...);

#ifdef VERBOSE_ERRORS

#define LibraryCriticalError() GlobalError(__FILE__, __LINE__, "");
#define LibraryCriticalErrorMessage(message, ...) GlobalError(__FILE__, __LINE__, message, __VA_ARGS__);

#else

#define LibraryCriticalError() GlobalError("", -1, "");
#define LibraryCriticalErrorMessage(message) GlobalError("", -1, "");

#endif

class Type;


#define NULL 0
#define GET_TYPE(type) AVUI::get_Type( (type *) 0)

template<class T>
const AVUI::Type& get_Type(T*);

};

#include <AVUIObject.h>
#include <AVUIObjectPtr.h>
#include <AVUITypeRegistration.h>
#include <AVUIType.h>

namespace AVUI {

template<class T>
TRefCountedPtr<T> object_allocate()
{
    TRefCountedPtr<T> pT(new T(), false);

    return pT;
}

template<class T, class U>
TRefCountedPtr<T> object_allocate(const U& u)
{
    TRefCountedPtr<T> pT(new T(u), false);

    return pT;
}

template<class T, class U, class V>
TRefCountedPtr<T> object_allocate(const U& u, const V& v)
{
    TRefCountedPtr<T> pT(new T(u, v), false);

    return pT;
}

template<class T, class U, class V, class W>
TRefCountedPtr<T> object_allocate(const U& u, const V& v, const W& w)
{
    TRefCountedPtr<T> pT(new T(u, v, w), false);

    return pT;
}

template<class T, class U, class V, class W, class X>
TRefCountedPtr<T> object_allocate(const U& u, const V& v, const W& w, const X& x)
{
    TRefCountedPtr<T> pT(new T(u, v, w, x), false);

    return pT;
}

template<class T, class U, class V, class W, class X, class Y>
TRefCountedPtr<T> object_allocate(const U& u, const V& v, const W& w, const X& x, const Y& y)
{
    TRefCountedPtr<T> pT(new T(u, v, w, x, y), false);

    return pT;
}

template<class T, class U, class V, class W, class X, class Y, class Z>
TRefCountedPtr<T> object_allocate(const U& u, const V& v, const W& w, const X& x, const Y& y, const Z& z)
{
    TRefCountedPtr<T> pT(new T(u, v, w, x, y, z), false);

    return pT;
}


template<class T>
T* object_cast(Object* pObject)
{
    if(pObject == NULL)
    {
        return NULL;
    }


    const Type& type = AVUI::get_Type((T*) 0);
    if(!pObject->IsType(type) )
    {
        return NULL;
    }
    return reinterpret_cast<T*>(pObject);
}

template<class T>
const T* object_cast(const Object* pObject) 
{
    if(pObject == NULL)
    {
        return NULL;
    }

    const Type& type = AVUI::get_Type((T*) 0);
    if(!pObject->IsType(type) )
    {
        return NULL;
    }

    return reinterpret_cast<const T*>(pObject);
}

template<class T>
T* interface_cast(Object* pObject)
{
    if(pObject == NULL)
    {
        return NULL;
    }


    const Type& interfaceType = AVUI::get_Type((T*) 0);
    const Type& objectType = pObject->get_Type();
    if(!objectType.HasInterface(interfaceType))
    {
        return NULL;
    }
    return reinterpret_cast<T*>(objectType.CastToInterface(interfaceType, pObject));
}

template<class T>
const T* interface_cast(const Object* pObject) 
{
    if(pObject == NULL)
    {
        return NULL;
    }


    const Type& interfaceType = get_Type((T*) 0);
    const Type& objectType = pObject->get_Type();
    if(!objectType.HasInterface(interfaceType))
    {
        return NULL;
    }
    return reinterpret_cast<const T*>(objectType.CastToInterface(interfaceType, pObject));
}

template<class T>
bool is_type(Object* pObject)
{
    if(pObject == NULL)
    {
        return false;
    }

    return pObject->IsType(get_Type((T*) 0));
}

};

#ifdef _WIN32_WCE
#define acosf(x)    ((float)acos((double)(x)))
#define asinf(x)    ((float)asin((double)(x)))
#define atanf(x)    ((float)atan((double)(x)))
#define atan2f(x,y) ((float)atan2((double)(x), (double)(y)))
#define ceilf(x)    ((float)ceil((double)(x)))
#define cosf(x)     ((float)cos((double)(x)))
#define coshf(x)    ((float)cosh((double)(x)))
#define expf(x)     ((float)exp((double)(x)))
#define fabsf(x)    ((float)fabs((double)(x)))
#define floorf(x)   ((float)floor((double)(x)))
#define fmodf(x,y)  ((float)fmod((double)(x), (double)(y)))
#define frexpf(x,y) ((float)frexp((double)(x), (y)))
#define hypotf(x,y) ((float)hypot((double)(x), (double)(y)))
#define ldexpf(x,y) ((float)ldexp((double)(x), (y)))
#define logf(x)     ((float)log((double)(x)))
#define log10f(x)   ((float)log10((double)(x)))
#define _matherrf   _matherr
#define modff(x,y)  ((float)modf((double)(x), (double *)(y)))
#define powf(x,y)   ((float)pow((double)(x), (double)(y)))
#define sinf(x)     ((float)sin((double)(x)))
#define sinhf(x)    ((float)sinh((double)(x)))
#define sqrtf(x)    ((float)sqrt((double)(x)))
#define tanf(x)     ((float)tan((double)(x)))
#define tanhf(x)    ((float)tanh((double)(x)))
#endif


#include <AVUIBoxedValueType.h>

#include <AVUIGenEnums.h>

#include <AVUIEvent.h>
#include <AVUIPredicate.h>
#include <AVUIRoutedEventHandler.h>
#include <AVUIRoutedEvent.h>
#include <AVUIRoutedEventArgs.h>
#include <AVUIEventRoute.h>
#include <AVUIRoutedCommand.h>

#include <AVUIArray.h>
#include <AVUINullable.h>


// This should likely be broken out into simple properties.
#include <AVUIDependencyProperty.h>
#include <AVUIBuiltinTypes.h>

#include <AVUIVector.h>
#include <AVUISize.h>
#include <AVUIPoint.h>
#include <AVUIRect.h>

#include <AVUIPoint3D.h>
#include <AVUIVector3D.h>
#include <AVUIQuaternion.h>


#include <AVUITimeSpan.h>
#include <AVUIUIString.h>


