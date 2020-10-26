#include <AVUICommon.h>
#include <AVUIMenuItem.h>
#include <AVUIDispatcher.h>
#include <AVUISeparator.h>
#include <AVUIPopup.h>
#include <AVUIToolTipService.h>
#include <AVUIMenuBase.h>
#include <AVUIItemCollection.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIDispatcherTimer.h>
#include <AVUIRoutedPropertyChanged.h>

REGISTER_ELEMENT_BEGIN(AVUI::MenuItem)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsPressed)
    REGISTER_PROPERTY(IsHighlighted)
    REGISTER_PROPERTY(IsSuspendingPopupAnimation)
    REGISTER_PROPERTY(Role)
    REGISTER_PROPERTY(IsCheckable)
    REGISTER_PROPERTY(IsChecked)
    REGISTER_PROPERTY(IsSubmenuOpen)
    REGISTER_PROPERTY(StaysOpenOnClick)
    REGISTER_PROPERTY(InsideContextMenu)

    REGISTER_ROUTED_EVENT(Click)
    REGISTER_ROUTED_EVENT(PreviewClick)
    REGISTER_ROUTED_EVENT(Checked)
    REGISTER_ROUTED_EVENT(Unchecked)
    REGISTER_ROUTED_EVENT(SubmenuOpened)
    REGISTER_ROUTED_EVENT(SubmenuClosed)

    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)

REGISTER_ELEMENT_END()

namespace AVUI {

void MenuItem::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(MenuItem), MenuBase::get_IsSelectedChangedEvent(), RoutedPropertyChangedBoolEventHandler::Create(OnIsSelectedChanged), false);
}


void MenuItem::OnApplyTemplate()
{
    HeaderedItemsControl::OnApplyTemplate();
    if (m_pSubmenuPopup != NULL)
    {
        m_pSubmenuPopup->remove_Closed(EventHandler::Create(this, &MenuItem::OnPopupClosed));
    }
    m_pSubmenuPopup = object_cast<Popup>(GetTemplateChild("PART_Popup"));
    if (m_pSubmenuPopup != NULL)
    {
        m_pSubmenuPopup->add_Closed(EventHandler::Create(this, &MenuItem::OnPopupClosed));
    }
}

TRefCountedPtr<DependencyObject> MenuItem::GetContainerForItemOverride()
{
    return (DependencyObject*)object_allocate<MenuItem>();
}

bool MenuItem::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem->IsType(GET_TYPE(MenuItem)) || pItem->IsType(GET_TYPE(Separator));
}

void MenuItem::PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem)
{
    HeaderedItemsControl::PrepareContainerForItemOverride(pElement, pItem);
    PrepareMenuItem(pElement, pItem);
}

bool MenuItem::ShouldApplyItemContainerStyle(DependencyObject* pContainer, Object* pItem)
{
    if (pItem->IsType(GET_TYPE(Separator)))
    {
        return false;
    }
    return HeaderedItemsControl::ShouldApplyItemContainerStyle(pContainer, pItem);
}

void MenuItem::OnInitialized(EventArgs* pEventArgs)
{
    HeaderedItemsControl::OnInitialized(pEventArgs);
    UpdateRole();
}

void MenuItem::OnItemsChanged(CollectionChangedEventArgs* pArgs)
{
    UpdateRole();
    HeaderedItemsControl::OnItemsChanged(pArgs);
}

