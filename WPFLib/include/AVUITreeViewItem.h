#pragma once

#include <AVUIHeaderedItemsControl.h>
#include <AVUISelector.h>

namespace AVUI {

class TreeView;
class RequestBringIntoViewEventArgs;

class TreeViewItem : public HeaderedItemsControl
{
    friend class TreeView;
public:

    TreeViewItem() : m_containsSelection(false) { }

    DECLARE_ELEMENT(TreeViewItem, HeaderedItemsControl);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsExpanded, bool, false, DependencyPropertyMetadata(PropertyFlag::None, TreeViewItem::OnIsExpandedChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsSelected, bool, false, DependencyPropertyMetadata(PropertyFlag::None, TreeViewItem::OnIsSelectedChanged));
    static const AVUI::DependencyProperty& get_IsSelectionActiveProperty()  { return Selector::get_IsSelectionActiveProperty(); }

    IMPLEMENT_ROUTED_EVENT(Expanded, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Collapsed, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Selected, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Unselected, RoutedEventArgs, RoutingStrategy::Bubble);

    bool get_IsSelectionActive() { return UnboxValue<bool>(GetValue(get_IsSelectionActiveProperty())); };

protected:

    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();
    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual void OnCollapsed(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnExpanded(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnGotFocus(RoutedEventArgs* pArgs) { Select(true); HeaderedItemsControl::OnGotFocus(pArgs); }
    virtual void OnItemsChanged(CollectionChangedEventArgs* pArgs);
    virtual void OnKeyDown(KeyEventArgs* pArgs);
    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnSelected(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnUnselected(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    virtual void OnVisualParentChanged(DependencyObject* pOldParent);

private:

    static void StaticConstructor();

    bool AllowHandleKeyEvent(FocusNavigationDirection::Enum direction);
    static float CalculateDelta(bool up, UIElement* pItem, ScrollViewer* pScroller, float startTop, float startBottom);
    static float CalculateDelta(bool up, UIElement* pItem, ScrollViewer* pScroller, float startTop, float startBottom, float* pfCloseEdge);
    bool FocusDown();
    static bool FocusIntoItem(TreeViewItem* pItem);
    void GetTopAndBottom(Visual* pParent, float* pfTop, float* pfBottom);
    static void GetTopAndBottom(UIElement* pItem, Visual* pParent, float* pfTop, float* pfBottom);
    void HandleBringIntoView(RequestBringIntoViewEventArgs* pArgs);
    bool HandleDownKey();
    bool HandleScrollByPage(bool up, ScrollViewer* pScroller, float viewportHeight, float startTop, float startBottom, float* pfCurrentDelta);
    bool HandleUpKey();
    bool LogicalLeft(Key::Enum key);
    static void OnIsExpandedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& args);
    static void OnIsSelectedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& args);
    static void OnMouseButtonDown(Object* pSender, MouseButtonEventArgs* pArgs);
    static void OnRequestBringIntoView(Object* pSender, RequestBringIntoViewEventArgs* pArgs);
    void Select(bool selected);
    void UpdateContainsSelection(bool selected);

 
 

 

 

 

    bool get_CanExpand() { return get_HasItems(); }

    bool get_CanExpandOnInput() { return get_CanExpand() && get_IsEnabled(); }

    bool get_ContainsSelection() { return m_containsSelection; }
    void set_ContainsSelection(bool containsSelection) { m_containsSelection = containsSelection; }

    UIElement* get_HeaderElement() { return object_cast<UIElement>(FindTemplateChild(L"PART_Header")); }

    static bool get_IsControlKeyDown();

    ItemsControl* get_ParentItemsControl() { return ItemsControl::ItemsControlFromItemContainer(this); }

    TreeView* get_ParentTreeView();

    TreeViewItem* get_ParentTreeViewItem() { return object_cast<TreeViewItem>(get_ParentItemsControl()); }

    bool m_containsSelection;
};


}; // namespace AVUI


