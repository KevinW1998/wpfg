#include <AVUICommon.h>
#include <AVUIGridView.h>
#include <AVUIGridViewColumnCollection.h>
#include <AVUIListViewItem.h>

REGISTER_ELEMENT_BEGIN(AVUI::GridView)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(ColumnCollection)
    REGISTER_PROPERTY(ColumnHeaderContainerStyle)
    REGISTER_PROPERTY(ColumnHeaderTemplate)
    REGISTER_PROPERTY(ColumnHeaderTemplateSelector)
    REGISTER_PROPERTY(AllowsColumnReorder)
    REGISTER_PROPERTY(Columns)

REGISTER_ELEMENT_END()


namespace AVUI
{



GridViewColumnCollection* GridView::get_Columns()
{
    if (m_pColumns == NULL)
    {
        m_pColumns = object_allocate<GridViewColumnCollection>(this);
        m_pColumns->set_InViewMode(true);
    }
    return m_pColumns;
}

void GridView::ClearItem(ListViewItem* pItem)
{
    pItem->ClearValue(get_ColumnCollectionProperty());
    ViewBase::ClearItem(pItem);
}

void GridView::PrepareItem(ListViewItem* pItem)
{
    ViewBase::PrepareItem(pItem);
    pItem->SetValue(get_ColumnCollectionProperty(), m_pColumns);
}



};
