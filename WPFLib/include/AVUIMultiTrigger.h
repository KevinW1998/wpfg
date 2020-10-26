#pragma once

#include <AVUITriggerBase.h>
#include <AVUISetterCollection.h>
#include <AVUIConditionCollection.h>

namespace AVUI {

class MultiTrigger : public TriggerBase
{
public:
    MultiTrigger() { }

    DECLARE_ELEMENT(MultiTrigger, TriggerBase);

    void Seal();

    SetterCollection* get_Setters() { return &m_setters; }
    ConditionCollection* get_Conditions() { return &m_conditions; }

    virtual bool GetCurrentState(DependencyObject* pContainer);

private:

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Setters, SetterCollection, true);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Conditions, ConditionCollection, false);

    Object* get_SettersValue() { return &m_setters; }
    Object* get_ConditionsValue() { return &m_conditions; }

    SetterCollection m_setters;
    ConditionCollection m_conditions;
};




}; // namespace AVUI
