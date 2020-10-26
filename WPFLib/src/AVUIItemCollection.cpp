#include <AVUICommon.h>
#include <AVUIItemCollection.h>
#include <AVUIInnerItemCollectionView.h>
#include <AVUIUIElement.h>
#include <AVUIItemsControl.h>
#include <AVUISortDescription.h>

REGISTER_ELEMENT_BEGIN(AVUI::ItemCollection)

    REGISTER_INTERFACE(ICollection)
REGISTER_ELEMENT_END()


namespace AVUI {

ItemCollection::ItemCollection(DependencyObject* pModelParent, int capacity) : CollectionView(EmptyEnumerable::get_Instance(), false), 
                                                                               m_pModelParent(pModelParent), m_defaultCapacity(capacity),
                                                                               m_isUsingItemsSource(false), m_deferLevel(0), m_isInitializing(false)
{

}

UIElement* ItemCollection::get_ModelParentFE()
{
    return object_cast<UIElement>(m_pModelParent);
}



void ItemCollection::Add(Object* pNewItem)
{
    CheckIsUsingInnerView();
    m_pInternalView->Add(pNewItem);

    m_pModelParent->SetValue(ItemsControl::get_HasItemsProperty(), BoxValue(true));
}

void ItemCollection::Move(int oldIndex, int newIndex)
{
    ObjectPtr pItem = GetItemAt(oldIndex);

    RemoveAt(oldIndex);
    Insert(newIndex, pItem);
}


void ItemCollection::Clear()
{
    VerifyRefreshNotDeferred();
    if(get_IsUsingItemsSource())
    {
        LibraryCriticalError();
    }
    if(m_pInternalView)
    {
        m_pInternalView->Clear();
    }

    m_pModelParent->ClearValue(ItemsControl::get_HasItemsProperty());
}


void ItemCollection::CheckIsUsingInnerView()
{
    if(get_IsUsingItemsSource())
    {
        LibraryCriticalError();
    }
    EnsureInternalView();
    EnsureCollectionView();
    VerifyRefreshNotDeferred();
}

bool ItemCollection::EnsureCollectionView()
{
    if(m_pCollectionView == NULL && !get_IsUsingItemsSource() && m_pInternalView != NULL)
    {
        if (m_pInternalView->get_IsEmpty())
        {
            bool oldIsInitializing = m_isInitializing;
            m_isInitializing = true;
            SetCollectionView(m_pInternalView);
            m_isInitializing = oldIsInitializing;
        }
        else
        {
            SetCollectionView(m_pInternalView);
        }
        if (!m_isInitializing)
        {
            HookCollectionView(m_pCollectionView);
        }
    }
    return (m_pCollectionView != NULL);
}

void ItemCollection::EnsureInternalView()
{
    if (m_pInternalView == NULL)
    {
        m_pInternalView = object_allocate<InnerItemCollectionView>(m_defaultCapacity, this);
    }
}

void ItemCollection::SetCollectionView(CollectionView* pView)
{
    if (m_pCollectionView != pView)
    {
        if (m_pCollectionView != NULL)
        {
            if (!m_isInitializing)
            {
                UnhookCollectionView(m_pCollectionView);
            }
            if (get_IsRefreshDeferred())
            {
                m_pDeferInnerRefresh = NULL;
            }
        }
        bool fireCollectionChanged = false;
        m_pCollectionView = pView;
        CollectionView::InvalidateEnumerableWrapper();
        if (m_pCollectionView != NULL)
        {
            m_pDeferInnerRefresh = m_pCollectionView->DeferRefresh();

            ApplySortFilterAndGroup();

            if (!m_isInitializing)
            {
                HookCollectionView(m_pCollectionView);
            }
            if (!get_IsRefreshDeferred())
            {
                fireCollectionChanged = !m_pCollectionView->get_NeedsRefresh();
                m_pDeferInnerRefresh = NULL;
            }
        }
        else if (!get_IsRefreshDeferred())
        {
            fireCollectionChanged = true;
        }
        if (fireCollectionChanged)
        {
            CollectionChangedEventArgs args(NotifyCollectionChangedAction::Reset);
            OnCollectionChanged(&args);
        }
    }
}

void ItemCollection::HookCollectionView(CollectionView* pView)
{
    pView->add_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &ItemCollection::OnInnerCollectionChanged));
    pView->add_CurrentChanging(CurrentChangingEventHandler::Create(this, &ItemCollection::OnInnerCurrentChanging));
    pView->add_CurrentChanged(EventHandler::Create(this, &ItemCollection::OnInnerCurrentChanged));
    pView->add_PropertyChanged(PropertyChangedEventHandler::Create(this, &ItemCollection::OnInnerPropertyChanged));
    /*
    // TBD -- Sorting
    SortDescriptionCollection source = view.SortDescriptions;
    if ((source != null) && (source != SortDescriptionCollection.Empty))
    {
        CollectionChangedEventManager.AddListener(source, this);
    }
    ObservableCollection<GroupDescription> groupDescriptions = view.GroupDescriptions;
    if (groupDescriptions != null)
    {
        CollectionChangedEventManager.AddListener(groupDescriptions, this);
    }
    */
}

