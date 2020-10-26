#pragma once

#include <AVUISelector.h>

namespace AVUI {

class DataTemplate;
class ComboBoxItem;

class ComboBox : public Selector
{
    friend class ComboBoxItem;
public:
    ComboBox() : m_updatingText(false), m_updatingSelectedItem(false), m_isMouseOverItemsHost(false), m_isContextMenuOpen(false), m_hasMouseEnteredItemsHost(false) { };

    ~ComboBox() { };

    DECLARE_ELEMENT(ComboBox, Selector);

    IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(SelectionBoxItem, Object, NULL, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_DEPENDENCY_PROPERTY(SelectionBoxItemTemplate, DataTemplate, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Text, String, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsDropDownOpen, bool, false, DependencyPropertyMetadata(PropertyFlag::None, ComboBox::OnIsDropDownOpenChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsEditable, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_VALUE_DEPENDENCY_PROPERTY(IsSelectionBoxHighlighted, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsReadOnly, bool, true, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(MaxDropDownHeight, float, 200.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(StaysOpenOnEdit, bool, false, PropertyFlag::None);
    virtual void OnApplyTemplate();

    virtual void FocusItem(Object* pItem, ItemNavigateArgs* pItemNavigateArgs);

protected:

    virtual void PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem);
    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();

    virtual void OnKeyDown(KeyEventArgs* pArgs) { KeyDownHandler(pArgs); }
    virtual void OnSelectionChanged(SelectionChangedEventArgs* pArgs);
    virtual void OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs);


private:

    static void StaticConstructor();

    void NotifyComboBoxItemMouseDown(ComboBoxItem* pComboBoxItem) { }
    void NotifyComboBoxItemMouseUp(ComboBoxItem* pComboBoxItem);

    static void OnIsDropDownOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnLostMouseCapture(Object* pSender, MouseEventArgs* pArgs); 
    static void OnMouseButtonDown(Object* pSender, MouseButtonEventArgs* pArgs);
    static void OnMouseMove(Object* pSender, MouseEventArgs* pArgs);

    // Deviation -- do this using an explicit function
    void AsyncNavigateToSelectedItem(Object* pSender, EventArgs* pArgs);

    void NotifyComboBoxItemEnter(ComboBoxItem* pItem);

    void Close();
    void SelectedItemUpdated();
    void Update();
    void UpdateSelectionBoxItem();
    void KeyDownHandler(KeyEventArgs* pArgs);
    bool get_IsItemsHostVisible();
    Object* get_HighlightedItem();
    ComboBoxItem* get_HighlightedElement();
    void set_HighlightedElement(ComboBoxItem* pHighlightedElement);
    void KeyboardToggleDropDown(bool commitSelection);
    void KeyboardToggleDropDown(bool openDropDown, bool commitSelection);
    void KeyboardCloseDropDown(bool commitSelection);

    void SelectPrev();
    void SelectNext();
    void SelectLast();
    void SelectFirst();
    void SelectItemHelper(int startIndex, int increment, int stopIndex);
    bool IsSelectableHelper(Object* pO);

    bool get_IsMouseOverItemsHost() { return m_isMouseOverItemsHost; }
    void set_IsMouseOverItemsHost(bool isMouseOverItemsHost) { m_isMouseOverItemsHost = isMouseOverItemsHost; }

    bool get_IsContextMenuOpen() { return m_isContextMenuOpen; }

    bool get_HasMouseEnteredItemsHost() { return m_hasMouseEnteredItemsHost; }
    void set_HasMouseEnteredItemsHost(bool hasMouseEnteredItemsHost) { m_hasMouseEnteredItemsHost = hasMouseEnteredItemsHost; }

    bool get_HasCapture();

    TWeakPtr<ComboBoxItem> m_pHighlightedElement;
    bool m_updatingText : 1;
    bool m_updatingSelectedItem : 1;
    bool m_isMouseOverItemsHost : 1;
    bool m_isContextMenuOpen : 1;
    bool m_hasMouseEnteredItemsHost : 1;
};


}; // namespace AVUI

