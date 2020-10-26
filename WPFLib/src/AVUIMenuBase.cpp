#include <AVUICommon.h>
#include <AVUIMenuBase.h>
#include <AVUIMenuItem.h>
#include <AVUISeparator.h>
#include <AVUIItemCollection.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIPopupControlService.h>
#include <AVUIItemCollection.h>
#include <AVUIPopup.h>
#include <AVUIPopupRoot.h>

REGISTER_ELEMENT_BEGIN(AVUI::MenuBase)
    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor);
REGISTER_ELEMENT_END()

namespace AVUI {

void MenuBase::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(MenuBase), MenuItem::get_PreviewClickEvent(), RoutedEventHandler::Create(OnMenuItemPreviewClick), false);
    EventManager::RegisterClassHandler(GET_TYPE(MenuBase), get_IsSelectedChangedEvent(), RoutedPropertyChangedBoolEventHandler::Create(OnIsSelectedChanged), false);
    EventManager::RegisterClassHandler(GET_TYPE(MenuBase), Mouse::get_PreviewMouseDownOutsideCapturedElementEvent(), MouseButtonEventHandler::Create(OnClickThroughThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(MenuBase), Mouse::get_PreviewMouseUpOutsideCapturedElementEvent(), MouseButtonEventHandler::Create(OnClickThroughThunk), false);
}

MenuBase::MenuBase() : m_menuFlags(0)
{

}

TRefCountedPtr<DependencyObject> MenuBase::GetContainerForItemOverride()
{
    return (DependencyObject*)object_allocate<MenuItem>();
}

bool MenuBase::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem->IsType(GET_TYPE(MenuItem)) || pItem->IsType(GET_TYPE(Separator));
}

void MenuBase::OnIsKeyboardFocusWithinChanged(DependencyPropertyChangedEventArgs* pArgs)
{
    ItemsControl::OnIsKeyboardFocusWithinChanged(pArgs);
    if (get_IsKeyboardFocusWithin())
    {
        if (!get_IsMenuMode())
        {
            set_IsMenuMode(true);
            set_OpenOnMouseEnter(false);
        }
        /*
        if (KeyboardNavigation.IsKeyboardMostRecentInputDevice())
        {
            KeyboardNavigation.EnableKeyboardCues(this, true);
        }
        */
    }
    else
    {
        // KeyboardNavigation.EnableKeyboardCues(this, false);
        if (get_IsMenuMode())
        {
            if (get_HasCapture())
            {
                set_IsMenuMode(false);
            }
        }
        else if (get_CurrentSelection() != NULL)
        {
            set_CurrentSelection(NULL);
        }
    }
    // this.InvokeMenuOpenedClosedAutomationEvent(base.IsKeyboardFocusWithin);   
}

void MenuBase::OnKeyDown(KeyEventArgs* pArgs)
{
    ItemsControl::OnKeyDown(pArgs);
    Key::Enum key = pArgs->get_Key();
    if (key != Key::Escape)
    {
        if (key != Key::System)
        {
            return;
        }
    }
    else
    {
        if (get_CurrentSelection() != NULL && get_CurrentSelection()->get_IsSubmenuOpen())
        {
            get_CurrentSelection()->set_IsSubmenuOpen(false);
            set_OpenOnMouseEnter(false);
            pArgs->set_IsHandled(true);
            return;
        }
        KeyboardLeaveMenuMode();
        pArgs->set_IsHandled(true);
        return;
    }
    if (pArgs->get_SystemKey() == Key::LeftAlt || pArgs->get_SystemKey() == Key::RightAlt || pArgs->get_SystemKey() == Key::F10)
    {
        KeyboardLeaveMenuMode();
        pArgs->set_IsHandled(true);
    }
}

void MenuBase::OnMouseLeave(MouseEventArgs* pArgs)
{
    ItemsControl::OnMouseLeave(pArgs);
    if (!get_HasCapture() && !get_IsMouseOver() && get_CurrentSelection() != NULL && !get_CurrentSelection()->get_IsKeyboardFocused() && 
        !get_CurrentSelection()->get_IsSubmenuOpen())
    {
        set_CurrentSelection(NULL);
    }
}

bool MenuBase::ElementIsWithin(DependencyObject* pElement)
{
    DependencyObject* pParent;
    if (pElement == this)
    {
        return true;
    }
    for (MenuItem* pItem = object_cast<MenuItem>(pElement); pItem != NULL; pItem = object_cast<MenuItem>(pParent))
    {
        pParent = pItem->get_Parent();
        if (pParent == NULL)
        {
            pParent = ItemsControl::ItemsControlFromItemContainer(pItem);
        }
        if (pParent == this)
        {
            return true;
        }
    }
    return false;
}

