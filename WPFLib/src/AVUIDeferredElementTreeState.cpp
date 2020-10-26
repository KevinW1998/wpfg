#include <AVUICommon.h>
#include <AVUIDeferredElementTreeState.h>
#include <AVUIVisual.h>
#include <AVUIUIElement.h>

REGISTER_ELEMENT_BEGIN(AVUI::DeferredElementTreeState)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()


namespace AVUI
{


DependencyObject* DeferredElementTreeState::GetCoreParent(DependencyObject* pElement, DeferredElementTreeState* pTreeState)
{
    if(pTreeState == NULL)
    {
        return NULL;
    }

    for(int idx = 0; idx < pTreeState->m_coreParents.get_Count(); idx++)
    {
        if(pTreeState->m_coreParents[idx].pElement == pElement)
        {
            return pTreeState->m_coreParents[idx].pParent;
        }
    }

    Visual* pVisual = object_cast<Visual>(pElement);
    if(pVisual != NULL)
    {
        return pVisual->get_VisualParent();
    }
    return NULL;
}

DependencyObject* DeferredElementTreeState::GetLogicalParent(DependencyObject* pElement, DeferredElementTreeState* pTreeState)
{
    if(pTreeState == NULL)
    {
        return NULL;
    }

    for(int idx = 0; idx < pTreeState->m_logicalParents.get_Count(); idx++)
    {
        if(pTreeState->m_logicalParents[idx].pElement == pElement)
        {
            return pTreeState->m_logicalParents[idx].pParent;
        }
    }

    UIElement* pUIElement = object_cast<UIElement>(pElement);
    if(pUIElement != NULL)
    {
        return pUIElement->get_Parent();
    }
    return NULL;
}

void DeferredElementTreeState::SetCoreParent(DependencyObject* pElement, DependencyObject* pParent)
{
    m_coreParents.AddItem(DeferredElementEntry(pElement, pParent));
}

void DeferredElementTreeState::SetLogicalParent(DependencyObject* pElement, DependencyObject* pParent)
{
    m_logicalParents.AddItem(DeferredElementEntry(pElement, pParent));
}

};

