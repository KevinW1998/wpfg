#include <AVUICommon.h>
#include <AVUIGridViewRowPresenterBase.h>
#include <AVUIGridViewColumnCollection.h>
#include <AVUIGridViewColumn.h>
#include <AVUIItemsControl.h>

REGISTER_ELEMENT_BEGIN(AVUI::GridViewRowPresenterBase)

    REGISTER_PROPERTY(Columns)
REGISTER_ELEMENT_END()


namespace AVUI
{

GridViewRowPresenterBase::~GridViewRowPresenterBase()
{
    GridViewColumnCollection* pColumns = get_Columns();

    if(pColumns != NULL)
    {
        pColumns->remove_InternalCollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &GridViewRowPresenterBase::ColumnCollectionChanged));
    }

    m_children.InvalidateWeakReferences();
}


void GridViewRowPresenterBase::EnsureDesiredWidthList()
{
    GridViewColumnCollection* pColumns = get_Columns();
    if(pColumns == NULL)
    {
        return;
    }

    int capacity = pColumns->get_Count();

    int toAdd = capacity - m_desiredWidthList.get_Count();
    for (int i = 0; i < toAdd; i++)
    {
        m_desiredWidthList.AddItem(Float::get_NaN());
    }
}

void GridViewRowPresenterBase::ColumnsPropertyChanged(Object* pObject, const DependencyPropertyChangedEventArgs& e)
{
    GridViewRowPresenterBase* pListener = object_cast<GridViewRowPresenterBase>(pObject);
    GridViewColumnCollection* pOldValue = object_cast<GridViewColumnCollection>(e.pOldValue);
    if (pOldValue != NULL)
    {
        pOldValue->remove_InternalCollectionChanged(NotifyCollectionChangedEventHandler::Create(pListener, &GridViewRowPresenterBase::ColumnCollectionChanged));
        if (!pOldValue->get_InViewMode() && pOldValue->get_Owner() == pListener->GetStableAncestor())
        {
            pOldValue->set_Owner(NULL);
        }
    }
    GridViewColumnCollection* pNewValue = object_cast<GridViewColumnCollection>(e.pNewValue);
    if (pNewValue != NULL)
    {
        pNewValue->add_InternalCollectionChanged(NotifyCollectionChangedEventHandler::Create(pListener, &GridViewRowPresenterBase::ColumnCollectionChanged));

        if (!pNewValue->get_InViewMode() && pNewValue->get_Owner() == NULL)
        {
            pNewValue->set_Owner(pListener->GetStableAncestor());
        }

    }
    pListener->set_NeedUpdateVisualTree(true);
    pListener->InvalidateMeasure();
}


DependencyObject* GridViewRowPresenterBase::GetStableAncestor()
{
    ItemsControl* pControl = ItemsControl::ItemsControlFromItemContainer(get_TemplatedParent());
    if (pControl == NULL)
    {
        return this;
    }
    return pControl;
}

void GridViewRowPresenterBase::ColumnCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArg)
{
    GridViewColumnCollectionChangedEventArgs* pGVArgs = object_cast<GridViewColumnCollectionChangedEventArgs>(pArg);
    if (pGVArgs != NULL && get_IsPresenterVisualReady())
    {
        if (pGVArgs->pColumn != NULL)
        {
            OnColumnPropertyChanged(pGVArgs->pColumn, pGVArgs->pProperty);
        }
        else
        {
            OnColumnCollectionChanged(pGVArgs);
        }
    }
}

void GridViewRowPresenterBase::OnColumnCollectionChanged(GridViewColumnCollectionChangedEventArgs* pArgs)
{
    if (m_desiredWidthList.get_Count() > 0)
    {
        if ((pArgs->get_Action() == NotifyCollectionChangedAction::Remove) || (pArgs->get_Action() == NotifyCollectionChangedAction::Replace))
        {
            if (m_desiredWidthList.get_Count() > pArgs->ActualIndex)
            {
                m_desiredWidthList.RemoveAt(pArgs->ActualIndex);
            }
        }
        else if (pArgs->get_Action() == NotifyCollectionChangedAction::Reset)
        {
            m_desiredWidthList.Clear();
        }
    }
}

};


