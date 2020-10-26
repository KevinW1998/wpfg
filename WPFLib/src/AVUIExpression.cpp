#include <AVUICommon.h>
#include <AVUIExpression.h>
#include <AVUIDependencyObject.h>
#include <AVUIBindingExpressionBase.h>

REGISTER_ELEMENT_BEGIN(AVUI::Expression)

REGISTER_ELEMENT_END()

namespace AVUI {


Expression::Expression(ExpressionMode::Enum mode)
{
    m_flags = InternalFlags::None;
    switch (mode)
    {
    case ExpressionMode::None:
        return;

    case ExpressionMode::NonSharable:
        m_flags = (InternalFlags::Enum) (m_flags | InternalFlags::NonShareable);
        return;

    case ExpressionMode::ForwardsInvalidations:
        m_flags = (InternalFlags::Enum) (m_flags | InternalFlags::ForwardsInvalidations);
        m_flags = (InternalFlags::Enum) (m_flags | InternalFlags::NonShareable);
        return;

    case ExpressionMode::SupportsUnboundSources:
        m_flags = (InternalFlags::Enum) (m_flags | InternalFlags::ForwardsInvalidations);
        m_flags = (InternalFlags::Enum) (m_flags | InternalFlags::NonShareable);
        m_flags = (InternalFlags::Enum) (m_flags | InternalFlags::SupportsUnboundSources);
        return;
    }

    LibraryCriticalError();
}

void Expression::ChangeSources(DependencyObject* pDO, const DependencyProperty* pDP, ItemStructList<TRefCountedPtr<DependencySource> >* pNewSources)
{
    if ((pDO == NULL) && !get_ForwardsInvalidations())
    {
        LibraryCriticalError();
    }
    if ((pDP == NULL) && !get_ForwardsInvalidations())
    {
        LibraryCriticalError();
    }
    if (get_Shareable())
    {
        LibraryCriticalError();
    }

    // DependencyObject::ValidateSources(pDO, pNewSources, this); -- Unnecessary, don't support avalon's threading model
    if (get_ForwardsInvalidations())
    {
        DependencyObject::ChangeExpressionSources(this, NULL, NULL, pNewSources);
    }
    else
    {
        DependencyObject::ChangeExpressionSources(this, pDO, pDP, pNewSources);
    }
}


WeakDependencySource::WeakDependencySource(DependencyObject* pItem, const DependencyProperty* pDP) : m_pDP(pDP)
{
    m_pItem = BindingExpressionBase::CreateReference(pItem);
}

DependencyObject* WeakDependencySource::get_DependencyObject() { return (DependencyObject*) (Object*) BindingExpressionBase::GetReference(m_pItem); }



}; // namespace AVUI

