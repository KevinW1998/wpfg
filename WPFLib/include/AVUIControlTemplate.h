#pragma once

#include <AVUITemplate.h>
#include <AVUITriggerCollection.h>
#include <AVUIStyleStructures.h>
#include <AVUIItemStructList.h>
#include <AVUIIXamlResolutionContext.h>

namespace AVUI {

class Control;
class String;

class ControlTemplate : public FrameworkTemplate, public IXamlResolutionContext
{
    friend class UIElement;
    friend class StyleHelper;
    friend class Control;
    friend class FrameworkTemplate;
public:
    ControlTemplate() { };

    DECLARE_ELEMENT(ControlTemplate, FrameworkTemplate);

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Triggers, TriggerCollection, false);
    IMPLEMENT_REFERENCE_PROPERTY(TargetType, Type, false);

    TriggerCollection* get_Triggers() { return &m_visualTriggers; };
    void set_TargetType(const Type* pTargetType) { m_pTargetType = pTargetType; }
    const Type* get_TargetType() { return m_pTargetType; }

protected:

    virtual void Seal();

private:

    const Type* GetPropertyResolutionContext(Object* pObjectCurrent);
    const Property* GetValueConverterContext(Object* pObjectCurrent);

    void ProcessVisualTriggers();

    TriggerCollection m_visualTriggers;
    ItemStructList<ChildRecord> ChildRecordFromChildIndex;
    ItemStructList<ContainerDependent> ContainerDependents;
    ItemStructList<PropertyValue> PropertyValues;
    ItemStructList<ItemStructMap<TriggerSourceRecord> > TriggerSourceRecordFromChildIndex;
    bool m_hasInstanceValues;

    ItemStructMap<ObjectPtr> PropertyTriggersWithActions;

    const Type* m_pTargetType;
};



}; // namespace AVUI
