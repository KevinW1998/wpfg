#pragma once


#include <memory.h>
#include <AVUIEventableObject.h>
#include <AVUIObjectPtr.h>
#include <AVUIAnimatable.h>
#include <AVUIIEnumerable.h>

namespace AVUI {

class ICollection
{
public:

    virtual bool get_IsEmpty()             = 0;
    virtual void Add(Object* pObject)      = 0;
    virtual void Clear()                   = 0;
    virtual bool Contains(Object* pObject) = 0;
    virtual bool Remove(Object* pObject)   = 0;
    virtual void RemoveAt(int idx) = 0;
    virtual void Move(int oldIndex, int newIndex) = 0;
    virtual int get_Count() = 0;
    virtual TRefCountedPtr<Object> GetItemAt(int idx) = 0;
    virtual int IndexOf(Object* pObject) = 0;
};

class ObjectCollectionBase : public EventableObject, public ICollection, public IEnumerable
{
public:
    DECLARE_ELEMENT(ObjectCollectionBase, Object);
};

class AnimatableCollectionBase : public Animatable, public ICollection, public IEnumerable
{
public:
    DECLARE_ELEMENT(AnimatableCollectionBase, Animatable);
};


template <class T, class U>
class TCollectionBase : public U
{
public:
    TCollectionBase(unsigned int cInitialCapacity = 8) : m_valuesTable(cInitialCapacity), m_version(0)
    { 
    }

    ~TCollectionBase() { Clear(); }

    void Add(T* pItem)
    {
        m_valuesTable.AddItem(pItem);

        OnCollectionChanged();

        OnAdd(pItem);
    }

    void Insert(int index, T* pItem)
    {
        m_valuesTable.Insert(index, pItem);

        OnCollectionChanged();

        OnAdd(pItem);
    }


    void Clear() { RemoveRange(0, m_valuesTable.get_Count()); }

    void Move(int oldIndex, int newIndex)
    {
        TRefCountedPtr<T> pItem = m_valuesTable[oldIndex];

        RemoveAt(oldIndex);
        Insert(newIndex, pItem);
    }


    bool Contains(T* pItem)  
    {
        return m_valuesTable.IndexOf(pItem) != -1;
    }

    bool Remove(T* pItem)
    {
        for(int i = 0; i < get_Count(); i++)
        {
            if(m_valuesTable[i] == pItem)
            {
                RemoveAt(i);

                return true;
            }
        }
        return false;
    }

    void RemoveRange(int idx, int count)
    {
        while(count > 0)
        {
            int removeIdx = idx + count - 1;
            TRefCountedPtr<T> pValue = m_valuesTable[removeIdx];

            m_valuesTable.RemoveAt(removeIdx);

            OnRemove(pValue);
            count -= 1;
        }

        OnCollectionChanged();
    }

    void RemoveAt(int idx)
    {
        RemoveRange(idx, 1);
    }

    T* operator[](int idx)
    {
        if(idx >= get_Count())
        {
            LibraryCriticalError();
        }

        return m_valuesTable[idx];
    }

    int get_Count()
    {
        return m_valuesTable.get_Count();
    }

    int IndexOf(Object* pObject)
    {
        int cCount = get_Count();

        for(int idx = 0; idx < cCount; idx++)
        {
            if(Object::Equals(pObject, m_valuesTable[idx]))
            {
                return idx;
            }
        }
        return -1;
    }


    bool get_IsEmpty() { return get_Count() == 0; }

    TRefCountedPtr<Object> GetItemAt(int idx)
    {
        return (*this)[idx];
    }

    void SetItemAt(int idx, Object* pValue)
    {
        if(pValue != NULL && object_cast<T>(pValue) == NULL)
        {
            LibraryCriticalError();
        }

        m_valuesTable[idx] = object_cast<T>(pValue);
    }

    void Add(Object* pObject)
    {
        if(!pObject->IsType(T::get_StaticType()))
        {
            LibraryCriticalError();
        }

        TCollectionBase<T, U>::Add(object_cast<T>(pObject));
    }

    bool Contains(Object* pObject) 
    {
        if(!pObject->IsType(T::get_StaticType()))
        {
            LibraryCriticalError();
        }

        return TCollectionBase<T, U>::Contains(object_cast<T>(pObject));
    }

    bool Remove(Object* pObject)
    {
        if(!pObject->IsType(T::get_StaticType()))
        {
            LibraryCriticalError();
        }

        return TCollectionBase<T, U>::Remove(object_cast<T>(pObject));
    }

    ObjectPtr GetEnumerator()
    {
        return NULL;
    }

    void BlockWrite()
    {
        m_blockWrite = true;
    }

    void UnblockWrite()
    {
        m_blockWrite = false;
    }

protected:
    virtual void OnAdd(T* pAdd) { /* Override */ }
    virtual void OnRemove(T* pRemove) { /*Override  */ }
    virtual void OnCollectionChanged() { m_version++; }

private:

    ItemStructList< TRefCountedPtr<T> > m_valuesTable;

    bool m_blockWrite;
    unsigned int m_version;
};



template <class T>
class TObjectCollection : public TCollectionBase<T, ObjectCollectionBase>
{

};

template <class T>
class TAnimatableCollection : public TCollectionBase<T, AnimatableCollectionBase>
{
public:

void CloneCore(Animatable* pSource)
{
    TAnimatableCollection<T>* pCollectionSource = (TAnimatableCollection<T>*) pSource;

    for(int idx = 0; idx < pCollectionSource->get_Count(); idx++)
    {
        Animatable* pAnimatable = object_cast<Animatable>(pCollectionSource->GetItemAt(idx));

        if(pAnimatable != NULL)
        {
            TCollectionBase<T, AnimatableCollectionBase>::Add(pAnimatable->Clone());
        }
    }

    Animatable::CloneCore(pSource);
}

protected:

virtual void OnAdd(T* pAdd) { OnAnimatablePropertyChanged(NULL, pAdd); }
virtual void OnRemove(T* pRemove) { OnAnimatablePropertyChanged(pRemove, NULL); }

};


}; // namespace AVUI
