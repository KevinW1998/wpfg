#pragma once

#include <AVUIEventableObject.h>

namespace AVUI {

class DependencyObject;


class WeakDependencySource : public Object
{
public:
    WeakDependencySource(DependencyObject* pDO, const DependencyProperty* pDP);
    WeakDependencySource() : m_pDP(NULL) { }

    // Properties

    DependencyObject* get_DependencyObject();
    const DependencyProperty* get_DependencyProperty() { return m_pDP; }

private:

    TRefCountedPtr<Object> m_pItem;
    const DependencyProperty* m_pDP;
};

class DependencySource : public Object
{
public:
    DependencySource(DependencyObject* pDO, const DependencyProperty* pDP) : m_pDO(pDO), m_pDP(pDP) { }

    // Properties

    DependencyObject* get_DependencyObject() { return m_pDO; }
    const DependencyProperty* get_DependencyProperty() { return m_pDP; }

private:

    TRefCountedPtr<DependencyObject> m_pDO;
    const DependencyProperty* m_pDP;
};

class ExpressionMode
{
public:
    enum Enum
    {
        None,
        NonSharable,
        ForwardsInvalidations,
        SupportsUnboundSources
    };
};


class Expression : public EventableObject
{
public:
    DECLARE_ELEMENT(Expression, Object);

    Expression() : m_flags(InternalFlags::None) { }
    Expression(ExpressionMode::Enum mode);

    void ChangeSources(DependencyObject* pDO, const DependencyProperty* pDP, ItemStructList<TRefCountedPtr<DependencySource> >* pNewSources);
    virtual TRefCountedPtr<Expression> Copy(DependencyObject* pDO, const DependencyProperty& dp) { return this; }
    virtual TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > GetSources() { return NULL; }

    virtual ObjectPtr GetValue(DependencyObject* pDO, const DependencyProperty& dp) { return DependencyProperty::get_UnsetValue(); }
    void MarkAttached() { m_flags = (InternalFlags::Enum) (m_flags | InternalFlags::Attached); }
    virtual void OnAttach(DependencyObject* pDO, const DependencyProperty& dp) { };
    virtual void OnDetach(DependencyObject* pDO, const DependencyProperty& dp) { };
    virtual void OnPropertyInvalidation(DependencyObject* pDO, const DependencyPropertyChangedEventArgs* pArgs) { };

    virtual bool SetValue(DependencyObject* pDO, const DependencyProperty& dp, Object* pValue) { return false; }

    bool get_Attachable() { if(!get_Shareable()) { return !get_HasBeenAttached(); } return true; }
    bool get_ForwardsInvalidations() { return (m_flags & InternalFlags::ForwardsInvalidations) != InternalFlags::None; }
    bool get_HasBeenAttached() { return (m_flags & InternalFlags::Attached) != InternalFlags::None; }
    bool get_Shareable() { return (m_flags & InternalFlags::NonShareable) == InternalFlags::None; }
    bool get_SupportsUnboundSources() { return (m_flags & InternalFlags::SupportsUnboundSources) != InternalFlags::None; }

    static Object* get_NoValue() { static NoValue noValue; return &noValue; }

private:

    class InternalFlags
    {
    public:
        enum Enum
        {
            None = 0,
            NonShareable = 1,
            ForwardsInvalidations = 2,
            SupportsUnboundSources = 4,
            Attached = 8
        };
    };

    InternalFlags::Enum m_flags;

    class NoValue : public Object
    {
    public:

        int AddRef() { return 1; /* NOOP - NoValue is a STATIC only construct */ }
        int Release() { return 1; /* NOOP - NoValue is a STATIC only construct */ }
    };


};

typedef TRefCountedPtr<Expression> ExpressionPtr;

};
