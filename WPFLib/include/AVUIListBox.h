#pragma once

#include <AVUISelector.h>

namespace AVUI {

class Panel;
class DispatcherTimer;
class GridViewHeaderRowPresenter;
class ListBoxItem;

class ListBox : public Selector
{
    friend class GridViewHeaderRowPresenter;
    friend class ListBoxItem;    
public:
    ListBox() { };
    ~ListBox() { };

    DECLARE_ELEMENT(ListBox, Selector);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(SelectionMode, SelectionMode, SelectionMode::Single, DependencyPropertyMetadata(PropertyFlag::None, OnSelectionModeChanged));

protected:

    virtual void PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem);
    virtual bool IsItemItsOwnContainerOverride(Object* pItem);
    virtual TRefCountedPtr<DependencyObject> GetContainerForItemOverride();

    virtual void OnMouseUp(MouseButtonEventArgs* pArgs);
    virtual void OnIsMouseCapturedChanged();

private:

    Object* get_AnchorItem() { return m_pAnchorItem; }
    void set_AnchorItem(Object* pItem) { m_pAnchorItem = pItem; }

    ListBoxItem* get_LastActionItem() { return m_pLastActionItem; }
    void set_LastActionItem(ListBoxItem* pItem) { m_pLastActionItem = pItem; }

    void MakeAnchorSelection(ListBoxItem* pActionItem, bool clearCurrent);
    void MakeToggleSelection(ListBoxItem* pItem);
    void UpdateAnchorAndActionItem(ListBoxItem* pListContainer);
    void ValidateSelectionMode(SelectionMode::Enum mode);
    void NotifyListItemClicked(ListBoxItem* pitem, MouseButton::Enum mouseButton);
    void MakeSingleSelection(ListBoxItem* pListItem);
    int ElementIndex(ListBoxItem* pListItem);
    void OnAutoScrollTimeout(Object* pSender, EventArgs* pArgs);
    static void OnSelectionModeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    ObjectWeakPtr m_pAnchorItem;
    TWeakPtr<ListBoxItem> m_pLastActionItem;
    TRefCountedPtr<DispatcherTimer> m_pAutoScrollTimer;


};


}; // namespace AVUI

