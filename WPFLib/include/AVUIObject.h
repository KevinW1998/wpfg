#pragma once

#include <cstddef>

namespace AVUI {

class WeakReference;
class Type;

class Object
{
    friend class WeakReference;
public:

    Object();
    virtual ~Object();

    inline int AddRef()
    {
        return ++m_cRef;
    }

    inline int Release()
    {
        int cRef = --m_cRef;

        if(cRef == 0)
        {
            // Explicitly invalidate weak references while the object is still valid
            InvalidateWeakReferences();

            delete this;
        }

        return cRef;
    }

    int get_RefCount() { return m_cRef; };

    virtual bool Equals(const Object* pObject) const;
    virtual const char* ToString() const;

    virtual const Type& get_Type() const { return get_StaticType(); };
    virtual bool IsType(const Type& rhs) const;
    static const Type& get_StaticType();

    static bool Equals(const Object* pObject1, const Object* pObject2);

    void* operator new (size_t size); 
    void operator delete (void *p);

    void* operator new[] (size_t size); 
    void operator delete[] (void *p);

    static void* AllocateMemory(size_t size);
    static void FreeMemory(void* p);

    void InvalidateWeakReferences();

private:

    void AddWeakReference(WeakReference* pWeakReference);
    void RemoveWeakReference(WeakReference* pWeakReference);

    int m_cRef;
    WeakReference* m_pWeakReferenceFirst;

};

};
