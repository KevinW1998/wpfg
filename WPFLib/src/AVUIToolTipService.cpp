#include <AVUICommon.h>
#include <AVUIToolTipService.h>
#include <AVUIPopupControlService.h>

REGISTER_ELEMENT_BEGIN(AVUI::ToolTipService)
    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
    REGISTER_PROPERTY(ToolTip)
    REGISTER_PROPERTY(HorizontalOffset)
    REGISTER_PROPERTY(VerticalOffset)
    REGISTER_PROPERTY(HasDropShadow)
    REGISTER_PROPERTY(PlacementTarget)
    REGISTER_PROPERTY(PlacementRectangle)
    REGISTER_PROPERTY(Placement)
    REGISTER_PROPERTY(ShowOnDisabled)
    REGISTER_PROPERTY(IsOpen)
    REGISTER_PROPERTY(IsEnabled)
    REGISTER_PROPERTY(ShowDuration)
    REGISTER_PROPERTY(InitialShowDelay)
    REGISTER_PROPERTY(BetweenShowDelay)

    REGISTER_ROUTED_EVENT(ToolTipOpening)
    REGISTER_ROUTED_EVENT(ToolTipClosing)
    REGISTER_ROUTED_EVENT(FindToolTip)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT(AVUI::ToolTipEventArgs)
REGISTER_ELEMENT(AVUI::FindToolTipEventArgs)

namespace AVUI {




void ToolTipService::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(UIElement), get_FindToolTipEvent(), FindToolTipEventHandler::Create(OnFindToolTip), false);
}


void ToolTipService::OnFindToolTip(Object* pSender, FindToolTipEventArgs* pArgs)
{
    if (pArgs->get_TargetElement() == NULL)
    {
        DependencyObject* pDO = object_cast<DependencyObject>(pSender);
        if (pDO != NULL)
        {
            if (PopupControlService::get_Current()->StopLookingForToolTip(pDO))
            {
                pArgs->set_IsHandled(true);
                pArgs->set_KeepCurrentActive(true);
            }
            else if (ToolTipIsEnabled(pDO))
            {
                pArgs->set_TargetElement(pDO);
                pArgs->set_IsHandled(true);
            }
        }
    }
}

bool ToolTipService::ToolTipIsEnabled(DependencyObject* pDO)
{
    if (GetToolTip(pDO) == NULL || !GetIsEnabled(pDO) || (!PopupControlService::IsElementEnabled(pDO) && !GetShowOnDisabled(pDO)))
    {
        return false;
    }
    return true;
}

 

 


ToolTipEventArgs::ToolTipEventArgs(bool opening)
{
    if (opening)
    {
        set_RoutedEvent(&ToolTipService::get_ToolTipOpeningEvent());
    }
    else
    {
        set_RoutedEvent(&ToolTipService::get_ToolTipClosingEvent());
    }

}

FindToolTipEventArgs::FindToolTipEventArgs() : m_keepCurrentActive(false)
{
    set_RoutedEvent(&ToolTipService::get_FindToolTipEvent());
}




}; // namespace AVUI

