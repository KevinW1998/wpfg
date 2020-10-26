#include <AVUICommon.h>
#include <AVUIListView.h>
#include <AVUIListViewItem.h>
#include <AVUIViewBase.h>
#include <AVUIItemContainerGenerator.h>

REGISTER_ELEMENT_BEGIN(AVUI::ListView)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(View)
REGISTER_ELEMENT_END()

namespace AVUI {

ListView::ListView() : m_pPreviousView(NULL)
{
    set_SelectionMode(SelectionMode::Extended);
}


bool ListView::IsItemItsOwnContainerOverride(Object* pItem)
{
    return pItem->IsType(GET_TYPE(ListViewItem));
}

TRefCountedPtr<DependencyObject> ListView::GetContainerForItemOverride()
{
    return (DependencyObject*)object_allocate<ListViewItem>();
}

void ListView::PrepareContainerForItemOverride(DependencyObject* pElement, Object* pItem)
{
    ListBox::PrepareContainerForItemOverride(pElement, pItem);
    ListViewItem* pListViewItem = object_cast<ListViewItem>(pElement);
    if (pListViewItem != NULL)
    {
        ViewBase* pView = get_View();
        if (pView != NULL)
        {
            // pListViewItem->SetDefaultStyleKey(view.ItemContainerDefaultStyleKey);
            pView->PrepareItem(pListViewItem);
        }
        else
        {
            // pListViewItem->ClearDefaultStyleKey();
        }
    }
}


void ListView::OnViewChanged(Object* pObj, const DependencyPropertyChangedEventArgs& e)
{
    ListView* pElement = (ListView*) pObj;
    ViewBase* pOldValue = (ViewBase*) e.pOldValue;
    ViewBase* pNewValue = (ViewBase*) e.pNewValue;
    if (pNewValue != NULL)
    {
        if (pNewValue->get_IsUsed())
        {
            LibraryCriticalError();
        }
        pNewValue->set_IsUsed(true);
    }
    pElement->m_pPreviousView = pOldValue;
    pElement->ApplyNewView();
    pElement->m_pPreviousView = pNewValue;

    if (pOldValue != NULL)
    {
        pOldValue->set_IsUsed(false);
    }
}

 
void ListView::ApplyNewView()
{
    ViewBase* pView = get_View();
    if (pView != NULL)
    {
        //base.DefaultStyleKey = view.DefaultStyleKey;
    }
    else
    {
        //base.ClearValue(FrameworkElement.DefaultStyleKeyProperty);
    }

//    if (base.IsLoaded)
    {
        get_ItemContainerGenerator()->Refresh();
    }
}

 

 

 




}; // namespace AVUI

