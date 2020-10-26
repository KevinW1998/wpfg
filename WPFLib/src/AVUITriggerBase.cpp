#include <AVUICommon.h>
#include <AVUITriggerBase.h>
#include <AVUIUIElement.h>

REGISTER_ELEMENT_BEGIN(AVUI::TriggerBase)

    REGISTER_PROPERTY(EnterActions)
    REGISTER_PROPERTY(ExitActions)
REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::TriggerAction)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::TriggerActionCollection)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI {

int TriggerBase::m_layerRankNext = 0;

void TriggerBase::AddToPropertyValues(String* pChildName, const DependencyProperty& dp, Object* pValue, PropertyValueType::Enum valueType)
{
    PropertyValue propertyValue;

    propertyValue.pChildName = pChildName;
    propertyValue.ValueType = valueType;
    propertyValue.Conditions = NULL;
    propertyValue.pProperty = &dp;
    propertyValue.Value = pValue;

    m_propertyValueList.AddItem(propertyValue);
}

void TriggerBase::ProcessParametersContainer(const DependencyProperty& dp)
{
    if(dp == UIElement::get_StyleProperty())
    {
        LibraryCriticalError();
    }
}

void TriggerBase::ProcessParametersset_Value(const DependencyProperty& dp, Object* pValue)
{
    // NO IMPLEMENTATION
}


void TriggerBase::Seal()
{

}


}; // namespace AVUI
