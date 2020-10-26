#pragma once

#include <AVUISetterCollection.h>
#include <AVUITriggerCollection.h>
#include <AVUIStyleStructures.h>
#include <AVUIEventHandlersStore.h>
#include <AVUIIXamlResolutionContext.h>

namespace AVUI {

class Style : public Object, public IXamlResolutionContext
{
    friend class UIElement;
    friend class StyleHelper;
public:
    Style();

    SetterCollection* get_Setters() { return &m_setters; };
    const SetterCollection* get_Setters() const { return &m_setters; };

    TriggerCollection* get_Triggers() { return &m_visualTriggers; };
    const TriggerCollection* get_Triggers() const { return &m_visualTriggers; };

    DECLARE_ELEMENT(Style, Object);

    void Seal();

    void set_TargetType(const Type* pTargetType) { m_pTargetType = pTargetType; }
    const Type* get_TargetType() { return m_pTargetType; }

    void set_BasedOn(Style* pBasedOn) { m_pBasedOn = pBasedOn; }
    Style* get_BasedOn() { return m_pBasedOn; }

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Setters, SetterCollection, true);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Triggers, TriggerCollection, false);
    IMPLEMENT_REFERENCE_PROPERTY(TargetType, Type, false);
    IMPLEMENT_REFERENCE_PROPERTY(BasedOn, Style, false);

private:

    struct TriggerActionRecord
    {
        TriggerActionRecord() : pEvent(NULL) { }
        TriggerActionRecord(ItemStructList<TRefCountedPtr<TriggerAction> >* pActionsIn, const RoutedEvent* pEventIn) : pEvent(pEventIn), pActions(pActionsIn) { }

        const RoutedEvent* pEvent;
        TRefCountedPtr<ItemStructList< TRefCountedPtr<TriggerAction> > > pActions;
    };

    EventHandlersStore* get_EventHandlersStore() { return &m_eventHandlersStore; }

    void ProcessSelfStyles(Style* pStyle);
    void ProcessSetters();
    void ProcessVisualTriggers(Style* pStyle);
    void UpdatePropertyValueList(const DependencyProperty* pDependencyProperty, PropertyValueType::Enum propertyValueType, Object* pValue);

    const Type* GetPropertyResolutionContext(Object* pObjectCurrent);
    const Property* GetValueConverterContext(Object* pObjectCurrent);


    ItemStructList< TRefCountedPtr<TriggerAction> >* GetActionsForEvent(const RoutedEvent* pEvent);

    TRefCountedPtr<Style> m_pBasedOn;
    bool m_hasInstanceValues;
    int m_modified;
    bool m_isSealed;
    SetterCollection m_setters;

    const Type* m_pDefaultTargetType;

    const Type* m_pTargetType;
    TriggerCollection m_visualTriggers;
    ItemStructList<ChildRecord> ChildRecordFromChildIndex;
    ItemStructList<ContainerDependent> ContainerDependents;
    ItemStructList<PropertyValue> PropertyValues;
    ItemStructList<ItemStructMap<TriggerSourceRecord> > TriggerSourceRecordFromChildIndex;

    ItemStructMap<ObjectPtr> PropertyTriggersWithActions;

    ItemStructList<TriggerActionRecord> m_triggerActions;
    EventHandlersStore m_eventHandlersStore;
};


}; // namespace AVUI