void ItemCollection::UnhookCollectionView(CollectionView* pView)
{
    pView->remove_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &ItemCollection::OnInnerCollectionChanged));
    pView->remove_CurrentChanging(CurrentChangingEventHandler::Create(this, &ItemCollection::OnInnerCurrentChanging));
    pView->remove_CurrentChanged(EventHandler::Create(this, &ItemCollection::OnInnerCurrentChanged));
    pView->remove_PropertyChanged(PropertyChangedEventHandler::Create(this, &ItemCollection::OnInnerPropertyChanged));

    /*
    // TBD -- Sorting
    SortDescriptionCollection source = view.SortDescriptions;
    if ((source != null) && (source != SortDescriptionCollection.Empty))
    {
        CollectionChangedEventManager.RemoveListener(source, this);
    }
    ObservableCollection<GroupDescription> groupDescriptions = view.GroupDescriptions;
    if (groupDescriptions != null)
    {
        CollectionChangedEventManager.RemoveListener(groupDescriptions, this);
    }
    */
}

void ItemCollection::OnInnerCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs)
{    
    if (m_pCollectionView != NULL && pSender == m_pCollectionView->get_SortDescriptions())
    {
        OnInnerSortDescriptionsChanged(pSender, pArgs);
    }
/*
    else if (m_pCollectionView != NULL && pSender == m_pCollectionView->get_GroupDescriptions()))
    {
        OnInnerGroupDescriptionsChanged(sender, args);
    }
    */
    else
    {
        CollectionView::InvalidateEnumerableWrapper();
        OnCollectionChanged(pArgs);
    }
}

void ItemCollection::OnInnerSortDescriptionsChanged(Object* pSender, CollectionChangedEventArgs* pArgs)
{
    SynchronizeSortDescriptions(pArgs, m_pCollectionView->get_SortDescriptions(), m_pSort);
    m_isSortingSet = false;
}

void ItemCollection::OnInnerCurrentChanging(Object* pSender, CurrentChangingEventArgs* pArgs)
{
    CollectionView::OnCurrentChanging(pArgs);
}

void ItemCollection::OnInnerCurrentChanged(Object* pSender, EventArgs* pArgs)
{
    CollectionView::OnCurrentChanged();
}

void ItemCollection::OnInnerPropertyChanged(Object* pSender, PropertyChangedEventArgs* pArgs)
{
    CollectionView::OnPropertyChanged(pArgs);
}

bool ItemCollection::get_IsEmpty()
{
    if (!EnsureCollectionView())
    {
        return true;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->get_IsEmpty();
}

int ItemCollection::get_Count()
{
    if (!EnsureCollectionView())
    {
        return 0;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->get_Count();
}

void ItemCollection::BeginInit()
{
    m_isInitializing = true;
    if (m_pCollectionView != NULL)
    {
        UnhookCollectionView(m_pCollectionView);
    }

}

void ItemCollection::EndInit()
{
    EnsureCollectionView();
    m_isInitializing = false;
    if (m_pCollectionView != NULL)
    {
        HookCollectionView(m_pCollectionView);
        Refresh();
    }
}

void ItemCollection::ClearItemsSource()
{
    if (get_IsUsingItemsSource())
    {
        m_pItemsSourceObject = NULL;
        m_pItemsSource = NULL;
        m_isUsingItemsSource = false;
        SetCollectionView(m_pInternalView);
    }
}

bool ItemCollection::Contains(Object* pItem)
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->Contains(pItem);
}

ObjectPtr ItemCollection::DeferRefresh()
{
    m_deferLevel++;
    if ((m_deferLevel == 1) && (m_pCollectionView != NULL))
    {
        m_pDeferInnerRefresh = m_pCollectionView->DeferRefresh();
    }
    return (Object*) object_allocate<DeferHelper>(this);
}

void ItemCollection::EndDefer()
{
    m_deferLevel--;
    if (m_deferLevel == 0)
    {
        if (m_pDeferInnerRefresh != NULL)
        {
            m_pDeferInnerRefresh = NULL;
        }
        else
        {
            CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Reset);
            OnCollectionChanged(&eventArgs);
        }
    }
}
 