void MenuItem::OnKeyDown(KeyEventArgs* pArgs)
{
    HeaderedItemsControl::OnKeyDown(pArgs);
    bool flag = false;
    Key::Enum left = pArgs->get_Key();
    MenuItemRole::Enum role = get_Role();
    if (get_FlowDirection() == FlowDirection::RightToLeft)
    {
        switch (left)
        {
        case Key::Right:
            left = Key::Left;
            break;

        case Key::Left:
            left = Key::Right;
            break;
        }
    }
    switch (left)
    {
    case Key::Left:
    case Key::Escape:
        if (((role != MenuItemRole::TopLevelHeader) && (role != MenuItemRole::TopLevelItem)) && get_IsSubmenuOpen())
        {
            set_IsSubmenuOpen(false);
            flag = true;
        }
        break;

    case Key::Up:
        if (((role == MenuItemRole::SubmenuHeader) && get_IsSubmenuOpen()) && (get_CurrentSelection() == NULL))
        {
            HeaderedItemsControl::NavigateToEnd(object_allocate<ItemNavigateArgs>(pArgs->get_Device(), Keyboard::get_Current()->get_KeyboardModifiers()));
            flag = true;
        }
        break;

    case Key::Right:
        if ((role == MenuItemRole::SubmenuHeader) && !get_IsSubmenuOpen())
        {
            OpenSubmenuWithKeyboard();
            flag = true;
        }
        break;

    case Key::Down:
        if (((role == MenuItemRole::SubmenuHeader) && get_IsSubmenuOpen()) && (get_CurrentSelection() == NULL))
        {
            HeaderedItemsControl::NavigateToStart(object_allocate<ItemNavigateArgs>(pArgs->get_Device(), Keyboard::get_Current()->get_KeyboardModifiers()));
            flag = true;
        }
        break;

    case Key::Tab:
        if (((role == MenuItemRole::SubmenuHeader) && get_IsSubmenuOpen()) && (get_CurrentSelection() == NULL))
        {
            if ((Keyboard::get_Current()->get_KeyboardModifiers() & ModifierKeys::Shift) == ModifierKeys::Shift)
            {
                NavigateToEnd(object_allocate<ItemNavigateArgs>(pArgs->get_Device(), Keyboard::get_Current()->get_KeyboardModifiers()));
            }
            else
            {
                NavigateToStart(object_allocate<ItemNavigateArgs>(pArgs->get_Device(), Keyboard::get_Current()->get_KeyboardModifiers()));
            }
            flag = true;
        }
        break;

    case Key::Return:
        switch (role)
        {
            case MenuItemRole::SubmenuItem:
            case MenuItemRole::TopLevelItem:
                ClickItem(pArgs->get_IsUserInitiated());
                flag = true;
                break;
        }
        if (role == MenuItemRole::TopLevelHeader)
        {
            OpenSubmenuWithKeyboard();
            flag = true;
        }
        else if ((role == MenuItemRole::SubmenuHeader) && !get_IsSubmenuOpen())
        {
            OpenSubmenuWithKeyboard();
            flag = true;
        }
        break;
    }
    if (!flag)
    {
        flag = MenuItemNavigate(pArgs->get_Key(), pArgs->get_Device()->get_KeyboardModifiers());
    }
    if (flag)
    {
        pArgs->set_IsHandled(true);
    }
}

void MenuItem::OnMouseEnter(MouseEventArgs* pArgs)
{
    HeaderedItemsControl::OnMouseEnter(pArgs);
    MouseEnterHelper();
}

void MenuItem::OnMouseLeave(MouseEventArgs* pArgs)
{
    HeaderedItemsControl::OnMouseLeave(pArgs);
    MenuItemRole::Enum role = get_Role();
    if ((((role == MenuItemRole::TopLevelHeader) || (role == MenuItemRole::TopLevelItem)) && get_IsInMenuMode()) || ((role == MenuItemRole::SubmenuHeader) || (role == MenuItemRole::SubmenuItem)))
    {
        MouseLeaveInMenuMode(role);
    }
    else if (get_IsMouseOver() != get_IsSelected())
    {
        set_IsSelected(get_IsMouseOver());
    }
    UpdateIsPressed();
}

void MenuItem::OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled())
    {
        HandleMouseDown(pArgs);
        UpdateIsPressed();
        if (pArgs->get_IsUserInitiated())
        {
            m_userInitiatedPress = true;
        }
    }
    HeaderedItemsControl::OnMouseLeftButtonDown(pArgs);
}

void MenuItem::OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled())
    {
        HandleMouseUp(pArgs);
        UpdateIsPressed();
        m_userInitiatedPress = false;
    }
    HeaderedItemsControl::OnMouseLeftButtonUp(pArgs);
}

void MenuItem::OnMouseMove(MouseEventArgs* pArgs)
{
    MenuItem* pItem = object_cast<MenuItem>(ItemsControl::ItemsControlFromItemContainer(this));
    if (pItem != NULL && pItem->GetBoolField(BoolField::MouseEnterOnMouseMove))
    {
        pItem->SetBoolField(BoolField::MouseEnterOnMouseMove, false);
        MouseEnterHelper();
    }
}

void MenuItem::OnMouseRightButtonDown(MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled())
    {
        HandleMouseDown(pArgs);
        if (pArgs->get_IsUserInitiated())
        {
            m_userInitiatedPress = true;
        }
    }
    HeaderedItemsControl::OnMouseRightButtonDown(pArgs);
}

