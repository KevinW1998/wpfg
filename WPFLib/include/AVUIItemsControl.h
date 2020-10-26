#pragma once

#include <AVUIControl.h>

namespace AVUI {

class Panel;
class ScrollViewer;
class DataTemplate;
class DataTemplateSelector;
class StyleSelector;
class ItemContainerGenerator;
class ItemCollection;
class CollectionChangedEventArgs;
class Keyboard;

class ItemsPanelTemplate;

class ItemNavigateArgs
{
public:

    ItemNavigateArgs(Keyboard* pDeviceIn, ModifierKeys::Enum modifierKeysIn) : pInputDevice(pDeviceIn), Modifiers(modifierKeysIn)
    {

    }

    Keyboard* pInputDevice;
    ModifierKeys::Enum Modifiers;
};

class ItemsControl : public Control
{
    friend class Panel;
    friend class ComboBox;
    friend class HeaderedItemsControl;
    friend class MenuItem;
public:

    ItemsControl() : m_pFocusedItem(NULL), m_isScrollHostValid(false) { }
    virtual ~ItemsControl() { }

    DECLARE_ELEMENT(ItemsControl, Control);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ItemsSource, Object, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, ItemsControl::OnItemsSourceChanged));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ItemTemplate, DataTemplate, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ItemTemplateSelector, DataTemplateSelector, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ItemContainerStyle, Style, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ItemContainerStyleSelector, StyleSelector, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(ItemsPanel, ItemsPanelTemplate, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(HasItems, bool, false, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsGrouping, bool, false, PropertyFlag::AffectsMeasure);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Items, ItemCollection, true);

    // GroupStyleSelector

    ItemContainerGenerator* get_ItemContainerGenerator()
    {
        if(m_pItemContainerGenerator == NULL)
        {
            CreateItemCollectionAndGenerator();
        }
        return m_pItemContainerGenerator;
    }

    ItemCollection* get_View() { return get_Items(); }

    void ClearContainerForItem(DependencyObject* pContainer, Object* pItem); 

 

    static ItemsControl* GetItemsOwner(DependencyObject* pElement);
    void PrepareItemContainer(DependencyObject* pContainer, Object* pItem);
    TRefCountedPtr<DependencyObject> GetContainerForItem(Object* pItem);

    ItemCollection* get_Items();
    static ItemsControl* ItemsControlFromItemContainer(DependencyObject* pContainer);
    bool IsHostForItemContainer(DependencyObject* pContainer);
    Object* GetItemOrContainerFromContainer(DependencyObject* pContainer);

    virtual void FocusItem(Object* pItem, ItemNavigateArgs* pItemNavigateArgs);

protected:
    virtual void ClearContainerForItemOverride(DependencyObject* pElement, Object* pItem) { }
    virtual bool ShouldApplyItemContainerStyle(DependencyObject* pContainer, Object* pItem) { return true; }

    virtual void OnItemsSourceChanged(Object* pOldValue, Object* pNewValue) { }
    virtual void PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem);
    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();
    virtual void OnItemsChanged(CollectionChangedEventArgs* pArgs) { };

    void DoAutoScroll();
    void DoAutoScroll(Object* pStartingItem);

    void NavigateByPage(FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs)
    {
        NavigateByPage(get_FocusedItem(), direction, pItemNavigateArgs);
    }

    void NavigateByPage(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs);
    void NavigateToEnd(ItemNavigateArgs* pItemNavigateArgs);
    void NavigateToStart(ItemNavigateArgs* pItemNavigateArgs);
    void NavigateByLine(FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs);
    void NavigateByLine(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs);
    void NavigateByPageInternal(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs);

    void NavigateToItem(Object* pItem, ItemNavigateArgs* pItemNavigateArgs)
    {
        NavigateToItem(pItem, -1, pItemNavigateArgs, false);
    }
 
    void NavigateToItem(Object* pItem, int itemIndex, ItemNavigateArgs* pItemNavigateArgs)
    {
        NavigateToItem(pItem, itemIndex, pItemNavigateArgs, false);
    }

    void NavigateToItem(Object* pItem, ItemNavigateArgs* pItemNavigateArgs, bool alwaysAtTopOfViewport)
    {
        NavigateToItem(pItem, -1, pItemNavigateArgs, alwaysAtTopOfViewport);
    }


 

 

    Object* get_FocusedItem() { return m_pFocusedItem; }
    ScrollViewer* get_ScrollHost();


    // Deviation here -- This is to prevent directional keyboard navigation from getting 'stuck' on combobox
    bool get_IsItemsHostLogicalHorizontal();
    bool get_IsItemsHostLogicalVertical();

private:

    bool get_IsLogicalVertical();
    bool get_IsLogicalHorizontal();

    Object* GetEncapsulatingItem(UIElement* pElement);
    Object* FindFocusable(int startIndex, int direction, int* pFoundIndex);
    void NavigateToItem(Object* pItem, int elementIndex, ItemNavigateArgs* pItemNavigateArgs, bool alwaysAtTopOfViewport);
    void NavigateByLineInternal(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs);

    void NavigateToFirstItemOnCurrentPage(Object* pStartingItem, FocusNavigationDirection::Enum direction, ItemNavigateArgs* pItemNavigateArgs);
    Object* GetFirstItemOnCurrentPage(Object* pStartingItem, FocusNavigationDirection::Enum direction, int* pFoundIndex);

 

    bool IsOnCurrentPage(Object* pItem, FocusNavigationDirection::Enum axis);
    bool IsOnCurrentPage(UIElement* pElement, FocusNavigationDirection::Enum axis);
    bool IsOnCurrentPage(UIElement* pElement, FocusNavigationDirection::Enum axis, bool fullyVisible);
    bool MakeVisible(int index)
    {
        return MakeVisible(index, false, false);
    }
    bool MakeVisible(int index, bool alwaysAtTopOfViewport, bool alignMinorAxisToo);

 

 


    void OnItemCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs);
    void PrepareItemsControl(Object* pItem, DataTemplate* pItemTemplate, DataTemplateSelector* pItemTemplateSelector, Style* pItemContainerStyle, StyleSelector* pItemContainerStyleSelector);
    void ApplyItemContainerStyle(DependencyObject* pContainer, Object* pItem);
    static void OnItemsSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    Panel* get_ItemsHost() { return m_pItemsHost; }
    void set_ItemsHost(Panel* pPanel) { m_pItemsHost = pPanel; }

    void CreateItemCollectionAndGenerator();

    Object* m_pFocusedItem;
    bool m_isScrollHostValid : 1;
    TRefCountedPtr<Panel> m_pItemsHost; 
    TRefCountedPtr<ScrollViewer> m_pScrollHost; 
    TRefCountedPtr<ItemContainerGenerator> m_pItemContainerGenerator;
    TRefCountedPtr<ItemCollection> m_pItems; 
};


}; // namespace AVUI
