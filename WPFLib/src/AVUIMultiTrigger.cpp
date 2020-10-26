#include <AVUICommon.h>
#include <AVUIMultiTrigger.h>
#include <AVUISetter.h>
#include <AVUITrigger.h>

REGISTER_ELEMENT_BEGIN(AVUI::Condition)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Binding)
    REGISTER_PROPERTY(Property)
    REGISTER_PROPERTY(SourceName)
    REGISTER_PROPERTY(Value)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::ConditionCollection)
    REGISTER_DEFAULT_FACTORY()
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::MultiTrigger)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Setters)
    REGISTER_PROPERTY(Conditions)
REGISTER_ELEMENT_END()

namespace AVUI {

void MultiTrigger::Seal()
{
    if(get_IsSealed())
    {
        return;
    }

    Trigger::ProcessSettersCollection(this, *(get_Setters()));

    TriggerConditionArrayPtr conditions(new TriggerConditionArray(m_conditions.get_Count()), false);

    for(int i = 0; i < conditions->get_Length(); i++)
    {
        TriggerCondition& condition = (*conditions)[i];

        condition.pProperty = m_conditions[i]->get_Property();
        condition.LogicalOp = LogicalOp::Equals;
        condition.Value = m_conditions[i]->get_Value();
        condition.SourceName = m_conditions[i]->get_SourceName();
    }

    set_TriggerConditions(conditions);

    for(int i = 0; i < get_PropertyValues().get_Count(); i++)
    {
        get_PropertyValues()[i].Conditions = conditions;
    }
    TriggerBase::Seal();
}

bool MultiTrigger::GetCurrentState(DependencyObject* pContainer)
{
    TriggerConditionArray* pConditions = get_TriggerConditions();

    bool flag = pConditions->get_Length() > 0;
    for (int i = 0; flag && (i < get_TriggerConditions()->get_Length()); i++)
    {
        TriggerCondition* pCondition = &((*pConditions)[i]);

        flag = pCondition->Match(pContainer->GetValue(*(pCondition->pProperty)));
    }
    return flag;
}

}; // namespace AVUI
