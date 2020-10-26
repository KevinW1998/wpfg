#include <AVUICommon.h>
#include <AVUIUIElement.h>
#include <AVUIUIElementCollection.h>

REGISTER_ELEMENT_BEGIN(AVUI::UIElementCollection)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI {

void UIElementCollection::OnAdd(UIElement* pAdd)
{
    if(m_pOwner != NULL)
    {        
        m_pOwner->AddVisualChild(pAdd);

        if(!m_isNotLogical)
        {
            m_pOwner->AddLogicalChild(pAdd);
        }
    }
}

void UIElementCollection::OnRemove(UIElement* pRemove)
{
    if(m_pOwner != NULL)
    {
        if(!m_isNotLogical)
        {
            m_pOwner->RemoveLogicalChild(pRemove);
        }

        m_pOwner->RemoveVisualChild(pRemove);
    }
}

}; // namespace AVUI
