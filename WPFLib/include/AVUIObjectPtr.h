#pragma once

namespace AVUI {

template <class T>
class TRefCountedPtr
{
public:
	TRefCountedPtr()
	{
		m_p = NULL;
	}
	TRefCountedPtr(T* p, bool addRef = true)
	{
		m_p = p;
		if (p != NULL && addRef)
			((Object*)p)->AddRef();
	}
    TRefCountedPtr(const TRefCountedPtr<T>& other)
    {
        m_p = other.m_p;

        if(m_p != NULL)
        {
            ((Object*)m_p)->AddRef();
        }
    }

	~TRefCountedPtr()
	{
		if (m_p != NULL)
			((Object*)m_p)->Release();
	}

    const T* get_Ptr() const 
    {
        return m_p;
    }

    T* get_Ptr() 
    {
        return m_p;
    }

	operator T*() const
	{
		return m_p;
	}
	T& operator*() const
	{
		return *m_p;
	}

	T* operator->() const
	{
        return m_p;
	}
	bool operator!() const
	{
		return (m_p == NULL);
	}
	bool operator==(T* pT) const
	{
		return m_p == pT;
	}
    bool operator!=(T* pT) const
    {
        return m_p != pT;
    }

    T* operator=(const TRefCountedPtr<T>& p)
    {
        if(m_p == p.m_p)
        {
            return m_p;
        }

        if(m_p != NULL)
        {
            ((Object*)m_p)->Release();
            m_p = NULL;
        }

        m_p = (T*) p;

        if(m_p != NULL)
        {
            ((Object*)m_p)->AddRef();
        }

        return m_p;
    }

    T* operator=(T* p)
    {
        if(m_p == p)
        {
            return m_p;
        }

        if(m_p != NULL)
        {
            ((Object*)m_p)->Release();
            m_p = NULL;
        }

        m_p = p;

        if(m_p != NULL)
        {
            ((Object*)m_p)->AddRef();
        }

        return m_p;
    }

private:

	T* m_p;
};


class WeakReference
{
    friend class Object;
public:

    WeakReference() : m_pWeakReferenceNext(NULL), m_pWeakReferencePrevious(NULL) { }
    virtual ~WeakReference() { }

protected:

    virtual void InvalidateTarget()
    {
        m_pWeakReferenceNext = m_pWeakReferencePrevious = NULL;
    }

    void AddWeakReference(Object* pObject)
    {
        pObject->AddWeakReference(this);
    }

    void RemoveWeakReference(Object* pObject)
    {
        pObject->RemoveWeakReference(this);
    }

private:

    WeakReference* m_pWeakReferenceNext;
    WeakReference* m_pWeakReferencePrevious;

};

template <class T>
class TWeakPtr : public WeakReference
{
public:
	TWeakPtr() : m_p(NULL)
	{
		m_p = NULL;
	}
	TWeakPtr(T* p) : m_p(NULL)
	{
        set_Target(p);
	}
    TWeakPtr(const TWeakPtr<T>& other) : m_p(NULL)
    {
        set_Target(other.m_p);
    }

	~TWeakPtr()
	{
        if(m_p != NULL)
        {
            set_Target(NULL);
        }
	}
	operator T*() const
	{
		return m_p;
	}
	T& operator*() const
	{
		return *m_p;
	}
	T** operator&()
	{
		return &m_p;
	}
	T* operator->() const
	{
        return m_p;
	}
	bool operator!() const
	{
		return (m_p == NULL);
	}
	bool operator==(T* pT) const
	{
		return m_p == pT;
	}
    bool operator!=(T* pT) const
    {
        return m_p != pT;
    }

    T* operator=(T* p)
    {
        set_Target(p);
        return m_p;
    }

    T* operator=(const TWeakPtr<T>& p)
    {
        set_Target(p.m_p);
        return m_p;
    }

    void set_Target(T* p)
    {
        if(m_p != NULL)
        {
            RemoveWeakReference((Object*)m_p);
        }

        m_p = p;

        if(m_p != NULL)
        {
            AddWeakReference((Object*)m_p);
        }
    }

protected:

    virtual void InvalidateTarget()
    {
        m_p = NULL;
        WeakReference::InvalidateTarget();
    }

	T* m_p;
};

class IWeakPtrNotifier
{
public:
    virtual void TargetInvalidated() = 0;
};

template <class T>
class TWeakPtrWithNotification : public TWeakPtr<T>
{
public:
    TWeakPtrWithNotification(T* p, IWeakPtrNotifier* pNotifier) : TWeakPtr<T>(p), m_pNotifier(pNotifier)
    {
    }    

private:

    virtual void InvalidateTarget()
    {
        TWeakPtr<T>::InvalidateTarget();
        if(m_pNotifier != NULL)
        {
            m_pNotifier->TargetInvalidated();
        }
    }

    IWeakPtrNotifier* m_pNotifier;

};


typedef TRefCountedPtr<Object> ObjectPtr;
typedef TWeakPtr<Object> ObjectWeakPtr;

};