void MenuItem::OnMouseRightButtonUp(MouseButtonEventArgs* pArgs)
{
    if (!pArgs->get_IsHandled())
    {
        HandleMouseUp(pArgs);
        m_userInitiatedPress = false;
    }
    HeaderedItemsControl::OnMouseRightButtonUp(pArgs);
}

void MenuItem::OnVisualParentChanged(DependencyObject* pOldParent)
{
    HeaderedItemsControl::OnVisualParentChanged(pOldParent);
    UpdateRole();

    /*
    DependencyObject parentInternal = VisualTreeHelper.GetParentInternal(this);
    if (((base.Parent != null) && (parentInternal != null)) && (base.Parent != parentInternal))
    {
        Binding binding = new Binding();
        binding.Path = new PropertyPath(DefinitionBase.PrivateSharedSizeScopeProperty);
        binding.Mode = BindingMode.OneWay;
        binding.Source = parentInternal;
        BindingOperations.SetBinding(this, DefinitionBase.PrivateSharedSizeScopeProperty, binding);
    }
    if (parentInternal == null)
    {
        BindingOperations.ClearBinding(this, DefinitionBase.PrivateSharedSizeScopeProperty);
    }
    */
}

void MenuItem::OnClickImpl(bool userInitiated)
{
    if (get_IsCheckable())
    {
        set_IsChecked(!get_IsChecked());
    }
    if (!get_IsKeyboardFocusWithin())
    {
        FocusOrSelect();
    }
    HeaderedItemsControl::RaiseEvent(object_allocate<RoutedEventArgs>(&get_PreviewClickEvent(), this));

    get_Dispatcher()->BeginInvoke(DispatcherPriority::Render, DispatcherOperationCallback::Create(this, &MenuItem::InvokeClickAfterRender), BoxValue(userInitiated));
}

void MenuItem::InvokeClickAfterRender(Object* pSender, Object* pArg)
{
    bool userInitiated = UnboxValue<bool>(pArg);
    RaiseEvent(object_allocate<RoutedEventArgs>(&get_ClickEvent(), this));

    //CommandHelpers.CriticalExecuteCommandSource(this, userInitiated);
}

void MenuItem::OnPopupClosed(Object* pSource, EventArgs* pArgs)
{
    OnSubmenuClosed(object_allocate<RoutedEventArgs>(&get_SubmenuClosedEvent(), this));
}

void MenuItem::UpdateRole()
{
    MenuItemRole::Enum topLevelHeader;
    if (!get_IsCheckable() && get_HasItems())
    {
        if (get_MenuLogicalParent() != NULL && get_MenuLogicalParent()->IsType(GET_TYPE(Menu)))
        {
            topLevelHeader = MenuItemRole::TopLevelHeader;
        }
        else
        {
            topLevelHeader = MenuItemRole::SubmenuHeader;
        }
    }
    else if (get_MenuLogicalParent() != NULL && get_MenuLogicalParent()->IsType(GET_TYPE(Menu)))
    {
        topLevelHeader = MenuItemRole::TopLevelItem;
    }
    else
    {
        topLevelHeader = MenuItemRole::SubmenuItem;
    }

    SetValue(get_RoleProperty(), BoxValue<int>(topLevelHeader));
}

Object* MenuItem::get_MenuLogicalParent()
{
    if (get_Parent() != NULL)
    {
        return get_Parent();
    }
    return ItemsControl::ItemsControlFromItemContainer(this);
}

void MenuItem::SetBoolFieldAnonymousDelegate(Object* pSender, Object* pArg)
{
    SetBoolField(BoolField::IgnoreMouseEvents, false);
}