ObjectPtr ItemCollection::GetEnumerator()
{
    if(!EnsureCollectionView())
    {
        return EmptyEnumerator::get_Instance();
    }

    return static_cast<IEnumerable*>(m_pCollectionView)->GetEnumerator();
}
 
ObjectPtr ItemCollection::GetItemAt(int index)
{
    if (index < 0)
    {
        LibraryCriticalError();
    }
    VerifyRefreshNotDeferred();
    if (!EnsureCollectionView())
    {
        LibraryCriticalError();
    }
    if (m_pCollectionView == m_pInternalView && index >= m_pInternalView->get_Count())
    {
        LibraryCriticalError();
    }
    return m_pCollectionView->GetItemAt(index);
}

int ItemCollection::IndexOf(Object* pItem)
{
    if (!EnsureCollectionView())
    {
        return -1;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->IndexOf(pItem);
}

void ItemCollection::Insert(int insertIndex, Object* pInsertItem)
{
    CheckIsUsingInnerView();
    m_pInternalView->Insert(insertIndex, pInsertItem);
    m_pModelParent->SetValue(ItemsControl::get_HasItemsProperty(), BoxValue(true));
}

 
bool ItemCollection::MoveCurrentTo(Object* pItem)
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->MoveCurrentTo(pItem);
}

bool ItemCollection::MoveCurrentToFirst()
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->MoveCurrentToFirst();
}

bool ItemCollection::MoveCurrentToLast()
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->MoveCurrentToLast();
}

bool ItemCollection::MoveCurrentToNext()
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->MoveCurrentToNext();
}

bool ItemCollection::MoveCurrentToPosition(int position)
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->MoveCurrentToPosition(position);
}

bool ItemCollection::MoveCurrentToPrevious()
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->MoveCurrentToPrevious();
}

bool ItemCollection::PassesFilter(Object* pItem)
{
    return (!EnsureCollectionView() || m_pCollectionView->PassesFilter(pItem));
}
 
void ItemCollection::RefreshOverride()
{
    if (m_pCollectionView != NULL)
    {
        if (m_pCollectionView->get_NeedsRefresh())
        {
            m_pCollectionView->Refresh();
        }
        else
        {
            CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Reset);
            OnCollectionChanged(&eventArgs);
        }
    }
}

bool ItemCollection::Remove(Object* pObject)
{
    CheckIsUsingInnerView();
    bool isRemoved = m_pInternalView->Remove(pObject);
    if (get_IsEmpty())
    {
        m_pModelParent->ClearValue(ItemsControl::get_HasItemsProperty());
    }

    return isRemoved;
}

void ItemCollection::RemoveAt(int idx)
{
    CheckIsUsingInnerView();
    m_pInternalView->RemoveAt(idx);
    if (get_IsEmpty())
    {
        m_pModelParent->ClearValue(ItemsControl::get_HasItemsProperty());
    }
}

void ItemCollection::SetItemsSource(Object* pEnumerable)
{
    if ((!get_IsUsingItemsSource() && (m_pInternalView != NULL)) && (m_pInternalView->get_RawCount() > 0))
    {
        LibraryCriticalError();
    }
    m_pItemsSourceObject = pEnumerable;
    m_pItemsSource = interface_cast<IEnumerable>(pEnumerable);

    m_isUsingItemsSource = true;

    SetCollectionView(CollectionViewSource::GetDefaultCollectionView(m_pItemsSourceObject, m_pModelParent));
}

