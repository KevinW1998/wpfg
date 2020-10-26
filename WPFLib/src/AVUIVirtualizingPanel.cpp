#include <AVUICommon.h>
#include <AVUIVirtualizingPanel.h>
#include <AVUIItemContainerGenerator.h>

REGISTER_ELEMENT_BEGIN(AVUI::VirtualizingPanel)

REGISTER_ELEMENT_END()


namespace AVUI
{


void VirtualizingPanel::AddChild(UIElement* pChild)
{
    get_Children()->Add(pChild);
}

void VirtualizingPanel::InsertChild(int index, UIElement* pChild)
{
    get_Children()->Insert(index, pChild);
}

void VirtualizingPanel::RemoveChildRange(int index, int range)
{
    get_Children()->RemoveRange(index, range);

}


void VirtualizingPanel::OnItemsChangedInternal(Object* pSender, ItemsChangedEventArgs* pArgs)
{
    switch (pArgs->get_Action())
    {
    case NotifyCollectionChangedAction::Add:
    case NotifyCollectionChangedAction::Remove:
    case NotifyCollectionChangedAction::Replace:
    case NotifyCollectionChangedAction::Move:
        break;

    default:
        Panel::OnItemsChangedInternal(pSender, pArgs);
        break;
    }

    OnItemsChanged(pSender, pArgs);
}

 
void VirtualizingPanel::RemoveInternalChildRange(UIElementCollection* pChildren, int index, int range)
{
    pChildren->RemoveRange(index, range);
}

 




};

