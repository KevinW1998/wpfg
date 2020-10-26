#pragma once

#include <AVUIRoutedPropertyChanged.h>
#include <AVUIHeaderedItemsControl.h>
#include <AVUISelector.h>

namespace AVUI {

class Popup;
class DispatcherTimer;
class Menu;

class MenuItem : public HeaderedItemsControl
{
    friend class MenuBase;
    friend class ContextMenu;
public:

    MenuItem() : m_userInitiatedPress(false) { }

    DECLARE_ELEMENT(MenuItem, HeaderedItemsControl);

    virtual void OnApplyTemplate();


    IMPLEMENT_ROUTED_EVENT(Click, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(PreviewClick, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Checked, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Unchecked, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(SubmenuOpened, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(SubmenuClosed, RoutedEventArgs, RoutingStrategy::Bubble);

    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsPressed, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsHighlighted, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsSuspendingPopupAnimation, bool, false, PropertyFlag::None);

    IMPLEMENT_READONLY_ENUM_DEPENDENCY_PROPERTY(Role, MenuItemRole, MenuItemRole::TopLevelItem, PropertyFlag::None);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCheckable, bool, false, DependencyPropertyMetadata(PropertyFlag::None, MenuItem::OnIsCheckableChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsChecked, bool, false, DependencyPropertyMetadata(PropertyFlag::None, MenuItem::OnIsCheckedChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsSubmenuOpen, bool, false, DependencyPropertyMetadata(PropertyFlag::None, MenuItem::OnIsSubmenuOpenChanged, MenuItem::CoerceIsSubmenuOpen));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(StaysOpenOnClick, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(InsideContextMenu, bool, false, PropertyFlag::IsAttachedProperty | PropertyFlag::Inherits);

    /*
    CommandProperty = ButtonBase.CommandProperty.AddOwner(typeof(MenuItem), new FrameworkPropertyMetadata(null, new PropertyChangedCallback(MenuItem.OnCommandChanged)));
    CommandParameterProperty = ButtonBase.CommandParameterProperty.AddOwner(typeof(MenuItem), new FrameworkPropertyMetadata(null));
    CommandTargetProperty = ButtonBase.CommandTargetProperty.AddOwner(typeof(MenuItem), new FrameworkPropertyMetadata(null));
    IsSelectedProperty = Selector.IsSelectedProperty.AddOwner(typeof(MenuItem), new FrameworkPropertyMetadata(BooleanBoxes.FalseBox, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault, new PropertyChangedCallback(MenuItem.OnIsSelectedChanged)));
    InputGestureTextProperty = DependencyProperty.Register("InputGestureText", typeof(string), typeof(MenuItem), new FrameworkPropertyMetadata(string.Empty, new PropertyChangedCallback(MenuItem.OnInputGestureTextChanged), new CoerceValueCallback(MenuItem.CoerceInputGestureText)));
    IconProperty = DependencyProperty.Register("Icon", typeof(object), typeof(MenuItem), new FrameworkPropertyMetadata(null));
    InsideContextMenuProperty = DependencyProperty.RegisterAttached("InsideContextMenu", typeof(bool), typeof(MenuItem), new FrameworkPropertyMetadata(BooleanBoxes.FalseBox, FrameworkPropertyMetadataOptions.Inherits));
    CanExecuteChangedHandler = new UncommonField<EventHandler>();
    HeaderedItemsControl.HeaderProperty.OverrideMetadata(typeof(MenuItem), new FrameworkPropertyMetadata(null, new CoerceValueCallback(MenuItem.CoerceHeader)));
    EventManager.RegisterClassHandler(typeof(MenuItem), AccessKeyManager.AccessKeyPressedEvent, new AccessKeyPressedEventHandler(MenuItem.OnAccessKeyPressed));
    EventManager.RegisterClassHandler(typeof(MenuItem), MenuBase.IsSelectedChangedEvent, new RoutedPropertyChangedEventHandler<bool>(MenuItem.OnIsSelectedChanged));
    ToolTipService.IsEnabledProperty.OverrideMetadata(typeof(MenuItem), new FrameworkPropertyMetadata(null, new CoerceValueCallback(MenuItem.CoerceToolTipIsEnabled)));
    KeyboardNavigation.DirectionalNavigationProperty.OverrideMetadata(typeof(MenuItem), new FrameworkPropertyMetadata(KeyboardNavigationMode.None));
    FrameworkElement.FocusVisualStyleProperty.OverrideMetadata(typeof(MenuItem), new FrameworkPropertyMetadata(null));
    InputMethod.IsInputMethodSuspendedProperty.OverrideMetadata(typeof(MenuItem), new FrameworkPropertyMetadata(BooleanBoxes.TrueBox, FrameworkPropertyMetadataOptions.Inherits));
    */


protected:

    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();
    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual void PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem);
    virtual bool ShouldApplyItemContainerStyle(DependencyObject* pContainer, Object* pItem);
    virtual void OnChecked(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnClick() { OnClickImpl(false); }

    virtual void OnInitialized(EventArgs* pEventArgs);
    virtual void OnItemsChanged(CollectionChangedEventArgs* pArgs);
    virtual void OnKeyDown(KeyEventArgs* pArgs);
    virtual void OnMouseEnter(MouseEventArgs* pArgs);
    virtual void OnMouseLeave(MouseEventArgs* pArgs);
    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs);
    virtual void OnMouseMove(MouseEventArgs* pArgs);
    virtual void OnMouseRightButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnMouseRightButtonUp(MouseButtonEventArgs* pArgs);

    virtual void OnSubmenuClosed(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnSubmenuOpened(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnUnchecked(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnVisualParentChanged(DependencyObject* pOldParent);

private:

    static void SetInsideContextMenuProperty(UIElement* pElement, bool value)
    {
        pElement->SetValue(get_InsideContextMenuProperty(), BoxValue(value));
    } 

    static void StaticConstructor();
    static void OnIsSelectedChanged(Object* pSender, RoutedPropertyChangedBoolEventArgs* pArgs);

    bool get_OpenOnMouseEnter();
    bool MenuItemNavigate(Key::Enum key, ModifierKeys::Enum modifiers);
    void MouseEnterHelper();
    void ClickItem(bool userInitiated);
    void ClickItem();
    bool OpenMenu();
    void ClickHeader();
    void HandleMouseUp(MouseButtonEventArgs* pArgs);
    void HandleMouseDown(MouseButtonEventArgs* pArgs);
    void NotifySiblingsToSuspendAnimation();
    void NotifyChildrenToResumeAnimation();
    void SetMenuMode(bool menuMode);
    void UpdateCanExecute();
    void FocusOrSelect();
    void UpdateIsPressed();
    void OpenSubmenuWithKeyboard();

    void OpenOnLoad(Object* pSender, RoutedEventArgs* pArgs);
    void RegisterToOpenOnLoad();
    static void PrepareMenuItem(DependencyObject* pElement, Object* pItem);

    static const AVUI::DependencyProperty& get_IsSelectedProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::None, OnIsSelectedChanged));
        const static AVUI::DependencyProperty& property = Selector::get_IsSelectedProperty().AddOwner(GET_TYPE(_thisType), BoxValue(false), &metadata);

        return property;
    }
    bool get_IsSelected() { return UnboxValue< bool >(GetValue(get_IsSelectedProperty())); }
    void set_IsSelected(const bool& value) { SetValue(get_IsSelectedProperty(), AVUI::BoxValue(value)); }

    static void OnIsSelectedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);


    Object* get_MenuLogicalParent();

    MenuItem* get_CurrentSelection() { return m_pCurrentSelection; }
    void set_CurrentSelection(MenuItem* pValue);

    class BoolField
    {
    public:
        enum Enum
        {
            OpenedWithKeyboard = 1,
            IgnoreNextMouseLeave = 2,
            IgnoreMouseEvents = 4,
            MouseEnterOnMouseMove = 8,
            CanExecuteInvalid = 0x10
        };
    };

    bool GetBoolField(unsigned int field)
    {
        return ((m_menuItemBoolFieldStore & field) != 0);
    }

    void SetBoolField(unsigned int field, bool value)
    {
        if (value)
        {
            m_menuItemBoolFieldStore |= field;
        }
        else
        {
            m_menuItemBoolFieldStore &= ~field;
        }
    }

    void OnPopupClosed(Object* pSource, EventArgs* pArgs); 

    void InvokeClickAfterRender(Object* pSender, Object* pArg);
    virtual void OnClickCore(bool userInitiated) { OnClick(); }
    void OnClickImpl(bool userInitiated);

    static void OnIsCheckableChanged(Object* pTarget, const DependencyPropertyChangedEventArgs& e)
    {
        ((MenuItem*) pTarget)->UpdateRole();
    }

    static void OnIsCheckedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
    {
        MenuItem* pItem = (MenuItem*) pDO;
        if (UnboxValue<bool>(e.pNewValue))
        {
            pItem->OnChecked(object_allocate<RoutedEventArgs>(&get_CheckedEvent()));
        }
        else
        {
            pItem->OnUnchecked(object_allocate<RoutedEventArgs>(&get_UncheckedEvent()));
        }
    }

    static void OnIsSubmenuOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static ObjectPtr CoerceIsSubmenuOpen(DependencyObject* pDO, Object* pValue);

    void UpdateRole();
    void StopTimer(TRefCountedPtr<DispatcherTimer> *ppTimer);
    void MouseEnterInMenuMode(MenuItemRole::Enum role);
    void set_IsHighlighted(bool isHighlighted) { SetValue(get_IsHighlightedProperty(), BoxValue(isHighlighted)); }
    void OpenHierarchy(MenuItemRole::Enum role);
    void MouseLeaveInMenuMode(MenuItemRole::Enum role);

    bool get_IsInMenuMode();
    bool get_IsMouseOverSibling();
    static bool IsMouseReallyOver(UIElement* pElement);
    MenuItem* get_CurrentSibling();
    void SetTimerToOpenHierarchy();
    void StartTimer(DispatcherTimer* pTimer);
    void SetTimerToCloseHierarchy();

    void SetBoolFieldAnonymousDelegate(Object* pSender, Object* pArg);
    void CoerceIsSubmenuOpenAnonymousDelegate(Object* pSender, Object* pArg);
    void SetIsSuspendingAnonymousDelegate(Object* pSender, Object* pArg);
    void OpenHierarchyAnonymousDelegate(Object* pSender, EventArgs* pArg);
    void CloseHierarchyAnonymousDelegate(Object* pSender, EventArgs* pArg);


    TRefCountedPtr<Popup> m_pSubmenuPopup;
    bool m_userInitiatedPress;
    TRefCountedPtr<DispatcherTimer> m_pOpenHierarchyTimer;
    TRefCountedPtr<DispatcherTimer> m_pCloseHierarchyTimer;
    unsigned int m_menuItemBoolFieldStore;
    TRefCountedPtr<MenuItem> m_pCurrentSelection;

};


}; // namespace AVUI


