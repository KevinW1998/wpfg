#include <AVUICommon.h>
#include <AVUIListBoxItem.h>
#include <AVUIListBox.h>

REGISTER_ELEMENT_BEGIN(AVUI::ListBoxItem)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(IsSelected)
    REGISTER_ROUTED_EVENT(Selected)
    REGISTER_ROUTED_EVENT(Unselected)

REGISTER_ELEMENT_END()

namespace AVUI {


void ListBoxItem::OnIsSelectedChanged(Object* pSender, const DependencyPropertyChangedEventArgs& e)
{
    ListBoxItem* pItem = (ListBoxItem*) pSender;
    bool isSelected = UnboxValue<bool>(e.pNewValue);

    if (isSelected)
    {
        RoutedEventArgs ea(&get_SelectedEvent(), pItem);
        pItem->OnSelected(&ea);
    }
    else
    {
        RoutedEventArgs ea(&get_UnselectedEvent(), pItem);
        pItem->OnUnselected(&ea);
    }
}

void ListBoxItem::OnSelected(RoutedEventArgs* pArgs)
{
    HandleIsSelectedChanged(true, pArgs);
}

void ListBoxItem::OnUnselected(RoutedEventArgs* pArgs)
{
    HandleIsSelectedChanged(false, pArgs);
}

void ListBoxItem::HandleIsSelectedChanged(bool newValue, RoutedEventArgs* pArgs)
{
    RaiseEvent(pArgs);
}

void ListBoxItem::OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs)
{
    if(!pArgs->get_IsHandled())
    {
        pArgs->set_IsHandled(true);
        HandleMouseButtonDown(MouseButton::Left);
    }

    ContentControl::OnMouseLeftButtonDown(pArgs);
}

void ListBoxItem::OnMouseRightButtonDown(MouseButtonEventArgs* pArgs)
{
    if(!pArgs->get_IsHandled())
    {
        pArgs->set_IsHandled(true);
        HandleMouseButtonDown(MouseButton::Right);
    }

    ContentControl::OnMouseRightButtonDown(pArgs);
}

 


void ListBoxItem::HandleMouseButtonDown(MouseButton::Enum mouseButton)
{
    if (Selector::UiGetIsSelectable(this) && Focus())
    {
        ListBox* pParentListBox = get_ParentListBox();
        if (pParentListBox != NULL)
        {
            pParentListBox->NotifyListItemClicked(this, mouseButton);
        }
    }
}

ListBox* ListBoxItem::get_ParentListBox()
{
    return object_cast<ListBox>(get_ParentSelector());

}

Selector* ListBoxItem::get_ParentSelector()
{
    return object_cast<Selector>(ItemsControl::ItemsControlFromItemContainer(this));
}

}; // namespace AVUI