bool MenuBase::IsDescendant(DependencyObject* pNode)
{
    return IsDescendant(this, pNode);
}

bool MenuBase::IsDescendant(DependencyObject* pReference, DependencyObject* pNode)
{
    DependencyObject* pDO = pNode;
    while (pDO != NULL)
    {
        if (pDO == pReference)
        {
            return true;
        }
        PopupRoot* pRoot = object_cast<PopupRoot>(pDO);
        if (pRoot != NULL)
        {
            Popup* pParent = object_cast<Popup>(pRoot->get_Parent());
            pDO = pParent;
            if (pParent != NULL)
            {
                pDO = pParent->get_Parent();
                if (pDO == NULL)
                {
                    pDO = pParent->get_PlacementTarget();
                }
            }
        }
        else
        {
            pDO = PopupControlService::FindParent(pDO);
        }
    }
    return false;
}

void MenuBase::KeyboardLeaveMenuMode()
{
    if (get_IsMenuMode())
    {
        set_IsMenuMode(false);
    }
    else
    {
        set_CurrentSelection(NULL);
        RestorePreviousFocus();
    }
}

void MenuBase::OnClickThrough(MouseButtonEventArgs* pArgs)
{
    if ((pArgs->get_ChangedButton() == MouseButton::Left || (pArgs->get_ChangedButton() == MouseButton::Right) && get_HasCapture()))
    {
        bool flag = true;
        if (pArgs->get_ButtonState() == MouseButtonState::Released)
        {
            if ((pArgs->get_ChangedButton() == MouseButton::Left) && get_IgnoreNextLeftRelease())
            {
                set_IgnoreNextLeftRelease(false);
                flag = false;
            }
            else if ((pArgs->get_ChangedButton() == MouseButton::Right) && get_IgnoreNextRightRelease())
            {
                set_IgnoreNextRightRelease(false);
                flag = false;
            }
        }
        if (flag)
        {
            set_IsMenuMode(false);
        }
    }
}

void MenuBase::OnIsSelectedChanged(Object* pSender, RoutedPropertyChangedBoolEventArgs* pArgs)
{
    MenuItem* pOriginalSource = object_cast<MenuItem>(pArgs->get_OriginalSource());
    if (pOriginalSource != NULL)
    {                                      
        MenuBase* pBase2 = (MenuBase*) pSender;
        if (pArgs->get_NewValue())
        {
            if (pBase2->get_CurrentSelection() != pOriginalSource && pOriginalSource->get_MenuLogicalParent() == pBase2)
            {
                bool isSubmenuOpen = false;
                if (pBase2->get_CurrentSelection() != NULL)
                {
                    isSubmenuOpen = pBase2->get_CurrentSelection()->get_IsSubmenuOpen();
                    pBase2->get_CurrentSelection()->set_IsSubmenuOpen(false);
                }
                pBase2->set_CurrentSelection(pOriginalSource);
                if (pBase2->get_CurrentSelection() != NULL && isSubmenuOpen)
                {
                    MenuItemRole::Enum role = pBase2->get_CurrentSelection()->get_Role();
                    if (((role == MenuItemRole::SubmenuHeader) || (role == MenuItemRole::TopLevelHeader)) && (pBase2->get_CurrentSelection()->get_IsSubmenuOpen() != isSubmenuOpen))
                    {
                        pBase2->get_CurrentSelection()->set_IsSubmenuOpen(isSubmenuOpen);
                    }
                }
            }
        }
        else if (pBase2->get_CurrentSelection() == pOriginalSource)
        {
            pBase2->set_CurrentSelection(NULL);
        }
        pArgs->set_IsHandled(true);
    }
}

void MenuBase::OnLostMouseCapture(Object* pSender, MouseEventArgs* pArgs)
{
    MenuBase* pReference = object_cast<MenuBase>(pSender);
    if (Mouse::get_Captured() != pReference)
    {
        if (pArgs->get_OriginalSource() == pReference)
        {
            if (Mouse::get_Captured() == NULL || !IsDescendant(pReference, (DependencyObject*) (Mouse::get_Captured())))
            {
                pReference->set_IsMenuMode(false);
            }
        }
        else if (IsDescendant(pReference, object_cast<DependencyObject>(pArgs->get_OriginalSource())))
        {
            if (pReference->get_IsMenuMode() && (Mouse::get_Captured() == NULL))
            {
                Mouse::Capture(pReference, CaptureMode::SubTree);
                pArgs->set_IsHandled(true);
            }
        }
        else
        {
            pReference->set_IsMenuMode(false);
        }
    }
}

