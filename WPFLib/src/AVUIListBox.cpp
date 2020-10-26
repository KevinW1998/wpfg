#include <AVUICommon.h>
#include <AVUIControlTemplate.h>
#include <AVUIListBox.h>
#include <AVUIListBoxItem.h>
#include <AVUISeparator.h>
#include <AVUIItemContainerGenerator.h>
#include <AVUIItemCollection.h>
#include <AVUIInputManager.h>
#include <AVUIDispatcherTimer.h>

REGISTER_ELEMENT_BEGIN(AVUI::ListBox)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(SelectionMode)
REGISTER_ELEMENT_END()

namespace AVUI {



bool ListBox::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem->IsType(GET_TYPE(ListBoxItem));
}

TRefCountedPtr<DependencyObject> ListBox::GetContainerForItemOverride()
{
    return (DependencyObject*) object_allocate<ListBoxItem>();
} 


void ListBox::PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem)
{
    Selector::PrepareContainerForItemOverride(pElement, pItem);

    if (pItem->IsType(GET_TYPE(Separator)))
    {
        Separator::PrepareContainer(object_cast<Control>(pElement));
    }
}


void ListBox::NotifyListItemClicked(ListBoxItem* pItem, MouseButton::Enum mouseButton)
{
    if (mouseButton == MouseButton::Left && !get_IsMouseCaptured())
    {
        CaptureMouse();
        Selector::SetInitialMousePosition();
    }
    switch (get_SelectionMode())
    {
        case SelectionMode::Single:
            if (pItem->get_IsSelected())
            {
                if ((get_InputManager()->get_KeyboardModifiers() & ModifierKeys::Control) == ModifierKeys::Control)
                {
                    pItem->set_IsSelected(false);
                }
            } 
            else
            {
                pItem->set_IsSelected(true);
            }
            UpdateAnchorAndActionItem(pItem);
            break;

        case SelectionMode::Multiple:
            MakeToggleSelection(pItem);
            break;

        case SelectionMode::Extended:
            if ((get_InputManager()->get_KeyboardModifiers() & (ModifierKeys::Shift | ModifierKeys::Control)) != (ModifierKeys::Shift | ModifierKeys::Control))
            {
                if ((get_InputManager()->get_KeyboardModifiers() & ModifierKeys::Control) == ModifierKeys::Control)
                {
                    MakeToggleSelection(pItem);
                }
                else if ((get_InputManager()->get_KeyboardModifiers() & ModifierKeys::Shift) == ModifierKeys::Shift)
                {
                    MakeAnchorSelection(pItem, true);
                }
                else
                {
                    MakeSingleSelection(pItem);
                }
                break;
            }
            else
            {
                MakeAnchorSelection(pItem, false);
            }
    }
}

void ListBox::MakeToggleSelection(ListBoxItem* pItem)
{
    pItem->set_IsSelected(!pItem->get_IsSelected());

    UpdateAnchorAndActionItem(pItem);
}

 


void ListBox::OnMouseUp(MouseButtonEventArgs* pArgs)
{
    if(pArgs->get_ChangedButton() == MouseButton::Left)
    {
        if(!pArgs->get_IsHandled())
        {
            pArgs->set_IsHandled(true);

            ReleaseMouseCapture();
            Selector::ResetLastMousePosition();

        }
    }
    Selector::OnMouseUp(pArgs);
}


void ListBox::OnSelectionModeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ListBox* pListBox = (ListBox*) pDO;
    pListBox->ValidateSelectionMode(pListBox->get_SelectionMode());
}

void ListBox::ValidateSelectionMode(SelectionMode::Enum mode)
{
    set_CanSelectMultiple(mode != SelectionMode::Single);
}

void ListBox::UpdateAnchorAndActionItem(ListBoxItem* pListContainer)
{
    Object* pItem = get_ItemContainerGenerator()->ItemFromContainer(pListContainer);
    if (pItem == DependencyProperty::get_UnsetValue())
    {
        set_AnchorItem(NULL);
        set_LastActionItem(NULL);
    }
    else
    {
        set_AnchorItem(pItem);
        set_LastActionItem(pListContainer);
    }
    // KeyboardNavigation.SetTabOnceActiveElement(this, listItem);
}

 
void ListBox::MakeAnchorSelection(ListBoxItem* pActionItem, bool clearCurrent)
{
   if (get_AnchorItem() == NULL)
    {
        if (m_selectedItems.get_Count() > 0)
        {
            set_AnchorItem(m_selectedItems[m_selectedItems.get_Count() - 1]);
        }
        else
        {
            set_AnchorItem(get_Items()->GetItemAt(0));;
        }
        if (get_AnchorItem() == NULL)
        {
            return;
        }
    }
    int num = ElementIndex(pActionItem);
    int index = get_Items()->IndexOf(get_AnchorItem());
    if (num > index)
    {
        int num3 = num;
        num = index;
        index = num3;
    }
    bool flag = false;
    if (!get_SelectionChange()->get_IsActive())
    {
        flag = true;
        get_SelectionChange()->Begin();
    }

    if (clearCurrent)
    {
        for (int i = 0; i < m_selectedItems.get_Count(); i++)
        {
            Object* pItem = m_selectedItems[i];
            int num5 = get_Items()->IndexOf(pItem);
            if ((num5 < num) || (index < num5))
            {
                get_SelectionChange()->Unselect(pItem);
            }
        }
    }


    int listIdx = 0;
    for (int j = 0; j <= index; j++)
    {
        if (j >= num)
        {
            get_SelectionChange()->Select(get_Items()->GetItemAt(listIdx), true);
        }
        listIdx++;
    }
    if (flag)
    {
        get_SelectionChange()->End();
    }
    set_LastActionItem(pActionItem);
}

 
void ListBox::MakeSingleSelection(ListBoxItem* pListItem)
{
    if (ItemsControl::ItemsControlFromItemContainer(pListItem) == this)
    {
        Object* pItem = get_ItemContainerGenerator()->ItemFromContainer(pListItem);
        get_SelectionChange()->SelectJustThisItem(pItem, true);
        pListItem->Focus();
        UpdateAnchorAndActionItem(pListItem);
    }
}

int ListBox::ElementIndex(ListBoxItem* pListItem)
{
    return get_ItemContainerGenerator()->IndexFromContainer(pListItem);
}

 
void ListBox::OnIsMouseCapturedChanged()
{
    if (get_IsMouseCaptured())
    {
        if (m_pAutoScrollTimer == NULL)
        {
            m_pAutoScrollTimer = object_allocate<DispatcherTimer>(TimeSpan::FromMilliseconds(300), EventHandler::Create(this, &ListBox::OnAutoScrollTimeout));
            m_pAutoScrollTimer->Start();
        }
    }
    else if (m_pAutoScrollTimer != NULL)
    {
        m_pAutoScrollTimer->Stop();
        m_pAutoScrollTimer = NULL;
    }
    Control::OnIsMouseCapturedChanged();
}

void ListBox::OnAutoScrollTimeout(Object* pSender, EventArgs* pArgs)
{
    /*
    if (Mouse.LeftButton == MouseButtonState.Pressed)
    {
        DoAutoScroll();
    }
    */
}

 

 
 
 

 




}; // namespace AVUI
