#pragma once

#include <AVUIDependencyObject.h>

namespace AVUI {

class ControlTemplate;
class Style;
class TriggerBase;

class TriggerAction : public DependencyObject
{
public:
    DECLARE_ELEMENT(TriggerAction, DependencyObject);

    TriggerAction() { }

    virtual void Invoke(DependencyObject* pInvokeElement, ControlTemplate* pTemplateScope) = 0;

    void Seal(TriggerBase* pContainingTrigger)
    {
        if(pContainingTrigger != m_pContainingTrigger)
        {
            CheckSealed();
        }
        DependencyObject::Seal();
    }

    TriggerBase* get_ContainingTrigger() { return m_pContainingTrigger; }

private:

    TriggerBase* m_pContainingTrigger;
};


}; // namespace AVUI
