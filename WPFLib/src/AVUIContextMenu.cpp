#include <AVUICommon.h>
#include <AVUIContextMenu.h>
#include <AVUIDispatcher.h>

REGISTER_ELEMENT_BEGIN(AVUI::ContextMenu)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(HorizontalOffset)
    REGISTER_PROPERTY(VerticalOffset)
    REGISTER_PROPERTY(IsOpen)
    REGISTER_PROPERTY(PlacementTarget)
    REGISTER_PROPERTY(PlacementRectangle)
    REGISTER_PROPERTY(Placement)
    REGISTER_PROPERTY(HasDropShadow)
    REGISTER_PROPERTY(StaysOpen)
    REGISTER_PROPERTY(InsideContextMenu)
    REGISTER_ROUTED_EVENT(ContextMenuOpened)
    REGISTER_ROUTED_EVENT(ContextMenuClosed)
    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
REGISTER_ELEMENT_END()

namespace AVUI {

void ContextMenu::StaticConstructor()
{
    /*
    EventManager.RegisterClassHandler(typeof(ContextMenu), AccessKeyManager.AccessKeyPressedEvent, new AccessKeyPressedEventHandler(ContextMenu.OnAccessKeyPressed));
    FrameworkElement.DefaultStyleKeyProperty.OverrideMetadata(typeof(ContextMenu), new FrameworkPropertyMetadata(typeof(ContextMenu)));
    _dType = DependencyObjectType.FromSystemTypeInternal(typeof(ContextMenu));
    Control.IsTabStopProperty.OverrideMetadata(typeof(ContextMenu), new FrameworkPropertyMetadata(BooleanBoxes.FalseBox));
    KeyboardNavigation.TabNavigationProperty.OverrideMetadata(typeof(ContextMenu), new FrameworkPropertyMetadata(KeyboardNavigationMode.Cycle));
    KeyboardNavigation.ControlTabNavigationProperty.OverrideMetadata(typeof(ContextMenu), new FrameworkPropertyMetadata(KeyboardNavigationMode.Contained));
    KeyboardNavigation.DirectionalNavigationProperty.OverrideMetadata(typeof(ContextMenu), new FrameworkPropertyMetadata(KeyboardNavigationMode.Cycle));
    FrameworkElement.FocusVisualStyleProperty.OverrideMetadata(typeof(ContextMenu), new FrameworkPropertyMetadata(null));
    */
}

ObjectPtr ContextMenu::CoerceHorizontalOffset(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ContextMenuService::get_HorizontalOffsetProperty());
}

ObjectPtr ContextMenu::CoerceVerticalOffset(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ContextMenuService::get_VerticalOffsetProperty());
}

ObjectPtr ContextMenu::CoercePlacementTarget(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ContextMenuService::get_PlacementTargetProperty());
}

ObjectPtr ContextMenu::CoercePlacementRectangle(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ContextMenuService::get_PlacementRectangleProperty());
}

ObjectPtr ContextMenu::CoercePlacement(DependencyObject* pDO, Object* pValue)
{
    return PopupControlService::CoerceProperty(pDO, pValue, &ContextMenuService::get_PlacementProperty());
}

ObjectPtr ContextMenu::CoerceHasDropShadow(DependencyObject* pDO, Object* pValue)
{
    ContextMenu* pMenu = (ContextMenu*) pDO;

    if ((pMenu->m_pParentPopup != NULL && pMenu->m_pParentPopup->get_AllowsTransparency())) // && SystemParameters.DropShadow)
    {
        return PopupControlService::CoerceProperty(pDO, pValue, &ContextMenuService::get_HasDropShadowProperty());
    }
    return BoxValue(false);
}

void ContextMenu::Initialize()
{
    MenuItem::SetInsideContextMenuProperty(this, true);
    add_InternalMenuModeChanged(EventHandler::Create(this, &ContextMenu::OnIsMenuModeChanged));
}


void ContextMenu::OnIsOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ContextMenu* pMenu = (ContextMenu*) pDO;
    if (UnboxValue<bool>(e.pNewValue))
    {
        if (pMenu->m_pParentPopup == NULL)
        {
            pMenu->HookupParentPopup();
        }
        pMenu->m_pParentPopup->add_Unloaded(RoutedEventHandler::Create(pMenu, &ContextMenu::OnPopupUnloaded));
        //pMenu->SetValue(KeyboardNavigation.ShowKeyboardCuesProperty, KeyboardNavigation.IsKeyboardMostRecentInputDevice());
    }
    else
    {
        pMenu->ClosingMenu();
    }
}

