#pragma once

#include <AVUIItemStructMap.h>
#include <AVUIItemStructList.h>

namespace AVUI {

class Delegate : public Object
{
    friend class EventHelper;
public:
    Delegate() { }
    virtual ~Delegate() { }

    virtual void Invoke(Object* pSender, Object* pArgs) = 0;
    virtual const Type* GetArgsType() = 0;

protected:
    virtual void Reset()
    {
        m_pNext = NULL;
    }

    virtual bool IsEqual(Delegate* pBase) = 0;

private:

    TRefCountedPtr<Delegate> m_pNext;
};


class IEventHandlerInvoke
{
public:
    virtual ~IEventHandlerInvoke() { };
    virtual void Invoke(Object* pSender, Object* pArgs) = 0;
    virtual bool IsObjectType() = 0;
    virtual bool IsEqual(IEventHandlerInvoke* pInvoke) = 0;
    virtual const void* GetObjectPtr() = 0;
};


template<typename Args, class T>
class TObjectEventHandlerInvoke : public IEventHandlerInvoke
{
public:

    typedef void (T::*F)(Object* pSender, Args*);

    TObjectEventHandlerInvoke(T* pT, F func) : m_pListener(pT), m_func(func) { }

    virtual void Invoke(Object* pSender, Object* pArgs)
    {
        Args* pTypedArgs = object_cast<Args>(pArgs);
        if(pArgs != NULL && pTypedArgs == NULL)
        {
            LibraryCriticalError();
        }

        (m_pListener->*m_func)(pSender, pTypedArgs);
    }
    virtual ~TObjectEventHandlerInvoke() { }

    virtual bool IsEqual(IEventHandlerInvoke* pInvoke)
    {
        if(pInvoke->GetObjectPtr() != GetObjectPtr())
        {
            return false;
        }

        TObjectEventHandlerInvoke<Args, T>* pTypedInvoke = (TObjectEventHandlerInvoke<Args, T>*) pInvoke;

        return m_func == pTypedInvoke->m_func;

    }

    virtual bool IsObjectType() { return true; };

    virtual const void* GetObjectPtr() { return m_pListener; }

    void* operator new (size_t size)
    {
        return Object::AllocateMemory(size);
    }
    void operator delete (void *p)
    {
        return Object::FreeMemory(p);
    }

private:

    F m_func;
    T* m_pListener;
};


template<typename Args>
class TSimpleEventHandlerInvoke : public IEventHandlerInvoke
{
public:

    typedef void (*F)(Object* pSender, Args*);

    TSimpleEventHandlerInvoke(F func) : m_func(func) { }

    virtual void Invoke(Object* pSender, Object* pArgs)
    {
        Args* pTypedArgs = object_cast<Args>(pArgs);
        if(pArgs != NULL && pTypedArgs == NULL)
        {
            LibraryCriticalError();
        }

        (*m_func)(pSender, pTypedArgs);
    }
    virtual ~TSimpleEventHandlerInvoke() { }

    virtual bool IsEqual(IEventHandlerInvoke* pInvoke)
    {
        return m_func == ((TSimpleEventHandlerInvoke<Args>*) pInvoke)->m_func;
    }

    virtual bool IsObjectType() 
    { 
        return false;
    }

    virtual const void* GetObjectPtr() { return NULL; }

    void* operator new (size_t size)
    {
        return Object::AllocateMemory(size);
    }
    void operator delete (void *p)
    {
        return Object::FreeMemory(p);
    }

private:

    F m_func;
};



template<typename Args>
class TDelegate : public Delegate
{
public:

    ~TDelegate() { if(m_pInvoke != NULL) delete m_pInvoke; }

    template<class T>
    static TRefCountedPtr<TDelegate<Args> > Create(T* pT, void(T::*func)(Object*, Args*))
    {
        TRefCountedPtr<TDelegate<Args> > pHandler(new TDelegate<Args>(), false);

        pHandler->m_pInvoke = new TObjectEventHandlerInvoke<Args, T>(pT, func);

        return pHandler;
    }

    static TRefCountedPtr<TDelegate<Args> > Create(void(*func)(Object*, Args*))
    {
        TRefCountedPtr<TDelegate<Args> > pHandler(new TDelegate<Args>(), false);

        pHandler->m_pInvoke = new TSimpleEventHandlerInvoke<Args>(func);

        return pHandler;
    }

    virtual void Invoke(Object* pSender, Object* pArgs)
    {
        if(m_pInvoke != NULL)
        {
            m_pInvoke->Invoke(pSender, pArgs);
        }
    }

    virtual bool IsEqual(Delegate* pBase)
    {
        if(pBase->GetArgsType() != GetArgsType())
        {
            return false;
        }

        TDelegate<Args>* pTypedBase = (TDelegate<Args>*)pBase;

        if(m_pInvoke->IsObjectType() != pTypedBase->m_pInvoke->IsObjectType())
        {
            return false;
        }

        return m_pInvoke->IsEqual(pTypedBase->m_pInvoke);
    }

    virtual const Type* GetArgsType()
    {
        return &(GET_TYPE(Args));
    }

private:

    TDelegate() : m_pInvoke(NULL) { }

    IEventHandlerInvoke* m_pInvoke;
};



typedef Object EventArgs;

typedef TDelegate<EventArgs> EventHandler;

typedef ItemStructList<TRefCountedPtr<Delegate> > EventHandlerList;

class EventHelper
{
public:
    static void AddEventHandler(EventHandlerList* pList, Delegate* pNewHandler);
    static void RemoveEventHandler(EventHandlerList* pList, Delegate* pRemoveHandler);
    static void InvokeEventHandler(EventHandlerList* pList, Object* pSender, Object* pArgs);
};

class RoutedEvent;

class EventManager
{
public:
    static void RegisterClassHandler(const Type& type, const RoutedEvent& routedEvent, Delegate* pEventHandler, bool handledEventsToo);
    static void AddListenersToRoute(EventRoute* pRoute, Object* pObject, const Type& type, const RoutedEvent* pEvent);
    static void CleanupStaticAllocations();
private:

    struct ClassEventHandler
    {
    public:
        ClassEventHandler() : pEvent(NULL), HandledEventsToo(false) { }
        ClassEventHandler(const RoutedEvent& routedEvent, Delegate* pHandler, bool handledEventsToo) : pEvent(&routedEvent), 
                          pEventHandler(pHandler), HandledEventsToo(handledEventsToo) { }

        TRefCountedPtr<Delegate> pEventHandler;
        const RoutedEvent* pEvent;
        bool HandledEventsToo;
    };

    static ItemStructMap<ItemStructList<ClassEventHandler> >* get_ClassEventHandlers()
    {
        static ItemStructMap<ItemStructList<ClassEventHandler> > s_classEventHandlers;

        return &s_classEventHandlers;
    }

};


}; // namespace AVUI
