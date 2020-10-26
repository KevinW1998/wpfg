#pragma once

#include <AVUIExpression.h>

namespace AVUI {

class DependencyObject;

class TemplateBinding : public Expression
{
public:
    TemplateBinding();
    TemplateBinding(DependencyObject* pSource, const DependencyProperty* pProperty);
    ~TemplateBinding();


    ObjectPtr GetValue(DependencyObject* pDO, const DependencyProperty& dp);
    bool SetValue(DependencyObject* pDO, const DependencyProperty& dp, Object* pValue);
    void OnAttach(DependencyObject* pDO, const DependencyProperty& dp);
    void MarkAttached();
    void OnDetach(DependencyObject* pDO, const DependencyProperty& dp);

    DECLARE_ELEMENT(TemplateBinding, Expression);

    const DependencyProperty* get_SourceProperty() { return m_pSourceProperty; }
    void set_SourceProperty(const DependencyProperty* pProperty) { m_pSourceProperty = pProperty; }

    DependencyObject* get_SourceObject() { return m_pSourceObject; }
    void set_SourceObject(DependencyObject* pDO) { m_pSourceObject = pDO; }

    virtual TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > GetSources();
    // TODO -- Copy a binding?
    virtual TRefCountedPtr<Expression> Copy(DependencyObject* pDO, const DependencyProperty& dp) { LibraryCriticalError(); return NULL; };

    virtual void OnPropertyInvalidation(DependencyObject* pDO, const DependencyPropertyChangedEventArgs* pArgs);

private:

    const DependencyProperty* m_pTargetProperty;
    TWeakPtr<DependencyObject> m_pTargetObject;

    const DependencyProperty* m_pSourceProperty;
    TWeakPtr<DependencyObject> m_pSourceObject;
    bool m_isAttached;
};

typedef TRefCountedPtr<TemplateBinding> TemplateBindingPtr;

};
