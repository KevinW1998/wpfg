#include <AVUICommon.h>
#include <AVUITrigger.h>
#include <AVUISetter.h>

REGISTER_ELEMENT_BEGIN(AVUI::Trigger)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Setters)
    REGISTER_PROPERTY(Property)
    REGISTER_PROPERTY(Value)
    REGISTER_PROPERTY(SourceName)
REGISTER_ELEMENT_END()

namespace AVUI {

Setter* Trigger::CheckChildIsSetter(Object* pObject)
{
    if(pObject == NULL)
    {
        LibraryCriticalError();
    }

    if(!pObject->IsType(GET_TYPE(Setter)))
    {
        LibraryCriticalError();
    }

    return object_cast<Setter>(pObject);
}
void Trigger::ProcessSettersCollection(TriggerBase* pTrigger, SetterCollection& setters)
{
//    setters.Seal();

    for(int i = 0; i < setters.get_Count(); i++)
    {
        Setter* pSetter = setters[i];

        pTrigger->AddToPropertyValues(pSetter->get_TargetName(), *(pSetter->get_Property()), pSetter->get_Value(), PropertyValueType::Trigger);
    }
}

void Trigger::Seal()
{
    if(get_IsSealed())
    {
        return;
    }

    ProcessSettersCollection(this, *(get_Setters()));

    TriggerConditionArrayPtr conditions(new TriggerConditionArray(1), false);

    TriggerCondition& condition = (*conditions)[0];

    condition.pProperty = m_pProperty;
    condition.LogicalOp = LogicalOp::Equals;
    condition.Value = m_pPropertyValue;
    condition.SourceName = m_pSourceName;

    set_TriggerConditions(conditions);

    for(int i = 0; i < get_PropertyValues().get_Count(); i++)
    {
        get_PropertyValues()[i].Conditions = conditions;
    }
}

bool Trigger::GetCurrentState(DependencyObject* pContainer)
{
    TriggerConditionArray* pConditions = get_TriggerConditions();

    if(pConditions == NULL)
    {
        return false;
    }

    TriggerCondition* pCondition = &((*pConditions)[0]);
    return pCondition->Match(pContainer->GetValue(*(pCondition->pProperty)));

}


}; // namespace AVUI
