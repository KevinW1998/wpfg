#pragma once

#include <AVUIDependencyProperty.h>

namespace AVUI {

class Setter : public Object
{
    friend class XamlParser;
public:
    Setter();

    DECLARE_ELEMENT(Setter, Object);

    IMPLEMENT_REFERENCE_PROPERTY(Property, DependencyProperty, false);
    IMPLEMENT_REFERENCE_PROPERTY(Value, Object, false);
    IMPLEMENT_REFERENCE_PROPERTY(TargetName, String, false);

    String* get_TargetName() { return m_pTargetName; }
    void set_TargetName(String* pTargetName) { m_pTargetName = pTargetName; }

    void set_Property(const DependencyProperty* pProperty) { m_pProperty = pProperty; }
    const DependencyProperty* get_Property() { return m_pProperty; }

    void set_Value(Object* pValue) { m_pPropertyValue = pValue; }
    Object* get_Value() { return m_pPropertyValue; }
private:

    StringPtr m_pTargetName;
    ObjectPtr m_pPropertyValue;
    const DependencyProperty* m_pProperty;

};


}; // namespace AVUI
