#pragma once

#include <AVUIItemsControl.h>
#include <AVUIBindingExpression.h>

namespace AVUI {

class TreeViewItem;
class RoutedPropertyChangedObjectEventArgs;

class TreeView : public ItemsControl
{
    friend class TreeViewItem;
public:

    DECLARE_ELEMENT(TreeView, ItemsControl);

    TreeView() : m_isSelectionChangeActive(false) { }

    IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(SelectedItem, Object, NULL, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(SelectedValue, Object, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(SelectedValuePath, String, NULL, DependencyPropertyMetadata(PropertyFlag::None, &TreeView::OnSelectedValuePathChanged));

    IMPLEMENT_ROUTED_EVENT(SelectedItemChanged, RoutedPropertyChangedObjectEventArgs, RoutingStrategy::Bubble);

protected:

    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();
    virtual void OnIsKeyboardFocusWithinChanged(DependencyPropertyChangedEventArgs* pArgs);
    virtual void OnItemsChanged(CollectionChangedEventArgs* pArgs);
    virtual void OnKeyDown(KeyEventArgs* pArgs);
    virtual void OnSelectedItemChanged(RoutedPropertyChangedObjectEventArgs* pArgs);
    virtual bool get_HandlesScrolling() { return true; }

private:

    void ChangeSelection(Object* pData, TreeViewItem* pContainer, bool selected);
    static DependencyObject* FindParent(DependencyObject* pDO);
    bool FocusFirstItem();
    bool FocusLastItem();
    bool GetFirstItem(Object** ppItem, TreeViewItem** ppContainer);
    void HandleMouseButtonDown();
    bool HandleScrollByPage(bool up);
    bool HandleScrollKeys(Key::Enum key);
    void HandleSelectionAndCollapsed(TreeViewItem* pCollapsed);
    void OnFocusEnterMainFocusScope(Object* pSender, EventArgs* pArgs);
    static void OnSelectedValuePathChanged(Object* pDO, const DependencyPropertyChangedEventArgs& args);
    TRefCountedPtr<BindingExpression> PrepareSelectedValuePathBindingExpression(Object* pItem);
    void SelectFirstItem();
    void SetSelectedItem(Object* pData);
    void SetSelectedValue(Object* pData);
    void UpdateSelectedValue(Object* pSelectedItem);

    bool get_IsSelectedContainerHookedUp();    

    static bool get_IsControlKeyDown();
    static bool get_IsShiftKeyDown();
    bool get_IsSelectionChangeActive() { return m_isSelectionChangeActive; }
    void set_IsSelectionChangeActive(bool isActive) { m_isSelectionChangeActive = isActive; }
    TreeViewItem* get_SelectedContainer() { return m_pSelectedContainer; }

    bool m_isSelectionChangeActive;
    TRefCountedPtr<TreeViewItem> m_pSelectedContainer;

    static BindingExpressionUncommonField SelectedValuePathBindingExpression;  
};


}; // namespace AVUI

