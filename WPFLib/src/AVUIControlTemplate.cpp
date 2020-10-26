#include <AVUICommon.h>
#include <AVUIControl.h>
#include <AVUIControlTemplate.h>
#include <AVUIUIElement.h>
#include <AVUIXamlParser.h>
#include <AVUISetter.h>
#include <AVUITrigger.h>
#include <AVUIStyleHelper.h>
#include <AVUIMultiTrigger.h>


REGISTER_ELEMENT_BEGIN(AVUI::ControlTemplate)

    REGISTER_INTERFACE(IXamlResolutionContext)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Triggers)
    REGISTER_PROPERTY(TargetType)
REGISTER_ELEMENT_END()

namespace AVUI {


void ControlTemplate::Seal()
{
    if(get_IsSealed())
    {
        return;
    }

    FrameworkTemplate::Seal();
    ProcessVisualTriggers();
}

const Type* ControlTemplate::GetPropertyResolutionContext(Object* pObjectCurrent)
{   
    if(pObjectCurrent == NULL)
    {
        return get_TargetType();
    }

    if(pObjectCurrent->IsType(GET_TYPE(Setter)))
    {
        Setter* pSetter = (Setter*) pObjectCurrent;
        if(pSetter->get_TargetName() != NULL)
        {
            return GetTypeFromChildName(*(pSetter->get_TargetName()));
        }
    }

    if(pObjectCurrent->IsType(GET_TYPE(Trigger)))
    {
        Trigger* pTrigger = (Trigger*) pObjectCurrent;
        if(pTrigger->get_SourceName() != NULL)
        {
            return GetTypeFromChildName(*(pTrigger->get_SourceName()));
        }
    }

    return get_TargetType();
}

const Property* ControlTemplate::GetValueConverterContext(Object* pObjectCurrent)
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

void ControlTemplate::ProcessVisualTriggers()
{
    TriggerCollection& triggers = m_visualTriggers;
    for(int i = 0; i < triggers.get_Count(); i++)
    {
        TriggerBase* pTrigger = triggers[i];

        pTrigger->Seal();

        ItemStructList<PropertyValue>& propertyValues = pTrigger->get_PropertyValues();

        for(int propertyValueIndex = 0; propertyValueIndex < propertyValues.get_Count(); propertyValueIndex++)
        {
            PropertyValue& propertyValue = propertyValues[propertyValueIndex];

            if(propertyValue.pChildName == NULL)
            {
                StyleHelper::AddContainerDependent(*(propertyValue.pProperty), true, ContainerDependents);
            }

            if(propertyValue.Conditions != NULL)
            {
                TriggerConditionArray& conditions = *(propertyValue.Conditions);
                for(int conditionIndex = 0; conditionIndex < conditions.get_Length(); conditionIndex++)
                {
                    conditions[conditionIndex].SourceChildIndex = GetChildIndexByName(conditions[conditionIndex].SourceName);
                }
            }

            StyleHelper::UpdateTables(propertyValue, GetChildIndexByName(propertyValue.pChildName), 
                                      ChildRecordFromChildIndex, TriggerSourceRecordFromChildIndex, &m_hasInstanceValues);
        }

        if (pTrigger->get_HasEnterActions() || pTrigger->get_HasExitActions())
        {
              if (pTrigger->IsType(GET_TYPE(Trigger)))
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
    }

}

}; // namespace AVUI
