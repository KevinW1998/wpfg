#pragma once

#include <AVUIDependencyProperty.h>

namespace AVUI {

class BindingBase;

class Condition : public Object
{
    friend class XamlParser;
public:
    Condition() : m_pProperty(NULL) { };

    DECLARE_ELEMENT(Condition, Object);

    IMPLEMENT_REFERENCE_PROPERTY(Binding, BindingBase, false);
    IMPLEMENT_REFERENCE_PROPERTY(Property, DependencyProperty, false);
    IMPLEMENT_REFERENCE_PROPERTY(SourceName, String, false);
    IMPLEMENT_REFERENCE_PROPERTY(Value, Object, false);

    BindingBase* get_Binding() { return m_pBinding; }
    void set_Binding(BindingBase* pBinding) { m_pBinding = pBinding; }

    void set_Property(const DependencyProperty* pProperty) { m_pProperty = pProperty; }
    const DependencyProperty* get_Property() { return m_pProperty; }

    String* get_SourceName() { return m_pSourceName; }
    void set_SourceName(String* pSourceName) { m_pSourceName = pSourceName; }

    void set_Value(Object* pValue) { m_pPropertyValue = pValue; }
    Object* get_Value() { return m_pPropertyValue; }
private:

    TRefCountedPtr<BindingBase> m_pBinding;
    StringPtr m_pSourceName;
    ObjectPtr m_pPropertyValue;
    const DependencyProperty* m_pProperty;

};


}; // namespace AVUI