void MenuItem::OnIsSubmenuOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    TRefCountedPtr<DispatcherOperationCallback> pMethod = NULL;
    MenuItem* pMenuItem = (MenuItem*) pDO;
    bool oldValue = UnboxValue<bool>(e.pOldValue);
    bool newValue = UnboxValue<bool>(e.pNewValue);

    pMenuItem->StopTimer(&(pMenuItem->m_pOpenHierarchyTimer));
    pMenuItem->StopTimer(&(pMenuItem->m_pCloseHierarchyTimer));

    if (newValue)
    {
        /*
        CommandManager.InvalidateRequerySuggested();
        */
        pMenuItem->set_IsSelected(true);
        if (pMenuItem->get_Role() == MenuItemRole::TopLevelHeader)
        {
            pMenuItem->SetMenuMode(true);
        }
        pMenuItem->set_CurrentSelection(NULL);
        pMenuItem->NotifySiblingsToSuspendAnimation();
        for (int i = 0; i < pMenuItem->get_Items()->get_Count(); i++)
        {
            MenuItem* pItem = object_cast<MenuItem>(pMenuItem->get_ItemContainerGenerator()->ContainerFromIndex(i));
            if (pItem != NULL && pItem->GetBoolField(BoolField::CanExecuteInvalid))
            {
                pItem->UpdateCanExecute();
            }
        }
        pMenuItem->OnSubmenuOpened(object_allocate<RoutedEventArgs>(&get_SubmenuOpenedEvent(), pMenuItem));
        pMenuItem->SetBoolField(BoolField::IgnoreMouseEvents, true);
        pMenuItem->SetBoolField(BoolField::MouseEnterOnMouseMove, false);
        if (pMethod == NULL)
        {
            pMethod = DispatcherOperationCallback::Create(pMenuItem, &MenuItem::SetBoolFieldAnonymousDelegate);
        }
        pMenuItem->get_Dispatcher()->BeginInvoke(DispatcherPriority::Background, pMethod, NULL);
    }
    else
    {
        if (pMenuItem->get_CurrentSelection() != NULL)
        {
            if (pMenuItem->get_CurrentSelection()->get_IsKeyboardFocusWithin())
            {
                pMenuItem->Focus();
            }
            if (pMenuItem->get_CurrentSelection()->get_IsSubmenuOpen())
            {
                pMenuItem->get_CurrentSelection()->set_IsSubmenuOpen(false);
            }
        }
        else if (pMenuItem->get_IsKeyboardFocusWithin())
        {
            pMenuItem->Focus();
        }
        pMenuItem->set_CurrentSelection(NULL);
        if (pMenuItem->get_IsMouseOver() && (pMenuItem->get_Role() == MenuItemRole::SubmenuHeader))
        {
            pMenuItem->SetBoolField(BoolField::IgnoreNextMouseLeave, true);
        }
        pMenuItem->NotifyChildrenToResumeAnimation();
        if (pMenuItem->m_pSubmenuPopup == NULL)
        {
            pMenuItem->OnSubmenuClosed(object_allocate<RoutedEventArgs>(&get_SubmenuClosedEvent(), pMenuItem));
        }
    }
    pMenuItem->CoerceValue(ToolTipService::get_IsEnabledProperty());
}

ObjectPtr MenuItem::CoerceIsSubmenuOpen(DependencyObject* pDO, Object* pValue)
{
    if (UnboxValue<bool>(pValue))
    {
        MenuItem* pItem = (MenuItem*) pDO;
        if (!pItem->get_IsLoaded())
        {
            pItem->RegisterToOpenOnLoad();
            return BoxValue<bool>(false);
        }
    }
    return pValue;
}


void MenuItem::set_CurrentSelection(MenuItem* pValue)
{
    if (m_pCurrentSelection != NULL)
    {
        m_pCurrentSelection->set_IsSelected(false);
    }
    m_pCurrentSelection = pValue;
    if (m_pCurrentSelection != NULL)
    {
        m_pCurrentSelection->set_IsSelected(true);
    }
}

void MenuItem::OnIsSelectedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    MenuItem* pItem = (MenuItem*) pDO;
    pItem->SetValue(get_IsHighlightedProperty(), e.pNewValue);
    if (UnboxValue<bool>(e.pOldValue))
    {
        if (pItem->get_IsSubmenuOpen())
        {
            pItem->set_IsSubmenuOpen(false);
        }
        pItem->StopTimer(&(pItem->m_pOpenHierarchyTimer));
        pItem->StopTimer(&(pItem->m_pCloseHierarchyTimer));
    }
    pItem->RaiseEvent(object_allocate<RoutedPropertyChangedBoolEventArgs>(UnboxValue<bool>(e.pOldValue), 
                                                                          UnboxValue<bool>(e.pNewValue), 
                                                                          &MenuBase::get_IsSelectedChangedEvent()));
}

void MenuItem::StopTimer(TRefCountedPtr<DispatcherTimer> *ppTimer)
{
    if (*ppTimer != NULL)
    {
        (*ppTimer)->Stop();
        (*ppTimer) = NULL;
    }
}

