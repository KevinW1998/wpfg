#include <AVUICommon.h>
#include <AVUIUIElement.h>
#include <AVUIReverseInheritProperty.h>
#include <AVUIDeferredElementTreeState.h>

namespace AVUI
{


void ReverseInheritProperty::OnOriginValueChanged(DependencyObject* pOldOrigin, DependencyObject* pNewOrigin, DeferredElementTreeState** ppOldTreeState)
{
    DeferredElementTreeState* pTreeState = *ppOldTreeState;

    *ppOldTreeState = NULL;

    if (pOldOrigin != NULL)
    {
        SetCacheFlagInAncestry(pOldOrigin, false, pTreeState);
    }
    if (pNewOrigin != NULL)
    {
        SetCacheFlagInAncestry(pNewOrigin, true, NULL);
    }
    if (pOldOrigin != NULL)
    {
        FirePropertyChangeInAncestry(pOldOrigin, true, pTreeState);
    }
    if (pNewOrigin != NULL)
    {
        FirePropertyChangeInAncestry(pNewOrigin, false, NULL);
    }
    if (*ppOldTreeState == NULL && pTreeState != NULL)
    {
        pTreeState->Clear();
        *ppOldTreeState = pTreeState;
    }
}

void ReverseInheritProperty::FirePropertyChangeInAncestry(DependencyObject* pElement, bool oldValue, DeferredElementTreeState* pTreeState)
{
   if (IsFlagSet(pElement, m_flagChanged))
   {
        SetFlag(pElement, m_flagChanged, false);
        if (oldValue)
        {
            pElement->ClearValue(*m_pProperty);
        }
        else
        {
            pElement->SetValue(*m_pProperty, BoxValue(true));
        }

        UIElement* pUIE = object_cast<UIElement>(pElement);
        FireNotifications(pUIE, oldValue);

        if (pUIE != NULL)
        {
            if(pUIE->BlockReverseInheritance())
            {
                return;
            }
        }

        DependencyObject* pCoreParent = DeferredElementTreeState::GetCoreParent(pElement, pTreeState);
        DependencyObject* pLogicalParent = DeferredElementTreeState::GetLogicalParent(pElement, pTreeState);
        if (pCoreParent != NULL)
        {
            FirePropertyChangeInAncestry(pCoreParent, oldValue, pTreeState);
        }
        if (pLogicalParent != NULL && pLogicalParent != pCoreParent)
        {
            FirePropertyChangeInAncestry(pLogicalParent, oldValue, pTreeState);
        }
    }
}

bool ReverseInheritProperty::IsFlagSet(DependencyObject* pObject, unsigned int flag)
{
    UIElement* pElement = object_cast<UIElement>(pObject);
    if (pElement != NULL)
    {
        return pElement->ReadFlag(flag);
    }
    return false;
}

void ReverseInheritProperty::SetCacheFlagInAncestry(DependencyObject* pElement, bool newValue, DeferredElementTreeState* pTreeState)
{
    if ((!newValue && IsFlagSet(pElement, m_flagCache)) || 
        (newValue && !IsFlagSet(pElement, m_flagCache)))
    {
        SetFlag(pElement, m_flagCache, newValue);
        SetFlag(pElement, m_flagChanged, !IsFlagSet(pElement, m_flagChanged));

        UIElement* pUIE = object_cast<UIElement>(pElement);
        if (pUIE == NULL || !pUIE->BlockReverseInheritance())
        {
            DependencyObject* pCoreParent = DeferredElementTreeState::GetCoreParent(pElement, pTreeState);
            DependencyObject* pLogicalParent = DeferredElementTreeState::GetLogicalParent(pElement, pTreeState);
            if (pCoreParent != NULL)
            {
                SetCacheFlagInAncestry(pCoreParent, newValue, pTreeState);
            }
            if (pLogicalParent != NULL && pLogicalParent != pCoreParent)
            {
                SetCacheFlagInAncestry(pLogicalParent, newValue, pTreeState);
            }
        }
    }
}

void ReverseInheritProperty::SetFlag(DependencyObject* pDO, unsigned int flag, bool value)
{
    UIElement* pElement = object_cast<UIElement>(pDO);
    if (pElement != NULL)
    {
        pElement->WriteFlag(flag, value);
    }
}
  

}; // namespace AVUI