void ContextMenu::OnIsMenuModeChanged(Object* pSender, EventArgs* pArgs)
{
    if (get_IsMenuMode())
    {
        if (Keyboard::get_Current()->get_FocusedElement() != NULL)
        {
            m_pWeakRefToPreviousFocus = Keyboard::get_Current()->get_FocusedElement();
        }
        Focus();
    }
    else
    {
        set_IsOpen(false);
        if (m_pWeakRefToPreviousFocus != NULL)
        {
            m_pWeakRefToPreviousFocus->Focus();
        }
    }
}

void ContextMenu::HookupParentPopup()
{
    m_pParentPopup = object_allocate<Popup>();
    m_pParentPopup->set_AllowsTransparency(true);
    CoerceValue(get_HasDropShadowProperty());
    m_pParentPopup->set_DropOpposite(false);
    m_pParentPopup->add_Opened(EventHandler::Create(this, &ContextMenu::OnPopupOpened));
    m_pParentPopup->add_Closed(EventHandler::Create(this, &ContextMenu::OnPopupClosed));
    m_pParentPopup->add_PopupCouldClose(EventHandler::Create(this, &ContextMenu::OnPopupCouldClose));
    //this._parentPopup.SetResourceReference(Popup.PopupAnimationProperty, SystemParameters.MenuPopupAnimationKey);
    Popup::CreateRootPopup(m_pParentPopup, this);
}

void ContextMenu::OnPopupClosed(Object* pSender, EventArgs* pArgs)
{
    set_IgnoreNextLeftRelease(false);
    set_IgnoreNextRightRelease(false);
    set_IsMenuMode(false);

    RoutedEventArgs closedArgs(&get_ContextMenuClosedEvent(), this);
    OnClosed(&closedArgs);
}

void ContextMenu::OnPopupUnloaded(Object* pSender, RoutedEventArgs* pArgs)
{
    if (get_IsOpen())
    {
        Dispatcher::get_CurrentDispatcher()->BeginInvoke(DispatcherPriority::Send, 
                                                         DispatcherOperationCallback::Create(this, &ContextMenu::PopupUnloadedAnonymousDelegate), this);
    }
}

void ContextMenu::ClosingMenuAnonymousDelegate(Object* pSender, Object* pArg)
{
    ContextMenu* pElement = (ContextMenu*) pArg;
    if (!pElement->get_IsOpen())
    {
//        FocusManager::SetFocusedElement(pElement, NULL);
    }
}

void ContextMenu::PopupUnloadedAnonymousDelegate(Object* pSender, Object* pArg)
{
    ContextMenu* pMenu = (ContextMenu*) pArg;
    if (pMenu->get_IsOpen())
    {
        pMenu->set_IsOpen(false);
    }
}

void ContextMenu::OnPopupOpened(Object* pSource, EventArgs* pArgs)
{
    if (get_CurrentSelection() != NULL)
    {
        set_CurrentSelection(NULL);
    }
    set_IsMenuMode(true);
    if (Mouse::get_Current()->get_LeftButton() == MouseButtonState::Pressed)
    {
        set_IgnoreNextLeftRelease(true);
    }
    if (Mouse::get_Current()->get_RightButton() == MouseButtonState::Pressed)
    {
        set_IgnoreNextRightRelease(true);
    }

    RoutedEventArgs openedArgs(&get_ContextMenuOpenedEvent(), this);
    OnOpened(&openedArgs);
}

void ContextMenu::OnPopupCouldClose(Object* pSender, EventArgs* pArgs)
{
    set_IsOpen(false);
} 
 
void ContextMenu::ClosingMenu()
{
    if (m_pParentPopup != NULL)
    {
        m_pParentPopup->remove_Unloaded(RoutedEventHandler::Create(this, &ContextMenu::OnPopupUnloaded));

        Dispatcher::get_CurrentDispatcher()->BeginInvoke(DispatcherPriority::Normal, 
                                                         DispatcherOperationCallback::Create(this, &ContextMenu::ClosingMenuAnonymousDelegate), this);
    }
}

 

 

 
 
 

 
 


}; // namespace AVUI