void MenuItem::PrepareMenuItem(DependencyObject* pElement, Object* pItem)
{
    /*
    MenuItem* pItem2 = object_cast<MenuItem>(pElement);
    if (pItem2 != NULL)
    {
        ICommand* pCommand = interface_cast<ICommand>(pItem);
        if (pCommand != NULL && !pItem2->HasNonDefaultValue(get_CommandProperty()))
        {
            pItem2->set_Command(pItem);
        }
        if (pItem2->GetBoolField(BoolField::CanExecuteInvalid))
        {
            pItem2->UpdateCanExecute();
        }
    }
    else
    {
        Separator* pSeparator = object_cast<Separator>(pItem);
        if (pSeparator != NULL)
        {
            bool flag;
            if (pSeparator->GetValueSource(FrameworkElement::get_StyleProperty(), NULL, &flag) <= BaseValueSource::ImplicitReference)
            {
                pSeparator->SetResourceReference(FrameworkElement::get_StyleProperty(), get_SeparatorStyleKey());
            }
            pSeparator->set_DefaultStyleKey(get_SeparatorStyleKey());
        }
    }
    */
}

void MenuItem::RegisterToOpenOnLoad()
{
    add_Loaded(RoutedEventHandler::Create(this, &MenuItem::OpenOnLoad));
}

void MenuItem::CoerceIsSubmenuOpenAnonymousDelegate(Object* pSender, Object* pArg)
{
    CoerceValue(get_IsSubmenuOpenProperty());
}

void MenuItem::OpenOnLoad(Object* pSender, RoutedEventArgs* pArgs)
{
    get_Dispatcher()->BeginInvoke(DispatcherPriority::Input, 
                                  DispatcherOperationCallback::Create(this, &MenuItem::CoerceIsSubmenuOpenAnonymousDelegate),
                                  NULL);
}
 
void MenuItem::NotifyChildrenToResumeAnimation()
{
    MenuBase::SetSuspendingPopupAnimation(this, NULL, false);
}

void MenuItem::SetIsSuspendingAnonymousDelegate(Object* pSender, Object* pArg)
{
    SetValue(get_IsSuspendingPopupAnimationProperty(), BoxValue(true));
}


void MenuItem::NotifySiblingsToSuspendAnimation()
{
    if (!get_IsSuspendingPopupAnimation())
    {
        bool boolField = GetBoolField(BoolField::OpenedWithKeyboard);
        MenuItem* pIgnore = boolField ? NULL : this;
        MenuBase::SetSuspendingPopupAnimation(ItemsControl::ItemsControlFromItemContainer(this), pIgnore, true);
        if (!boolField)
        {
            get_Dispatcher()->BeginInvoke(DispatcherPriority::Input, DispatcherOperationCallback::Create(this, &MenuItem::SetIsSuspendingAnonymousDelegate), NULL);
        }
        else
        {
            SetBoolField(BoolField::OpenedWithKeyboard, false);
        }
    }
}

void MenuItem::SetMenuMode(bool menuMode)
{
    MenuBase* pLogicalParent = object_cast<MenuBase>(get_MenuLogicalParent());
    if (pLogicalParent != NULL && pLogicalParent->get_IsMenuMode() != menuMode)
    {
        pLogicalParent->set_IsMenuMode(menuMode);
    }
}

void MenuItem::UpdateCanExecute()
{
    SetBoolField(BoolField::CanExecuteInvalid, false);
    /*
    if (get_Command() != NULL)
    {
        MenuItem* pItem = object_cast<MenuItem>(ItemsControl::ItemsControlFromItemContainer(this));
        if (pItem == NULL || pItem->get_IsSubmenuOpen())
        {
            this.CanExecute = CommandHelpers.CanExecuteCommandSource(this);
        }
        else
        {
            this.CanExecute = true;
            SetBoolField(BoolField::CanExecuteInvalid, true);
        }
    }
    else
    {
         this.CanExecute = true;
    }
    */
}

void MenuItem::FocusOrSelect()
{
    if (!get_IsKeyboardFocusWithin())
    {
        Focus();
    }
    if (!get_IsSelected())
    {
        set_IsSelected(true);
    }
    if (get_IsSelected() && !get_IsHighlighted())
    {
        set_IsHighlighted(true);
    }
}

