#include <AVUICommon.h>
#include <AVUILayoutManager.h>

namespace AVUI {

void LayoutManager::RemoveMeasureOrphans(UIElement* pElement)
{
    unsigned short parentTreeLevel = pElement->GetTreeLevel();

    for(int idx = 0; idx < m_measureQueue.get_Count(); idx++)
    {
        UIElement* pElement = m_measureQueue[idx].get_Ptr();
        if((pElement->GetTreeLevel() == parentTreeLevel + 1) && pElement->get_UIParent() == pElement)
        {
            RemoveFromMeasureQueue(pElement);
        }
    }
}

void LayoutManager::RemoveArrangeOrphans(UIElement* pElement)
{
    unsigned short parentTreeLevel = pElement->GetTreeLevel();

    for(int idx = 0; idx < m_arrangeQueue.get_Count(); idx++)
    {
        UIElement* pElement = m_arrangeQueue[idx].get_Ptr();
        if((pElement->GetTreeLevel() == parentTreeLevel + 1) && pElement->get_UIParent() == pElement)
        {
            RemoveFromArrangeQueue(pElement);
        }
    }
}

bool LayoutManager::CanRelyOnParentMeasure(UIElement* pParent)
{
    if(!pParent->get_IsMeasureValid())
    {
        return !pParent->get_MeasureInProgress();
    }

    return false;
}

bool LayoutManager::CanRelyOnParentArrange(UIElement* pParent)
{
    if(!pParent->get_IsArrangeValid())
    {
        return !pParent->get_ArrangeInProgress();
    }

    return false;
}

void LayoutManager::AddToMeasureQueue(UIElement* pElement)
{
    if(!pElement->get_IsAttachedToPresentationSource())
    {
        return;
    }

    if(!pElement->get_IsInMeasureQueue())
    {
        RemoveMeasureOrphans(pElement);

        UIElement* pParent = pElement->get_UIParent();
        if(pParent == NULL || !CanRelyOnParentMeasure(pParent))
        {
            m_measureQueue.AddItem(pElement);
            pElement->set_IsInMeasureQueue(true);
        }
    }
}

void LayoutManager::RemoveFromMeasureQueue(UIElement* pElement)
{
    if(pElement->get_IsInMeasureQueue())
    {
        m_measureQueue.Remove(pElement);
        pElement->set_IsInMeasureQueue(false);
    }
}

void LayoutManager::AddToArrangeQueue(UIElement* pElement)
{
    if(!pElement->get_IsAttachedToPresentationSource())
    {
        return;
    }

    if(!pElement->get_IsInArrangeQueue())
    {
        RemoveArrangeOrphans(pElement);

        UIElement* pParent = pElement->get_UIParent();
        if(pParent == NULL || !CanRelyOnParentArrange(pParent))
        {
            m_arrangeQueue.AddItem(pElement);
            pElement->set_IsInArrangeQueue(true);
        }
    }
}

void LayoutManager::RemoveFromArrangeQueue(UIElement* pElement)
{
    if(pElement->get_IsInArrangeQueue())
    {
        m_arrangeQueue.Remove(pElement);
        pElement->set_IsInArrangeQueue(false);
    }
}

void LayoutManager::Update()
{
    if(!m_isInUpdateLayout)
    {
        m_isInUpdateLayout = true;

        while(!m_measureQueue.IsEmpty() || !m_arrangeQueue.IsEmpty())
        {
            while(!m_measureQueue.IsEmpty())
            {
                UIElement* pElementMeasure = m_measureQueue[m_measureQueue.get_Count() - 1];

                m_measureQueue.RemoveAt(m_measureQueue.get_Count() - 1);
                pElementMeasure->set_IsInMeasureQueue(false);

                pElementMeasure->Measure(pElementMeasure->get_PreviousAvailableSize());
            }

            while(!m_arrangeQueue.IsEmpty())
            {
                UIElement* pElementArrange = m_arrangeQueue[m_arrangeQueue.get_Count() - 1];

                m_arrangeQueue.RemoveAt(m_arrangeQueue.get_Count() - 1);
                pElementArrange->set_IsInArrangeQueue(false);

                pElementArrange->Arrange(get_ProperArrangeRect(pElementArrange));

                if(!m_measureQueue.IsEmpty())
                {
                    break;
                }
            }

            FireLayoutUpdatedIfClean();
        }

        m_isInUpdateLayout = false;
    }
}

void LayoutManager::FireLayoutUpdatedIfClean()
{
    if(m_measureQueue.IsEmpty() && m_arrangeQueue.IsEmpty())
    {
        invoke_LayoutUpdated(NULL, NULL);   
    }
}


Rect LayoutManager::get_ProperArrangeRect(UIElement* pElement)
{
      Rect arrangeRect = pElement->get_PreviousArrangeRect();

      if(pElement->get_UIParent() == NULL)
      {
            arrangeRect.set_X(0.0f);
            arrangeRect.set_Y(0.0f);

            if (Float::IsPositiveInfinity(pElement->get_PreviousAvailableSize().get_Width()))
            {
                  arrangeRect.set_Width(pElement->get_DesiredSize().get_Width());
            }
            if (Float::IsPositiveInfinity(pElement->get_PreviousAvailableSize().get_Height()))
            {
                  arrangeRect.set_Height(pElement->get_DesiredSize().get_Height());
            }
      }
      return arrangeRect;
}

}; // namespace AVUI
