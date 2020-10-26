#include <AVUICommon.h>
#include <AVUIStyleHelper.h>
#include <AVUIStyleSelector.h>
#include <AVUIStyle.h>
#include <AVUITrigger.h>
#include <AVUIEventTrigger.h>
#include <AVUIMultiTrigger.h>

REGISTER_ELEMENT_BEGIN(AVUI::Style)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_INTERFACE(IXamlResolutionContext)
    REGISTER_PROPERTY(TargetType)
    REGISTER_PROPERTY(BasedOn)
    REGISTER_PROPERTY(Setters)
    REGISTER_PROPERTY(Triggers)
REGISTER_ELEMENT_END()

REGISTER_INTERFACE_TYPE(AVUI::IXamlResolutionContext);

REGISTER_ELEMENT_BEGIN(AVUI::StyleSelector)

    REGISTER_DEFAULT_FACTORY()
REGISTER_ELEMENT_END()


namespace AVUI {

Style::Style() : m_isSealed(false)
{

}

void Style::Seal()
{
    if(m_isSealed)
    {
        return;
    }

    m_isSealed = true;

    if(get_TargetType() == NULL)
    {
        LibraryCriticalError();
    }

    ProcessSetters();
    ProcessSelfStyles(this);
    ProcessVisualTriggers(this);
}

void Style::ProcessSelfStyles(Style* pStyle)
{
    if(pStyle == NULL)
    {
        return;
    }

    ProcessSelfStyles(pStyle->m_pBasedOn);
    for(int valueIndex = 0; valueIndex < pStyle->PropertyValues.get_Count(); valueIndex++)
    {
        PropertyValue& value = pStyle->PropertyValues[valueIndex];

        StyleHelper::UpdateTables(value, 0, ChildRecordFromChildIndex, TriggerSourceRecordFromChildIndex, &m_hasInstanceValues);
        StyleHelper::AddContainerDependent(*(value.pProperty), false, ContainerDependents);
    }

}

void Style::ProcessSetters()
{
    SetterCollection& styleSetters = *(get_Setters());
    for(int i = 0; i < styleSetters.get_Count(); i++)
    {
        Setter* pSetter = styleSetters[i];

        UpdatePropertyValueList(pSetter->get_Property(), PropertyValueType::Set, pSetter->get_Value());
    }
}

void Style::UpdatePropertyValueList(const DependencyProperty* pDependencyProperty, PropertyValueType::Enum propertyValueType, Object* pValue)
{
    int index = -1;
    for(int i = 0; i < PropertyValues.get_Count(); i++)
    {
        if(PropertyValues[i].pProperty == pDependencyProperty)
        {
            index = i;
            break;
        }
    }

    if(index >= 0)
    {
        PropertyValue& value = PropertyValues[index];
        value.ValueType = propertyValueType;
        value.Value = pValue;
    }
    else
    {
        PropertyValue value;

        value.ValueType = propertyValueType;
        value.pProperty = pDependencyProperty;
        value.Value = pValue;

        PropertyValues.AddItem(value);
    }
}

void Style::ProcessVisualTriggers(Style* pStyle)
{
    if(pStyle == NULL)
    {
        return;
    }

    ProcessVisualTriggers(pStyle->m_pBasedOn);

    TriggerCollection& triggers = pStyle->m_visualTriggers;
    for(int i = 0; i < triggers.get_Count(); i++)
    {
        TriggerBase* pTrigger = triggers[i];

        pTrigger->Seal();

        ItemStructList<PropertyValue>& propertyValues = pTrigger->get_PropertyValues();

        for(int propertyValueIndex = 0; propertyValueIndex < propertyValues.get_Count(); propertyValueIndex++)
        {
            PropertyValue& propertyValue = propertyValues[propertyValueIndex];

            StyleHelper::AddContainerDependent(*(propertyValue.pProperty), true, ContainerDependents);
            StyleHelper::UpdateTables(propertyValue, 0, ChildRecordFromChildIndex, TriggerSourceRecordFromChildIndex, &m_hasInstanceValues);
        }

        if (pTrigger->get_HasEnterActions() || pTrigger->get_HasExitActions())
        {
              if (is_type<Trigger>(pTrigger))
              {
                  Trigger* pPropertyTrigger = object_cast<Trigger>(pTrigger);

                  StyleHelper::AddPropertyTriggerWithAction(pTrigger, *(pPropertyTrigger->get_Property()), PropertyTriggersWithActions);
              }
              else if (is_type<MultiTrigger>(pTrigger))
              {
                    MultiTrigger* pMultiTrigger = (MultiTrigger*) pTrigger;
                    for (int num5 = 0; num5 < pMultiTrigger->get_Conditions()->get_Count(); num5++)
                    {
                          Condition* pCondition = (*(pMultiTrigger->get_Conditions()))[num5];
                          StyleHelper::AddPropertyTriggerWithAction(pMultiTrigger, *(pCondition->get_Property()), 
                                                                    PropertyTriggersWithActions);
                    }
              }
        }

        if(pTrigger->IsType(GET_TYPE(EventTrigger)))
        {
            EventTrigger* pEventTrigger = (EventTrigger*) pTrigger;

            StyleHelper::ProcessEventTrigger(pEventTrigger, &m_triggerActions, &m_eventHandlersStore);
        }
    }

}


const Type* Style::GetPropertyResolutionContext(Object* pObjectCurrent)
{
    return get_TargetType();
}

const Property* Style::GetValueConverterContext(Object* pObjectCurrent)
{
    if(pObjectCurrent == NULL)
    {
        return NULL;
    }

    if(pObjectCurrent->IsType(GET_TYPE(Setter)))
    {
        return ((Setter*)pObjectCurrent)->get_Property();
    }

    if(pObjectCurrent->IsType(GET_TYPE(Trigger)))
    {
        return ((Trigger*)pObjectCurrent)->get_Property();
    }

    if(pObjectCurrent->IsType(GET_TYPE(Condition)))
    {
        return ((Condition*)pObjectCurrent)->get_Property();
    }

    return NULL;
}

ItemStructList< TRefCountedPtr<TriggerAction> >* Style::GetActionsForEvent(const RoutedEvent* pEvent)
{
    for(int idx = 0; idx < m_triggerActions.get_Count(); idx++)
    {
        if(m_triggerActions[idx].pEvent == pEvent)
        {
            return m_triggerActions[idx].pActions;
        }
    }
    return NULL;
}


}; // namespace AVUI
