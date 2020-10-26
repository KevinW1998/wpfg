#include <AVUICommon.h>
#include <AVUIBinding.h>
#include <AVUIBindingExpression.h>
#include <AVUIPropertyPath.h>

REGISTER_ELEMENT_BEGIN(AVUI::DataTransferEventArgs)

REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::Binding)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(ElementName)
    REGISTER_PROPERTY(Path)
    REGISTER_PROPERTY(Mode)
    REGISTER_PROPERTY(RelativeSource)
    REGISTER_ROUTED_EVENT(SourceUpdated)
    REGISTER_ROUTED_EVENT(TargetUpdated)
REGISTER_ELEMENT_END()

namespace AVUI {

Binding::Binding()
{
    Initialize();
}

Binding::Binding(const String& propertyPath)
{
    Initialize();

    set_Path(object_allocate<PropertyPath>(propertyPath));
}

void Binding::Initialize()
{
    m_pSource = get_UnsetSource();
    m_pObjectSource = DependencyProperty::get_UnsetValue();
    m_attachedPropertiesInPath = 0;
    m_bindsDirectlyToSource = false;
    m_doesNotTransferDefaultValue = false;
    m_isAsync = false;
    m_sourceInUse = SourceProperties::None;
}


void Binding::set_ElementName(String* pElementName)
{
    CheckSealed();
    if ((m_sourceInUse != SourceProperties::None) && (m_sourceInUse != SourceProperties::ElementName))
    {
        LibraryCriticalError();
    }
    m_pElementSource = pElementName;
    set_SourceReference((pElementName != NULL) ? object_allocate<ElementObjectRef>(pElementName) : NULL);
}

void Binding::set_Path(PropertyPath* pPath)
{
    CheckSealed();
    m_pPropertyPath = pPath;
    m_attachedPropertiesInPath = -1;
    ClearFlag(BindingFlags::PathGeneratedInternally);
}

void Binding::set_RelativeSource(RelativeSource* pRelativeSource)
{
    CheckSealed();
    if ((m_sourceInUse != SourceProperties::None) && (m_sourceInUse != SourceProperties::RelativeSource))
    {
        LibraryCriticalError();
    }
    m_pRelativeSource = pRelativeSource;
    set_SourceReference(pRelativeSource != NULL ? object_allocate<RelativeObjectRef>(pRelativeSource) : NULL);
}


BindingMode::Enum Binding::get_Mode()
{
    switch (GetFlagsWithinMask(BindingFlags::PropagationMask))
    {
    case BindingFlags::OneTime:
        return BindingMode::OneTime;

    case BindingFlags::OneWay:
        return BindingMode::OneWay;

    case BindingFlags::OneWayToSource:
        return BindingMode::OneWayToSource;

    case BindingFlags::TwoWay:
        return BindingMode::TwoWay;

    case BindingFlags::PropDefault:
        return BindingMode::Default;
    }

    LibraryCriticalError();
    return BindingMode::TwoWay;

}
void Binding::set_Mode(BindingMode::Enum mode)
{
    CheckSealed();
    BindingFlags::Enum flags = BindingBase::FlagsFrom(mode);
    if (flags == BindingFlags::IllegalInput)
    {
        LibraryCriticalError();
    }
    ChangeFlagsWithinMask(BindingFlags::PropagationMask, flags);
}

bool Binding::get_NotifyOnSourceUpdated()
{
    return TestFlag(BindingFlags::NotifyOnSourceUpdated);
}

void Binding::set_NotifyOnSourceUpdated(bool notifyOnSourceUpdated)
{
    if (TestFlag(BindingFlags::NotifyOnSourceUpdated) != notifyOnSourceUpdated)
    {
        CheckSealed();
        ChangeFlag(BindingFlags::NotifyOnSourceUpdated, notifyOnSourceUpdated);
    }
}


bool Binding::get_NotifyOnTargetUpdated()
{
    return TestFlag(BindingFlags::NotifyOnTargetUpdated);
}

void Binding::set_NotifyOnTargetUpdated(bool notifyOnTargetUpdated)
{
    if (TestFlag(BindingFlags::NotifyOnTargetUpdated) != notifyOnTargetUpdated)
    {
        CheckSealed();
        ChangeFlag(BindingFlags::NotifyOnTargetUpdated, notifyOnTargetUpdated);
    }
}

bool Binding::get_NotifyOnValidationError()
{
    return TestFlag(BindingFlags::NotifyOnValidationError);
}

void Binding::set_NotifyOnValidationError(bool notifyOnValidationError)
{
    if (TestFlag(BindingFlags::NotifyOnValidationError) != notifyOnValidationError)
    {
        CheckSealed();
        ChangeFlag(BindingFlags::NotifyOnValidationError, notifyOnValidationError);
    }
}

void Binding::set_Source(Object* pSource)
{
    CheckSealed();
    if ((m_sourceInUse != SourceProperties::None) && (m_sourceInUse != SourceProperties::Source))
    {
        LibraryCriticalError();
    }
    m_pObjectSource = pSource;
    set_SourceReference(pSource != DependencyProperty::get_UnsetValue() ? object_allocate<ExplicitObjectRef>(pSource) : NULL);
}

void Binding::set_SourceReference(ObjectRef* pSourceReference)
{
    CheckSealed();
    m_pSource = pSourceReference;
    DetermineSource();
}

bool Binding::get_TreeContextIsRequired()
{
    if (m_attachedPropertiesInPath < 0)
    {
        if (m_pPropertyPath != NULL)
        {
            m_attachedPropertiesInPath = m_pPropertyPath->ComputeUnresolvedAttachedPropertiesInPath();
        }
        else
        {
            m_attachedPropertiesInPath = 0;
        }
    }
    return (m_attachedPropertiesInPath > 0);
}

UpdateSourceTrigger::Enum Binding::get_UpdateSourceTrigger()
{
    switch (GetFlagsWithinMask(BindingFlags::UpdateMask))
    {
    case BindingFlags::UpdateOnPropertyChanged:
        return UpdateSourceTrigger::PropertyChanged;
    
    case BindingFlags::UpdateOnLostFocus:
            return UpdateSourceTrigger::LostFocus;
    
    case BindingFlags::UpdateExplicitly:
        return UpdateSourceTrigger::Explicit;

    case BindingFlags::UpdateMask:
        return UpdateSourceTrigger::Default;
    }
    return UpdateSourceTrigger::Default;
}

void Binding::set_UpdateSourceTrigger(UpdateSourceTrigger::Enum mode)
{
    CheckSealed();
    BindingFlags::Enum flags = FlagsFrom(mode);
    if (flags == BindingFlags::IllegalInput)
    {
        LibraryCriticalError();
    }
    ChangeFlagsWithinMask(BindingFlags::UpdateDefault, flags);
}

void Binding::DetermineSource()
{
    m_sourceInUse = (m_pSource == get_UnsetSource()) ? SourceProperties::None : ((m_pRelativeSource != NULL) ? SourceProperties::RelativeSource : ((m_pElementSource != NULL) ? SourceProperties::ElementName : ((m_pObjectSource != DependencyProperty::get_UnsetValue()) ? SourceProperties::Source : SourceProperties::InternalSource)));
}

 
void Binding::UsePath(PropertyPath* pPath)
{
    m_pPropertyPath = pPath;
    SetFlag(BindingFlags::PathGeneratedInternally);
}

TRefCountedPtr<BindingExpressionBase> Binding::CreateBindingExpressionOverride(DependencyObject* pTarget, const DependencyProperty* pDP, BindingExpressionBase* pOwner)
{
    return (BindingExpressionBase*) BindingExpression::CreateBindingExpression(pTarget, pDP, this, pOwner);
}
 



}; // namespace AVUI