Object* ItemCollection::get_CurrentItem()
{
    if (!EnsureCollectionView())
    {
        return NULL;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->get_CurrentItem();
}

int ItemCollection::get_CurrentPosition()
{
    if (!EnsureCollectionView())
    {
        return -1;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->get_CurrentPosition();
}
 
ObjectPredicate* ItemCollection::get_Filter()
{
    if (!EnsureCollectionView())
    {
        return m_pFilter;
    }
    return m_pCollectionView->get_Filter();
}

void ItemCollection::set_Filter(ObjectPredicate* pPredicate)
{
    m_pFilter = pPredicate;
    if (m_pCollectionView != NULL)
    {
        m_pCollectionView->set_Filter(pPredicate);
    }
}

bool ItemCollection::get_IsCurrentAfterLast()
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->get_IsCurrentAfterLast();
}

bool ItemCollection::get_IsCurrentBeforeFirst()
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->get_IsCurrentBeforeFirst();
}

bool ItemCollection::get_NeedsRefresh()
{
    if (!EnsureCollectionView())
    {
        return false;
    }
    VerifyRefreshNotDeferred();
    return m_pCollectionView->get_NeedsRefresh();
}

void ItemCollection::ApplySortFilterAndGroup()
{
    if (m_pCollectionView->get_CanSort())
    {
        SortDescriptionCollection* pMaster = m_isSortingSet ? m_pSort : m_pCollectionView->get_SortDescriptions();
        SortDescriptionCollection* pClone = m_isSortingSet ? m_pCollectionView->get_SortDescriptions() : m_pSort;
        CloneList(pClone, pMaster);
    }
    if (m_pCollectionView->get_CanFilter() && m_pFilter != NULL)
    {
        m_pCollectionView->set_Filter(m_pFilter);
    }
    /*
    if (m_pCollectionView->get_CanGroup())
    {
        ObservableCollection<GroupDescription>* pObservables = m_isGroupingSet ? m_pGroupBy : m_pCollectionView->get_GroupDescriptions();
        ObservableCollection<GroupDescription>* pObservables2 = m_isGroupingSet ? m_pCollectionView->get_GroupDescriptions() : m_pGroupBy;
        CloneList(pObservables2, pObservables);
    }
    */
}

SortDescriptionCollection* ItemCollection::get_SortDescriptions()
{
    if (m_pSort == NULL)
    {
        m_pSort = object_allocate<SortDescriptionCollection>();
        if (m_pCollectionView != NULL)
        {
            CloneList(m_pSort, m_pCollectionView->get_SortDescriptions());
        }
        m_pSort->add_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &ItemCollection::SortDescriptionsChanged));
    }
    return m_pSort;
}

void ItemCollection::CloneList(ICollection* pClone, ICollection* pMaster)
{
    if (pClone != NULL && pMaster != NULL)
    {
        if (pClone->get_Count() > 0)
        {
            pClone->Clear();
        }
        int num = 0;
        int count = pMaster->get_Count();
        while (num < count)
        {
            pClone->Add(pMaster->GetItemAt(num));
            num++;
        }
    }
}

void ItemCollection::SortDescriptionsChanged(Object* pSender, CollectionChangedEventArgs* pArgs)
{
    if (m_pCollectionView != NULL && m_pCollectionView->get_CanSort())
    {
        SynchronizeSortDescriptions(pArgs, m_pSort, m_pCollectionView->get_SortDescriptions());
    }
    m_isSortingSet = true;
}

void ItemCollection::SynchronizeSortDescriptions(CollectionChangedEventArgs* pArgs, SortDescriptionCollection* pOrigin, SortDescriptionCollection* pClone)
{
   if (pClone != NULL)
    {
        switch (pArgs->get_Action())
        {
        case NotifyCollectionChangedAction::Add:
            pClone->Insert(pArgs->get_NewStartingIndex(), UnboxValue<SortDescription>(pArgs->get_NewItem()));
            return;

        case NotifyCollectionChangedAction::Remove:
            pClone->RemoveAt(pArgs->get_OldStartingIndex());
            return;

        case NotifyCollectionChangedAction::Replace:
            pClone->SetItemAt(pArgs->get_OldStartingIndex(), pArgs->get_NewItem());
            return;

        case NotifyCollectionChangedAction::Move:
            pClone->RemoveAt(pArgs->get_OldStartingIndex());
            pClone->Insert(pArgs->get_NewStartingIndex(), UnboxValue<SortDescription>(pArgs->get_NewItem()));
            return;

        case NotifyCollectionChangedAction::Reset:
            CloneList(pClone, pOrigin);
            return;
        }
        LibraryCriticalError();
    }
}


}; // namespace AVUI
