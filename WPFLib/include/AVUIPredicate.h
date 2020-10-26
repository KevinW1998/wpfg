namespace AVUI {

class Predicate : public Object
{
public:
    Predicate() { }
    virtual ~Predicate() { }

    virtual bool Invoke(Object* pItem) = 0;
    virtual const Type* GetArgsType() = 0;
};


class IPredicateInvoke
{
public:
    virtual ~IPredicateInvoke() { };
    virtual bool Invoke(Object* pArgs) = 0;
};


template<typename Args, typename T>
class TObjectPredicateInvoke : public IPredicateInvoke
{
public:

    typedef bool (T::*F)(Args* pItem);

    TObjectPredicateInvoke(T* pT, F func) : m_pListener(pT), m_func(func) { }

    virtual bool Invoke(Object* pArgs)
    {
        Args* pTypedArgs = object_cast<Args>(pArgs);
        if(pArgs != NULL && pTypedArgs == NULL)
        {
            LibraryCriticalError();
        }

        return (m_pListener->*m_func)(pTypedArgs);
    }
    virtual ~TObjectPredicateInvoke() { }

private:

    F m_func;
    T* m_pListener;
};


template<typename Args>
class TSimplePredicateInvoke : public IPredicateInvoke
{
public:

    typedef bool (*F)(Args* pItem);

    TSimplePredicateInvoke(F func) : m_func(func) { }

    virtual bool Invoke(Object* pArgs)
    {
        Args* pTypedArgs = object_cast<Args>(pArgs);
        if(pArgs != NULL && pTypedArgs == NULL)
        {
            LibraryCriticalError();
        }

        return (*m_func)(pTypedArgs);
    }
    virtual ~TSimplePredicateInvoke() { }

private:

    F m_func;
};



template<typename Args>
class TPredicate : public Predicate
{
public:

    ~TPredicate() { if(m_pInvoke != NULL) delete m_pInvoke; }

    template<class T>
    static TRefCountedPtr<TPredicate<Args> > Create(T* pT, bool(T::*func)(Args*))
    {
        TRefCountedPtr<TPredicate<Args> > pPredicate(new TPredicate<Args>(), false);

        pPredicate->m_pInvoke = new TObjectPredicateInvoke<Args, T>(pT, func);

        return pPredicate;
    }

    static TRefCountedPtr<TPredicate<Args> > Create(void(*func)(Object*, Args*))
    {
        TRefCountedPtr<TPredicate<Args> > pPredicate(new TPredicate<Args>(), false);

        pPredicate->m_pInvoke = new TSimplePredicateInvoke<Args>(func);

        return pPredicate;
    }

    virtual bool Invoke(Object* pArgs)
    {
        if(m_pInvoke != NULL)
        {
            return m_pInvoke->Invoke(pArgs);
        }
        return false;
    }

    virtual const Type* GetArgsType()
    {
        return &(GET_TYPE(Args));
    }

private:

    TPredicate() : m_pInvoke(NULL) { }

    IPredicateInvoke* m_pInvoke;
};

typedef TPredicate<Object> ObjectPredicate;

};