void MenuItem::HandleMouseDown(MouseButtonEventArgs* pArgs)
{
    Rect rect(Point(), get_RenderSize());
    if (rect.Contains(pArgs->GetPosition(this)) && ((pArgs->get_ChangedButton() == MouseButton::Left) || 
                                                    ((pArgs->get_ChangedButton() == MouseButton::Right) && get_InsideContextMenu())))
    {
        switch (get_Role())
        {
        case MenuItemRole::TopLevelHeader:
        case MenuItemRole::SubmenuHeader:
            ClickHeader();
            break;
        }
    }
    pArgs->set_IsHandled(true);
}

void MenuItem::HandleMouseUp(MouseButtonEventArgs* pArgs)
{
    Rect rect(Point(), get_RenderSize());
    if (rect.Contains(pArgs->GetPosition(this)) && ((pArgs->get_ChangedButton() == MouseButton::Left) || 
                                                    ((pArgs->get_ChangedButton() == MouseButton::Right) && get_InsideContextMenu())))
    {
        switch (get_Role())
        {
        case MenuItemRole::TopLevelItem:
        case MenuItemRole::SubmenuItem:
            if (m_userInitiatedPress)
            {
                ClickItem(pArgs->get_IsUserInitiated());
            }
            else
            {
                ClickItem(false);
            }
            break;
        }
    }
    if ((pArgs->get_ChangedButton() != MouseButton::Right) || get_InsideContextMenu())
    {
        pArgs->set_IsHandled(true);
    }
}



void MenuItem::ClickHeader()
{
    if (!get_IsKeyboardFocusWithin())
    {
        FocusOrSelect();
    }
    if (get_IsSubmenuOpen())
    {
        if (get_Role() == MenuItemRole::TopLevelHeader)
        {
            SetMenuMode(false);
        }
    }
    else
    {
        OpenMenu();
    }
}

bool MenuItem::OpenMenu()
{
    if (!get_IsSubmenuOpen())
    {
        ItemsControl* pParent = ItemsControl::ItemsControlFromItemContainer(this);
        if (pParent == NULL)
        {
            pParent = object_cast<ItemsControl>(get_VisualParent());
        }
        if (pParent != NULL && (pParent->IsType(GET_TYPE(MenuItem)) || pParent->IsType(GET_TYPE(MenuBase))))
        {
            set_IsSubmenuOpen(true);
            return true;
        }
    }
    return false;
}

void MenuItem::ClickItem()
{
    ClickItem(false);
}

void MenuItem::ClickItem(bool userInitiated)
{
    OnClickCore(userInitiated);
    if (get_Role() == MenuItemRole::TopLevelItem)
    {
        SetMenuMode(false);
    }
}
 
void MenuItem::MouseEnterHelper()
{
    MenuItem* pItem = object_cast<MenuItem>(ItemsControl::ItemsControlFromItemContainer(this));
    if (pItem == NULL || !pItem->GetBoolField(BoolField::IgnoreMouseEvents))
    {
        MenuItemRole::Enum role = get_Role();
        if (((role == MenuItemRole::TopLevelHeader || role == MenuItemRole::TopLevelItem) && get_OpenOnMouseEnter()) || (role == MenuItemRole::SubmenuHeader || role == MenuItemRole::SubmenuItem))
        {
            MouseEnterInMenuMode(role);
        }
        else if (get_IsMouseOver() != get_IsSelected())
        {
            set_IsSelected(get_IsMouseOver());
        }
        UpdateIsPressed();
    }
    else
    {
        pItem->SetBoolField(BoolField::MouseEnterOnMouseMove, true);
    }
}

 
void MenuItem::UpdateIsPressed()
{
    Rect rect(Point(), get_RenderSize());
    if (((Mouse::get_Current()->get_LeftButton() == MouseButtonState::Pressed) && get_IsMouseOver()) && rect.Contains(Mouse::get_Current()->get_MousePosition(this)))
    {
        SetValue(get_IsPressedProperty(), BoxValue(true));
    }
    else
    {
        ClearValue(get_IsPressedProperty());
    }
}

void MenuItem::OpenSubmenuWithKeyboard()
{
    SetBoolField(BoolField::OpenedWithKeyboard, true);
    if (OpenMenu())
    {
        NavigateToStart(object_allocate<ItemNavigateArgs>(Keyboard::get_Current(), Keyboard::get_Current()->get_KeyboardModifiers()));
    }
}

