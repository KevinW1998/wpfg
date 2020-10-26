#pragma once

#include <AVUIItemsControl.h>
#include <AVUIRoutedPropertyChanged.h>

namespace AVUI {

class MenuItem;
             
class MenuBase : public ItemsControl
{
    friend class ComboBox;
    friend class Popup;
    friend class MenuItem;
    friend class ContextMenu;
public:

    DECLARE_ELEMENT(MenuBase, ItemsControl);

protected:
    MenuBase();

    virtual void HandleMouseButton(MouseButtonEventArgs* pArgs) { };
    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();
    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual void OnIsKeyboardFocusWithinChanged(DependencyPropertyChangedEventArgs* pArgs);
    virtual void OnKeyDown(KeyEventArgs* pArgs);
    virtual void OnMouseLeave(MouseEventArgs* pArgs);

private:

    static void StaticConstructor();

    class MenuBaseFlags
    {
    public:
        enum Enum
        {
            IgnoreNextLeftRelease = 1,
            IgnoreNextRightRelease = 2,
            IsMenuMode = 4,
            OpenOnMouseEnter = 8
        };
    };


    bool ElementIsWithin(DependencyObject* pElement);
    bool IsDescendant(DependencyObject* pNode);
    static bool IsDescendant(DependencyObject* pReference, DependencyObject* pNode);
    void KeyboardLeaveMenuMode();
    void OnClickThrough(MouseButtonEventArgs* pArgs);
    static void OnClickThroughThunk(Object* pSender, MouseButtonEventArgs* pArgs) { ((MenuBase*)pSender)->OnClickThrough(pArgs); }
    static void OnIsSelectedChanged(Object* pSender, RoutedPropertyChangedBoolEventArgs* pArgs);
    static void OnLostMouseCapture(Object* pSender, MouseEventArgs* pArgs);
    static void OnMenuItemPreviewClick(Object* pSender, RoutedEventArgs* pArgs);
    static void OnMouseButtonDown(Object* pSender, MouseButtonEventArgs* pArgs) { ((MenuBase*)pSender)->HandleMouseButton(pArgs); }
    static void OnMouseButtonUp(Object* pSender, MouseButtonEventArgs* pArgs) { ((MenuBase*)pSender)->HandleMouseButton(pArgs); }
    static void OnPromotedMouseButton(Object* pSender, MouseButtonEventArgs* pArgs);
    void RestorePreviousFocus();
    static void SetSuspendingPopupAnimation(ItemsControl* pMenu, MenuItem* pIgnore, bool suspend);

    MenuItem* get_CurrentSelection() { return m_pCurrentSelection; }
    void set_CurrentSelection(MenuItem* pSelection);

    bool get_HasCapture();
    bool get_IgnoreNextLeftRelease() { return (m_menuFlags & MenuBaseFlags::IgnoreNextLeftRelease) == MenuBaseFlags::IgnoreNextLeftRelease; }
    void set_IgnoreNextLeftRelease(bool ignoreNextLeftRelease) { m_menuFlags = (ignoreNextLeftRelease ? m_menuFlags | MenuBaseFlags::IgnoreNextLeftRelease : 
                                                                                m_menuFlags & ~MenuBaseFlags::IgnoreNextLeftRelease); }
    bool get_IgnoreNextRightRelease() { return (m_menuFlags & MenuBaseFlags::IgnoreNextRightRelease) == MenuBaseFlags::IgnoreNextRightRelease; }
    void set_IgnoreNextRightRelease(bool ignoreNextRightRelease) { m_menuFlags = (ignoreNextRightRelease ? m_menuFlags | MenuBaseFlags::IgnoreNextRightRelease : 
                                                                                 m_menuFlags & ~MenuBaseFlags::IgnoreNextRightRelease); }
    bool get_IsMenuMode() { return (m_menuFlags & MenuBaseFlags::IsMenuMode) == MenuBaseFlags::IsMenuMode; }
    void set_IsMenuMode(bool isMenuMode);

    bool get_OpenOnMouseEnter() { return (m_menuFlags & MenuBaseFlags::OpenOnMouseEnter) == MenuBaseFlags::OpenOnMouseEnter; }
    void set_OpenOnMouseEnter(bool openOnMouseEnter) { m_menuFlags = (openOnMouseEnter ? m_menuFlags | MenuBaseFlags::OpenOnMouseEnter : 
                                                                                 m_menuFlags & ~MenuBaseFlags::OpenOnMouseEnter); }

    IMPLEMENT_EVENT(InternalMenuModeChanged, EventArgs);
    IMPLEMENT_ROUTED_EVENT(IsSelectedChanged, RoutedPropertyChangedBoolEventArgs, RoutingStrategy::Bubble);


    unsigned int m_menuFlags;
    TRefCountedPtr<MenuItem> m_pCurrentSelection;
};


}; // namespace AVUI

