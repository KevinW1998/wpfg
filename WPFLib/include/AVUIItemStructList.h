#pragma once

#include <new>
#include <AVUIIEnumerable.h>
#include <AVUIBuiltinTypes.h>
#include <AVUIIComparer.h>
#pragma warning(disable:4345)
namespace AVUI {


template <class T>
class ItemStructList : public Object, public IEnumerable
{
public:

    ItemStructList(int capacity = 0) : m_count(0), m_capacity(capacity), m_rgData(NULL), m_notifyOnChange(false)
    {
        if(capacity > 0)
        {
            m_rgData = (T*) Object::AllocateMemory(m_capacity * sizeof(T));
        }
    }

    ItemStructList(const ItemStructList<T>& other)
    {
        if(other.m_rgData != NULL)
        {
            m_notifyOnChange = other.m_notifyOnChange;
            m_count = other.m_count;
            m_capacity = other.m_capacity;
            m_rgData = (T*) Object::AllocateMemory(m_capacity * sizeof(T));

            for(int i = 0; i < m_count; i++)
            {
                ::new (m_rgData + i) T(other.m_rgData[i]);
            }
        }
    }

    ItemStructList& operator=(const ItemStructList<T>& other)
    {
        if(this == &other)
        {
            return *this;
        }

        EraseData();

        if(other.m_rgData != NULL)
        {
            m_notifyOnChange = other.m_notifyOnChange;
            m_count = other.m_count;
            m_capacity = other.m_capacity;
            m_rgData = (T*) Object::AllocateMemory(m_capacity * sizeof(T));

            for(int i = 0; i < m_count; i++)
            {
                ::new (m_rgData + i) T(other.m_rgData[i]);
            }
        }
        return *this;
    }

    void EraseData()
    {
        if(m_rgData != NULL)
        {
            for(int i = 0; i < m_count; i++)
            {
                m_rgData[i].~T();
            }

            Object::FreeMemory(m_rgData);
            m_rgData = NULL;
            m_count = 0;
            m_capacity = 0;
        }
    }



    ~ItemStructList()
    {
        EraseData();
    }

    int AddNew() { return Add(1, true); }
    int AddEmptyItems(int delta) { return Add(delta, true); }

    void AddItem(const T& item) 
    { 
        int insertPosition = Add(1, false);
        ::new (m_rgData + insertPosition) T(item);
        m_count++;

        if(m_notifyOnChange)
        {
            OnItemAdded(insertPosition, item);
        }
    }

    void AppendTo(ItemStructList<T>& destinationList)
    {
        for(int i = 0; i < m_count; i++)
        {
            destinationList.Add(m_rgData[i]);
        }
    }

    void Clear()
    {
        for(int i = 0; i < m_count; i++)
        {
            m_rgData[i].~T();
        }

        m_count = 0;

        if(m_notifyOnChange)
        {
            OnCleared();
        }
    }

    void Move(int oldIndex, int newIndex)
    {
        T t = m_rgData[oldIndex];

        RemoveAt(oldIndex);
        Insert(newIndex, t);

        if(m_notifyOnChange)
        {
            OnItemMoved(oldIndex, newIndex);
        }
    }

    virtual ObjectPtr GetEnumerator() 
    { 
        return NULL; 
    }

    void Sort()
    {
        for(int idxOuter = 0; idxOuter < m_count; idxOuter++)
        {
            for(int idxInner = 1; idxInner < m_count; idxInner++)
            {
                if(m_rgData[idxInner - 1] > m_rgData[idxInner])
                {
                    T temp = m_rgData[idxInner - 1];
                    m_rgData[idxInner - 1] = m_rgData[idxInner];
                    m_rgData[idxInner] = temp;
                }
            }
        }
    }
    bool Contains(const T& value) const
    {
        return IndexOf(value) != -1;
    }

    bool IsEmpty() const { return m_count == 0; }

    void EnsureIndex(int index)
    {
        int delta = (index + 1) - m_count;

        if(delta > 0)
        {
            AddEmptyItems(delta);
        }
    }