bool MenuItem::MenuItemNavigate(Key::Enum key, ModifierKeys::Enum modifiers)
{
    if (key == Key::Left || key == Key::Right || key == Key::Up || key == Key::Down)
    {
        ItemsControl* pControl = ItemsControl::ItemsControlFromItemContainer(this);
        if (pControl != NULL)
        {
            if (!pControl->get_HasItems())
            {
                return false;
            }
            /* // This looks like a WPF bug
            if (pControl->get_Items()->get_Count() == 1 && !(pControl->IsType(GET_TYPE(Menu))) && key == Key::Up && (key == Key.Down)))
            {
                return true;
            }
            */

            Object* pFocusedElement = Keyboard::get_Current()->get_FocusedElement();
            pControl->NavigateByLine(KeyboardNavigation::KeyToTraversalDirection(key), object_allocate<ItemNavigateArgs>(Keyboard::get_Current(), modifiers));
            Object* pObj3 = Keyboard::get_Current()->get_FocusedElement();
            if (pObj3 != pFocusedElement && pObj3 != this)
            {
                return true;
            }
        }
    }
    return false;
}

bool MenuItem::get_OpenOnMouseEnter()
{
    MenuBase* pLogicalParent = object_cast<MenuBase>(get_MenuLogicalParent());
    return pLogicalParent != NULL && pLogicalParent->get_OpenOnMouseEnter();
}
 

void MenuItem::MouseEnterInMenuMode(MenuItemRole::Enum role)
{
    switch (role)
    {
    case MenuItemRole::TopLevelItem:
    case MenuItemRole::TopLevelHeader:
            if (!get_IsSubmenuOpen())
            {
                OpenHierarchy(role);
            }
            break;

    case MenuItemRole::SubmenuItem:
    case MenuItemRole::SubmenuHeader:
        {
            MenuItem* pCurrentSibling = get_CurrentSibling();
            if (pCurrentSibling != NULL && pCurrentSibling->get_IsSubmenuOpen())
            {
                pCurrentSibling->set_IsHighlighted(false);
                set_IsHighlighted(true);

                if (!get_IsSelected() || !get_IsSubmenuOpen())
                {
                    SetTimerToOpenHierarchy();
                }

                break;
            }
            if (!get_IsSubmenuOpen())
            {
                FocusOrSelect();
            }
            else
            {
                set_IsHighlighted(true);
            }

            if (!get_IsSelected() || !get_IsSubmenuOpen())
            {
                SetTimerToOpenHierarchy();
            }

            break;
        }
    }
    StopTimer(&m_pCloseHierarchyTimer);
}
 
void MenuItem::OpenHierarchy(MenuItemRole::Enum role)
{
    FocusOrSelect();
    if (role == MenuItemRole::TopLevelHeader || role == MenuItemRole::SubmenuHeader)
    {
        OpenMenu();
    }
}

void MenuItem::MouseLeaveInMenuMode(MenuItemRole::Enum role)
{
    if ((role == MenuItemRole::SubmenuHeader) || (role == MenuItemRole::SubmenuItem))
    {
        if (GetBoolField(BoolField::IgnoreNextMouseLeave))
        {
            SetBoolField(BoolField::IgnoreNextMouseLeave, false);
        }
        else if (!get_IsSubmenuOpen())
        {
            if (get_IsSelected())
            {
                set_IsSelected(false);
            }
            else
            {
                set_IsHighlighted(false);
            }
            if (get_IsKeyboardFocusWithin())
            {
                ItemsControl* pControl = ItemsControl::ItemsControlFromItemContainer(this);
                if (pControl != NULL)
                {
                    pControl->Focus();
                }
            }
        }
        else if (get_IsMouseOverSibling())
        {
            SetTimerToCloseHierarchy();
        }
    }
    StopTimer(&m_pOpenHierarchyTimer);
}

bool MenuItem::get_IsInMenuMode()
{
    MenuBase* pLogicalParent = object_cast<MenuBase>(get_MenuLogicalParent());
    return pLogicalParent != NULL && pLogicalParent->get_IsMenuMode();
}
 
bool MenuItem::get_IsMouseOverSibling()
{
    UIElement* pLogicalParent = object_cast<UIElement>(get_MenuLogicalParent());
    return pLogicalParent != NULL && IsMouseReallyOver(pLogicalParent) && !get_IsMouseOver();
}
 
