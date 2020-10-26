#include <AVUICommon.h>
#include <AVUIBindingBase.h>

REGISTER_ELEMENT_BEGIN(AVUI::BindingBase)

REGISTER_ELEMENT_END()

namespace AVUI {

BindingBase::BindingBase()
{   
    m_isSealed = false;
    m_flags = BindingFlags::Default;
    m_pFallbackValue = DependencyProperty::get_UnsetValue();
}

BindingBase::~BindingBase()
{

}

void BindingBase::ChangeFlag(BindingFlags::Enum flag, bool value)
{
   if (value)
    {
        m_flags = (BindingFlags::Enum) (m_flags | flag);
    }
    else
    {
        m_flags = (BindingFlags::Enum) (m_flags & ~flag);
    }
}

void BindingBase::ChangeFlagsWithinMask(BindingFlags::Enum mask, BindingFlags::Enum flags)
{
    m_flags = (BindingFlags::Enum) ((m_flags & ~mask) | (flags & mask));
}

void BindingBase::CheckSealed()
{
    if (m_isSealed)
    {
        LibraryCriticalError();
    }
}

void BindingBase::ClearFlag(BindingFlags::Enum flag)
{
    m_flags = (BindingFlags::Enum) (m_flags & ~flag);
}

BindingFlags::Enum BindingBase::FlagsFrom(BindingMode::Enum bindingMode)
{
    switch (bindingMode)
    {
    case BindingMode::TwoWay:
        return BindingFlags::TwoWay;

    case BindingMode::OneWay:
        return BindingFlags::OneWay;

    case BindingMode::OneTime:
        return BindingFlags::OneTime;

    case BindingMode::OneWayToSource:
        return BindingFlags::OneWayToSource;

    case BindingMode::Default:
        return BindingFlags::PropDefault;
    }
    return BindingFlags::IllegalInput;
}

BindingFlags::Enum BindingBase::FlagsFrom(UpdateSourceTrigger::Enum updateSourceTrigger)
{
    switch (updateSourceTrigger)
    {
    case UpdateSourceTrigger::Default:
        return BindingFlags::UpdateDefault;

    case UpdateSourceTrigger::PropertyChanged:
        return BindingFlags::OneTime;

    case UpdateSourceTrigger::LostFocus:
        return BindingFlags::UpdateOnLostFocus;

    case UpdateSourceTrigger::Explicit:
        return BindingFlags::UpdateExplicitly;
    }
    return BindingFlags::IllegalInput;
}

BindingFlags::Enum BindingBase::GetFlagsWithinMask(BindingFlags::Enum mask)
{
    return (BindingFlags::Enum) (m_flags & mask);
}

void BindingBase::SetFlag(BindingFlags::Enum flag)
{
    m_flags = (BindingFlags::Enum) (m_flags | flag);
}

bool BindingBase::TestFlag(BindingFlags::Enum flag)
{
    return ((m_flags & flag) != BindingFlags::OneTime);
}

TRefCountedPtr<BindingExpressionBase> BindingBase::CreateBindingExpression(DependencyObject* pTargetObject, const DependencyProperty* pTargetProperty)
{
    m_isSealed = true;
    return CreateBindingExpressionOverride(pTargetObject, pTargetProperty, NULL);
}

TRefCountedPtr<BindingExpressionBase> BindingBase::CreateBindingExpression(DependencyObject* pTargetObject, const DependencyProperty* pTargetProperty, BindingExpressionBase* pOwner)
{
    m_isSealed = true;
    return CreateBindingExpressionOverride(pTargetObject, pTargetProperty, pOwner);
}



}; // namespace AVUI
