#pragma once

#include <AVUIItemsControl.h>


namespace AVUI
{


class SelectionChangedEventArgs : public RoutedEventArgs
{
public:
    SelectionChangedEventArgs(IEnumerable* pRemovedItems, IEnumerable* pAddedItems);

    DECLARE_ELEMENT(SelectionChangedEventArgs, RoutedEventArgs);
};

DEFINE_EVENTHANDLER(SelectionChanged);


class Selector : public ItemsControl
{
    friend class ComboBox;
public:

    Selector();
    ~Selector();

    DECLARE_ELEMENT(Selector, ItemsControl);

    IMPLEMENT_ROUTED_EVENT(SelectionChanged, SelectionChangedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Selected, RoutedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(Unselected, RoutedEventArgs, RoutingStrategy::Bubble);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsSelected, bool, false, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsSelectionActive, bool, false, PropertyFlag::Inherits);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsSynchronizedWithCurrentItem, Nullable<bool>, Nullable<bool>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(SelectedIndex, int, -1, DependencyPropertyMetadata(PropertyFlag::None, Selector::OnSelectedIndexChanged, Selector::CoerceSelectedIndex));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(SelectedItem, Object, NULL, PropertyFlag::None);

    static bool GetIsSelected(DependencyObject* pElement);
    static bool ItemGetIsSelectable(Object* pItem);
    static bool UiGetIsSelectable(DependencyObject* pDO);

protected:

    bool get_CanSelectMultiple() { return m_canSelectMultiple; }
    void set_CanSelectMultiple(bool canSelectMultiple);
    void SetInitialMousePosition();
    void ResetLastMousePosition();
    virtual void OnSelectionChanged(SelectionChangedEventArgs* pArgs);
    virtual void OnItemsChanged(CollectionChangedEventArgs* pArgs);

    bool DidMouseMove();

private:

    bool ItemGetIsSelected(Object* pItem);
    static bool ContainerGetIsSelected(DependencyObject* pContainer, Object* pItem);
    bool IndexGetIsSelected(int index, Object* pItem);
    void SetSelectedToCurrent();

    // Private -- 
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(SelectedItemsImpl, Object, NULL, PropertyFlag::None);

    static ObjectPtr CoerceSelectedIndex(DependencyObject* pDO, Object* pValue);
    static void OnSelectedIndexChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    void SetCurrentToSelected();
    void UpdateSelectedItems();
    void UpdatePublicSelectionProperties();
    void ItemSetIsSelected(Object* pItem, bool value);
    void InvokeSelectionChanged(IEnumerable* pUnselectedItems, IEnumerable* pSelectedItems);
    void NotifyIsSelectedChanged(UIElement* pContainer, bool selected, RoutedEventArgs* pArgs);
    void SetSelectedHelper(Object* pItem, UIElement* pContainer, bool selected);

    void OnGeneratorStatusChanged(Object* pSender, EventArgs* pArgs);

    Object* get_InternalSelectedItem()
    {
        if(m_selectedItems.get_Count() > 0)
        {
            return m_selectedItems[0];
        }
        return NULL;
    }

    int get_InternalSelectedIndex();

    class SelectionChanger : public Object
    {
    public:

        SelectionChanger(Selector* pSelector)
        {
            m_pOwner = pSelector;
            m_isActive = false;
        }

        void Begin()
        {
            m_isActive = true;
            m_toSelect.Clear();
            m_toUnselect.Clear();
        }

        void Cancel()
        {
            Cleanup();
        }

        void Cleanup()
        {
            m_isActive = false;
            m_toSelect.Clear();
            m_toUnselect.Clear();
        }

        void CleanupDeferSelection()
        {
            m_toDeferSelect.Clear();
        }

        void CreateDeltaSelectionChange(ItemStructList<ObjectPtr> *pUnselectedItems, ItemStructList<ObjectPtr> *pSelectedItems);
        void ApplyCanSelectMultiple();
        void End();
        bool Select(Object* pObject, bool assumeInItemsCollection);
        void SelectJustThisItem(Object* pItem, bool assumeInItemsCollection);
        bool Unselect(Object* pItem);
        void Validate()
        {
            Begin();
            End();
        }

        bool get_IsActive() { return m_isActive; }

    private:

        Selector* m_pOwner;
        bool m_isActive;
        ItemStructList<ObjectPtr> m_toSelect;
        ItemStructList<ObjectPtr> m_toUnselect;
        ItemStructList<ObjectPtr> m_toDeferSelect;
    };

    static void OnSelected(Object* pSender, RoutedEventArgs* pArgs);
    static void OnUnselected(Object* pSender, RoutedEventArgs* pArgs);
    static void StaticConstructor();

    bool m_canSelectMultiple : 1;
    bool m_skipCoerceSelectedItemCheck : 1;
    bool m_isSelectingSingleItem : 1; // 0x10 in cache bits
    bool m_isSettingCurrentToSelected : 1; // 0x1 in cache bits
    bool m_isSynchronizedWithCurrentItemPrivate : 1; // 0x4 in cache bits
    TRefCountedPtr<SelectionChanger> m_pSelectionInstance;
    Point m_lastMousePosition;

protected:

    SelectionChanger* get_SelectionChange();


    ItemStructList<ObjectPtr> m_selectedItems;

};


};
