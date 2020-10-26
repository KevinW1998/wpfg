#pragma once

#include <AVUITriggerBase.h>
#include <AVUISetterCollection.h>

namespace AVUI {

class Trigger : public TriggerBase
{
public:
    Trigger() : m_pProperty(NULL) { }

    DECLARE_ELEMENT(Trigger, TriggerBase);

    void Seal();

    const DependencyProperty* get_Property() { return m_pProperty; }
    void set_Property(const DependencyProperty* pDP) { m_pProperty = pDP; }

    Object* get_Value() { return m_pPropertyValue; }
    void set_Value(Object* pValue) { m_pPropertyValue = pValue; }

    String* get_SourceName() { return m_pSourceName; }
    void set_SourceName(String* pSourceName) { m_pSourceName = pSourceName; }

    SetterCollection* get_Setters() { return &m_setters; }

    static Setter* CheckChildIsSetter(Object* pObject);
    static void ProcessSettersCollection(TriggerBase* pTrigger, SetterCollection& setters);

    virtual bool GetCurrentState(DependencyObject* pContainer);

private:

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Setters, SetterCollection, true);
    IMPLEMENT_REFERENCE_PROPERTY(Property, DependencyProperty, false);
    IMPLEMENT_REFERENCE_PROPERTY(Value, Object, false);
    IMPLEMENT_REFERENCE_PROPERTY(SourceName, String, false);

    Object* get_SettersValue() { return &m_setters; }

    const DependencyProperty* m_pProperty;
    ObjectPtr m_pPropertyValue;
    StringPtr m_pSourceName;

    SetterCollection m_setters;

};




}; // namespace AVUI