bool MenuItem::IsMouseReallyOver(UIElement* pElement)
{
    bool isMouseOver = pElement->get_IsMouseOver();
    if (isMouseOver && Mouse::get_Captured() == pElement && Mouse::get_Current()->get_DirectlyOver() == pElement)
    {
        return false;
    }
    return isMouseOver;
}

 
MenuItem* MenuItem::get_CurrentSibling()
{
    Object* pLogicalParent = get_MenuLogicalParent();
    MenuItem* pItem = object_cast<MenuItem>(pLogicalParent);
    MenuItem* pCurrentSelection = NULL;
    if (pItem != NULL)
    {
        pCurrentSelection = pItem->get_CurrentSelection();
    }
    else
    {
        MenuBase* pBase2 = object_cast<MenuBase>(pLogicalParent);
        if (pBase2 != NULL)
        {
            pCurrentSelection = pBase2->get_CurrentSelection();
        }
    }

    if (pCurrentSelection == this)
    {
        pCurrentSelection = NULL;
    }
    return pCurrentSelection;
}

void MenuItem::OpenHierarchyAnonymousDelegate(Object* pSender, EventArgs* pArg)
{
    OpenHierarchy(get_Role());
    StopTimer(&m_pOpenHierarchyTimer);
}

void MenuItem::SetTimerToOpenHierarchy()
{
    TRefCountedPtr<EventHandler> pHandler = NULL;
    if (m_pOpenHierarchyTimer == NULL)
    {
        m_pOpenHierarchyTimer = object_allocate<DispatcherTimer>(DispatcherPriority::Normal);
        if (pHandler == NULL)
        {
            pHandler = EventHandler::Create(this, &MenuItem::OpenHierarchyAnonymousDelegate);
        }
        m_pOpenHierarchyTimer->add_Tick(pHandler);
    }
    else
    {
        m_pOpenHierarchyTimer->Stop();
    }
    StartTimer(m_pOpenHierarchyTimer);
}

void MenuItem::StartTimer(DispatcherTimer* pTimer)
{
    pTimer->set_Interval(TimeSpan::FromMilliseconds(50)); //(double) SystemParameters.MenuShowDelay);
    pTimer->Start();
}

void MenuItem::CloseHierarchyAnonymousDelegate(Object* pSender, EventArgs* pArg)
{
    set_IsSelected(false);
    StopTimer(&m_pCloseHierarchyTimer);
}

void MenuItem::SetTimerToCloseHierarchy()
{
    TRefCountedPtr<EventHandler> pHandler = NULL;
    if (m_pCloseHierarchyTimer == NULL)
    {
        m_pCloseHierarchyTimer = object_allocate<DispatcherTimer>(DispatcherPriority::Normal);
        if (pHandler == NULL)
        {
            pHandler = EventHandler::Create(this, &MenuItem::CloseHierarchyAnonymousDelegate);
        }
        m_pCloseHierarchyTimer->add_Tick(pHandler);
    }
    else
    {
        m_pCloseHierarchyTimer->Stop();
    }
    StartTimer(m_pCloseHierarchyTimer);
}

 
void MenuItem::OnIsSelectedChanged(Object* pSender, RoutedPropertyChangedBoolEventArgs* pArgs)
{
    if (pSender != pArgs->get_OriginalSource())
    {
        MenuItem* pItem = (MenuItem*) pSender;
        MenuItem* pOriginalSource = object_cast<MenuItem>(pArgs->get_OriginalSource());
        if (pOriginalSource != NULL)
        {
            if (pArgs->get_NewValue())
            {
                if (pItem->get_CurrentSelection() == pOriginalSource)
                {
                    pItem->StopTimer(&(pItem->m_pCloseHierarchyTimer));
                }
                if (pItem->get_CurrentSelection() != pOriginalSource && pOriginalSource->get_MenuLogicalParent() == pItem)
                {
                    if (pItem->get_CurrentSelection() != NULL && pItem->get_CurrentSelection()->get_IsSubmenuOpen())
                    {
                        pItem->get_CurrentSelection()->set_IsSubmenuOpen(false);
                    }
                    pItem->set_CurrentSelection(pOriginalSource);
                }
            }
            else if (pItem->get_CurrentSelection() == pOriginalSource)
            {
                pItem->set_CurrentSelection(NULL);
            }
            pArgs->set_IsHandled(true);
        }
    }
}

 
 

 
 

 
 

 
 

 



 
 

 

 

 

 

 
 

 
 

 
 

 



 
 

 
 

 
 

 

 
 



}; // namespace AVUI


