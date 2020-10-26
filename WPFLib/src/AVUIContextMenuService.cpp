#include <AVUICommon.h>
#include <AVUIContextMenuService.h>
#include <AVUIPopupControlService.h>

REGISTER_ELEMENT_BEGIN(AVUI::ContextMenuService)
    REGISTER_PROPERTY(ContextMenu)
    REGISTER_PROPERTY(PlacementTarget)
    REGISTER_PROPERTY(HorizontalOffset)
    REGISTER_PROPERTY(VerticalOffset)
    REGISTER_PROPERTY(HasDropShadow)
    REGISTER_PROPERTY(PlacementRectangle)
    REGISTER_PROPERTY(Placement)
    REGISTER_PROPERTY(ShowOnDisabled)
    REGISTER_PROPERTY(IsEnabled)

    REGISTER_ROUTED_EVENT(ContextMenuOpening)
    REGISTER_ROUTED_EVENT(ContextMenuClosing)

    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT(AVUI::ContextMenuEventArgs)

namespace AVUI {

void ContextMenuService::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_ContextMenuOpeningEvent(), ContextMenuEventHandler::Create(OnContextMenuOpening), false);
}

bool ContextMenuService::ContextMenuIsEnabled(DependencyObject* pDO)
{
    if (GetContextMenu(pDO) == NULL || !GetIsEnabled(pDO) || (!PopupControlService::IsElementEnabled(pDO) && !GetShowOnDisabled(pDO)))
    {
        return false;
    }
    return true;
}

void ContextMenuService::OnContextMenuOpening(Object* pSender, ContextMenuEventArgs* pArgs)
{
    if (pArgs->get_TargetElement() == NULL)
    {
        DependencyObject* pDO = object_cast<DependencyObject>(pSender);
        if (pDO != NULL && ContextMenuIsEnabled(pDO))
        {
            pArgs->set_TargetElement(pDO);
        }
    }
}

 

 


}; // namespace AVUI