void MenuBase::OnMenuItemPreviewClick(Object* pSender, RoutedEventArgs* pArgs)
{
    MenuBase* pBase2 = (MenuBase*) pSender;
    MenuItem* pOriginalSource = object_cast<MenuItem>(pArgs->get_OriginalSource());
    if (pOriginalSource != NULL && !pOriginalSource->get_StaysOpenOnClick())
    {
        switch (pOriginalSource->get_Role())
        {
        case MenuItemRole::TopLevelItem:
        case MenuItemRole::SubmenuItem:
            pBase2->set_IsMenuMode(false);
            pArgs->set_IsHandled(true);
            break;
        }
    }
}

void MenuBase::OnPromotedMouseButton(Object* pSender, MouseButtonEventArgs* pArgs)
{
    if (pArgs->get_ChangedButton() == MouseButton::Left)
    {
        pArgs->set_IsHandled(true);
    }
}

void MenuBase::RestorePreviousFocus()
{
    // TODO -- Cross focus scope focus handling
    /*
    if (base.IsKeyboardFocusWithin)
    {
        Keyboard.Focus(null);
    }
    DependencyObject parent = base.Parent;
    if (parent != null)
    {
        DependencyObject focusScope = FocusManager.GetFocusScope(parent);
        if (focusScope != null)
        {
            IInputElement focusedElement = FocusManager.GetFocusedElement(focusScope);
            if (this.ElementIsWithin(focusedElement))
            {
                FocusManager.SetFocusedElement(focusScope, null);
            }
        }
    }
    */
}

void MenuBase::SetSuspendingPopupAnimation(ItemsControl* pMenu, MenuItem* pIgnore, bool suspend)
{
    if (pMenu != NULL)
    {
        int count = pMenu->get_Items()->get_Count();
        for (int i = 0; i < count; i++)
        {
            MenuItem* pItem = object_cast<MenuItem>(pMenu->get_ItemContainerGenerator()->ContainerFromIndex(i));
            if (pItem != NULL && pItem != pIgnore && pItem->get_IsSuspendingPopupAnimation() != suspend)
            {
                pItem->SetValue(MenuItem::get_IsSuspendingPopupAnimationProperty(), BoxValue(suspend));
                if (!suspend)
                {
                    SetSuspendingPopupAnimation(pItem, NULL, suspend);
                }
            }
        }
    }
}

void MenuBase::set_CurrentSelection(MenuItem* pSelection)
{
    bool isKeyboardFocused = false;
    if (m_pCurrentSelection != NULL)
    {
        isKeyboardFocused = m_pCurrentSelection->get_IsKeyboardFocused();
        m_pCurrentSelection->set_IsSelected(false);
    }
    m_pCurrentSelection = pSelection;
    if (m_pCurrentSelection != NULL)
    {
        m_pCurrentSelection->set_IsSelected(true);
        if (isKeyboardFocused)
        {
            m_pCurrentSelection->Focus();
        }
    }
}

bool MenuBase::get_HasCapture()
{
    return Mouse::get_Captured() == this;
}

void MenuBase::set_IsMenuMode(bool isMenuMode)
{
    bool flag = get_IsMenuMode();
    if (flag != isMenuMode)
    {
        flag = isMenuMode;

        m_menuFlags = (isMenuMode ? m_menuFlags | MenuBaseFlags::IsMenuMode : m_menuFlags & ~MenuBaseFlags::IsMenuMode);

        if (flag)
        {
            if (!IsDescendant(this, object_cast<Visual>(Mouse::get_Captured())) && !Mouse::Capture(this, CaptureMode::SubTree))
            {
                flag = false;
                m_menuFlags = m_menuFlags & ~MenuBaseFlags::IsMenuMode;
            }
            else
            {
                invoke_InternalMenuModeChanged(this, NULL);
            }
        }
        if (!flag)
        {
            if (get_CurrentSelection() != NULL)
            {
                bool isSubmenuOpen = get_CurrentSelection()->get_IsSubmenuOpen();
                get_CurrentSelection()->set_IsSubmenuOpen(false);
                set_CurrentSelection(NULL);
            }
            invoke_InternalMenuModeChanged(this, NULL);
            SetSuspendingPopupAnimation(this, NULL, false);
            if (get_HasCapture())
            {
                Mouse::Capture(NULL);
            }
            RestorePreviousFocus();
        }
        set_OpenOnMouseEnter(flag);
    }
}

 

 



}; // namespace AVUI