    int IndexOf(const T& value) const
    {
        for (int i = 0; i < m_count; i++)
        {
            if (m_rgData[i] == value)
            {
                  return i;
            }
        }
        return -1;
    }

    bool IsValidIndex(int index) const
    {
        return index < m_count && index >= 0;
    }

    bool Remove(const T& value)
    {
        int index = IndexOf(value);
        if(index != -1)
        {
            RemoveAt(index);
            return true;
        }

        return false;
    } 

    void RemoveAt(int index)
    {
        if(index >= m_count)
        {
            LibraryCriticalError();
        }

        for(int i = index; i < m_count - 1; i++)
        {
            m_rgData[i] = m_rgData[i + 1];
        }
        m_rgData[m_count - 1].~T();
        m_count -= 1;

        if(m_notifyOnChange)
        {
            OnItemRemoved(index);
        }
    }

    void Insert(int index, const T& item)
    {
        AddNew();

        for(int idx = (int)m_count - 1; idx > index; idx--)
        {
            m_rgData[idx] = m_rgData[idx - 1];
        }
        m_rgData[index] = item;

        if(m_notifyOnChange)
        {
            OnItemAdded(index, item);
        }
    }


    int get_Count() const { return m_count; }
    int get_Capacity() const { return m_capacity; }

    const T& operator [] (int i) const { if(i >= m_count) LibraryCriticalError(); return m_rgData[i]; }
    T& operator [] (int i) { if(i >= m_count) LibraryCriticalError(); return m_rgData[i]; }

    T* get_Ptr() { return m_rgData; }
    const T* get_Ptr() const { return m_rgData; }

protected:
    bool get_NotifyOnChange() { return m_notifyOnChange; }
    void set_NotifyOnChange(bool notifyOnChange) { m_notifyOnChange = notifyOnChange; }

    virtual void OnItemRemoved(int index) { }
    virtual void OnItemAdded(int insertPosition, const T& item) { }
    virtual void OnItemMoved(int oldIndex, int newIndex) { }
    virtual void OnCleared() { };

private:

    int Add(int delta, bool incrCount)
    {
        if(m_rgData != NULL)
        {
            if(m_count + delta > m_capacity)
            {
                int newCapacity = Int::Max(m_capacity * 2, m_count + delta);

                T* rgNewData = (T*) Object::AllocateMemory(newCapacity * sizeof(T));

                for(int i = 0; i < m_count; i++)
                {
                    ::new (rgNewData + i) T(m_rgData[i]);

                    m_rgData[i].~T();
                }
                Object::FreeMemory(m_rgData);

                m_rgData = rgNewData;
                m_capacity = newCapacity;
            }
        }
        else
        {
            m_capacity = Int::Max(delta, 2);
            m_rgData = (T*) Object::AllocateMemory(m_capacity * sizeof(T));
        }

        int oldCount = m_count;
        if(incrCount)
        {
            for(int i = m_count; i < m_count + delta; i++)
            {
                ::new (m_rgData + i) T();
            }
            m_count += delta;
        }

        return oldCount;
    }

    T* m_rgData;
    int m_capacity;
    int m_count;
    bool m_notifyOnChange;
};


template<class T>
static void SortItemStructList(ItemStructList<T>* pList, int index, int count, IComparer* pComparer)
{
    for(int idxOuter = index; idxOuter < count; idxOuter++)
    {
        for(int idxInner = 1; idxInner < count; idxInner++)
        {
            if(pComparer->Compare((*pList)[idxInner - 1], (*pList)[idxInner]) > 0)
            {
                T temp = (*pList)[idxInner - 1];
                (*pList)[idxInner - 1] = (*pList)[idxInner];
                (*pList)[idxInner] = temp;
            }
        }
    }        
} 

template<class T>
static void SortItemStructList(ItemStructList<T>* pList, IComparer* pComparer)
{
    SortItemStructList(pList, 0, pList->get_Count(), pComparer);
};



};
