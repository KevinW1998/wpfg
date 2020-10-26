#include <AVUICommon.h>
#include <AVUIGridViewColumnCollection.h>
#include <AVUIINotifyPropertyChanged.h>

REGISTER_ELEMENT_BEGIN(AVUI::GridViewColumnCollectionChangedEventArgs)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::GridViewColumnCollection)

REGISTER_ELEMENT_END()

namespace AVUI {

void GridViewColumnCollection::OnAdd(GridViewColumn* pAdd)
{
    pAdd->add_PropertyChanged(PropertyChangedEventHandler::Create(this, &GridViewColumnCollection::ColumnPropertyChanged));

    if(m_pOwner != NULL)
    {
        m_pOwner->ProvideSelfAsInheritanceContext(pAdd, NULL);
    }

    pAdd->set_ActualIndex(IndexOf(pAdd));
}

void GridViewColumnCollection::OnRemove(GridViewColumn* pRemove)
{
    pRemove->add_PropertyChanged(PropertyChangedEventHandler::Create(this, &GridViewColumnCollection::ColumnPropertyChanged));

    if(m_pOwner != NULL)
    {
        m_pOwner->RemoveSelfAsInheritanceContext(pRemove, NULL);
    }
}

void GridViewColumnCollection::ColumnPropertyChanged(Object* pSender, PropertyChangedEventArgs* pArgs)
{
    GridViewColumn* pColumn = object_cast<GridViewColumn>(pSender);

    GridViewColumnCollectionChangedEventArgs eventArgs(pColumn, &pArgs->get_Property());
    invoke_InternalCollectionChanged(this, &eventArgs);
}

}; // namespace AVUI
