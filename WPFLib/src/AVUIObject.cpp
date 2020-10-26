#include <AVUICommon.h>
#include <AVUIObject.h>
#include <AVUIApplication.h>
#include <stdlib.h>

namespace AVUI {

    const Type& get_Type(Object *) { return Object::get_StaticType(); } 
const AVUI::Type& Object::get_StaticType() 
{ 
    static AVUI::Type s_type("Object"); 
    return s_type;
} 

bool Object::IsType(const Type& rhs) const { return get_StaticType().IsTypeOrBase(rhs); } 

Object::Object() : m_cRef(1), m_pWeakReferenceFirst(NULL)
{

}


Object::~Object()
{
}

bool Object::Equals(const Object* pObject1, const Object* pObject2)
{
    if(pObject1 == NULL && pObject2 == NULL)
    {
        return true;
    }

    if(pObject1 == NULL || pObject2 == NULL)
    {
        return false;
    }

    if(pObject1 != NULL)
    {
        return pObject1->Equals(pObject2);
    }

    return false;
}

bool Object::Equals(const Object* pObject) const
{
    return this==pObject;
}

const char* Object::ToString() const
{
    return get_Type().ToString();
}



void Object::AddWeakReference(WeakReference* pWeakReference)
{
    pWeakReference->m_pWeakReferenceNext = m_pWeakReferenceFirst;
    pWeakReference->m_pWeakReferencePrevious = NULL;

    if(m_pWeakReferenceFirst != NULL)
    {
        m_pWeakReferenceFirst->m_pWeakReferencePrevious = pWeakReference;
    }
    m_pWeakReferenceFirst = pWeakReference;
}

void Object::RemoveWeakReference(WeakReference* pWeakReference)
{
    if(pWeakReference->m_pWeakReferencePrevious == NULL)
    {
        if(pWeakReference != m_pWeakReferenceFirst)
        {
            LibraryCriticalError();
        }
        m_pWeakReferenceFirst = pWeakReference->m_pWeakReferenceNext;
    }
    else
    {
        pWeakReference->m_pWeakReferencePrevious->m_pWeakReferenceNext = pWeakReference->m_pWeakReferenceNext;
    }

    if(pWeakReference->m_pWeakReferenceNext != NULL)
    {
        pWeakReference->m_pWeakReferenceNext->m_pWeakReferencePrevious = pWeakReference->m_pWeakReferencePrevious;
    }

    pWeakReference->m_pWeakReferencePrevious = pWeakReference->m_pWeakReferenceNext = NULL;
 }

void Object::InvalidateWeakReferences()
{
    WeakReference* pWeakReferenceCur = m_pWeakReferenceFirst;

    while(pWeakReferenceCur != NULL)
    {
        WeakReference* pWeakReferenceNext = pWeakReferenceCur->m_pWeakReferenceNext;

        pWeakReferenceCur->InvalidateTarget();
        pWeakReferenceCur = pWeakReferenceNext;
    }

    m_pWeakReferenceFirst = NULL;
}



void* Object::operator new (size_t size)
{
    return AllocateMemory(size);
}

void Object::operator delete (void *p)
{
    FreeMemory(p);
}

void* Object::operator new[] (size_t size)
{
    return AllocateMemory(size);
}

void Object::operator delete[] (void *p)
{
    FreeMemory(p);
}


void* Object::AllocateMemory(size_t size)
{
    if(Application::GetAllocFunc() != NULL)
    {
        return Application::GetAllocFunc()(size);
    }
    else
    {
        return malloc(size);
    }
}

void Object::FreeMemory(void* p)
{
    if(Application::GetFreeFunc() != NULL)
    {
        Application::GetFreeFunc()(p);
    }
    else
    {
        free(p);
    }
}

}; // namespace AVUI
