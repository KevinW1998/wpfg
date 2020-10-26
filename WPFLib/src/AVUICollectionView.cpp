#include <AVUICommon.h>
#include <AVUICollectionView.h>
#include <AVUIIEnumerable.h>
#include <AVUIIEnumerator.h>
#include <AVUIIndexedEnumerable.h>
#include <AVUISortDescription.h>

REGISTER_ELEMENT_BEGIN(AVUI::CollectionView)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT(AVUI::CurrentChangingEventArgs);

namespace AVUI {

CollectionView::CollectionView(Object* pEnumerableObject, bool shouldProcessCollectionChanged, int moveToFirst) : m_flags(0)
{
    SetFlag(CollectionViewFlags::NeedsRefresh, true);
    SetFlag(CollectionViewFlags::ShouldProcessCollectionChanged, shouldProcessCollectionChanged);

    m_currentElementWasRemovedOrReplaced = false;
    m_deferLevel = 0;

    IEnumerable* pEnumerable = interface_cast<IEnumerable>(pEnumerableObject);
    if(pEnumerable == NULL)
    {
        LibraryCriticalError();
    }

    m_pSourceCollection = pEnumerableObject;

    INotifyCollectionChanged* pNotifyCollectionChanged = interface_cast<INotifyCollectionChanged>(pEnumerableObject);
    if(pNotifyCollectionChanged != NULL)
    {
        pNotifyCollectionChanged->add_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &CollectionView::OnCollectionChanged));
        SetFlag(CollectionViewFlags::IsDynamic, true);
    }

    ObjectPtr pEnumeratorObject = pEnumerable->GetEnumerator();
    IEnumerator* pEnumerator = interface_cast<IEnumerator>(pEnumeratorObject);

    if(moveToFirst >= 0 && pEnumerator->MoveNext())
    {
        SetCurrent(pEnumerator->get_Current(), 0);
    }
    else
    {
        SetFlag(CollectionViewFlags::IsCurrentBeforeFirst, true);
        SetCurrent(NULL, -1);
    }

    SetFlag(CollectionViewFlags::CachedIsEmpty, m_currentPosition < 0);
}

void CollectionView::SetFlag(CollectionViewFlags::Enum flag, bool value)
{
    if(value)
    {
        m_flags |= flag;
    }
    else
    {
        m_flags &= ~flag;
    }
}


void CollectionView::OnCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs)
{
    if(CheckFlag(CollectionViewFlags::ShouldProcessCollectionChanged))
    {
        ProcessCollectionChanged(pArgs);
    }
}

void CollectionView::ProcessCollectionChanged(CollectionChangedEventArgs* pArgs)
{
    ObjectPtr pOldCurrentItem = m_pCurrentItem;
    bool flag = CheckFlag(CollectionViewFlags::IsCurrentAfterLast);
    bool oldIsCurrentBeforeFirst = CheckFlag(CollectionViewFlags::IsCurrentBeforeFirst);
    int oldCurrentPosition = m_currentPosition;
    bool fireCollectionChanged = false;
    switch (pArgs->get_Action())
    {
    case NotifyCollectionChangedAction::Add:
        if(PassesFilter(pArgs->get_NewItem()))
        {
            fireCollectionChanged = true;
            AdjustCurrencyForAdd(pArgs->get_NewStartingIndex());
        }
        break;

    case NotifyCollectionChangedAction::Remove:
        if(PassesFilter(pArgs->get_OldItem()))
        {
            fireCollectionChanged = true;
            AdjustCurrencyForRemove(pArgs->get_OldStartingIndex());
        }
        break;

    case NotifyCollectionChangedAction::Replace:
        if (PassesFilter(pArgs->get_OldItem()) || PassesFilter(pArgs->get_NewItem()))
        {
            fireCollectionChanged = true;
            AdjustCurrencyForReplace(pArgs->get_OldStartingIndex());
        }
        break;

    case NotifyCollectionChangedAction::Move:
        if(PassesFilter(pArgs->get_NewItem()))
        {
            fireCollectionChanged = true;
            AdjustCurrencyForMove(pArgs->get_OldStartingIndex(), pArgs->get_NewStartingIndex());
        }
        break;

    case NotifyCollectionChangedAction::Reset:
        RefreshOrDefer();
        return;
    }

    if (fireCollectionChanged)
    {
        OnCollectionChanged(pArgs);
    }

    if (m_currentElementWasRemovedOrReplaced)
    {
        MoveCurrencyOffDeletedElement();
        m_currentElementWasRemovedOrReplaced = false;
    }
    if (get_IsCurrentAfterLast() != flag)
    {
        OnPropertyChanged(get_IsCurrentAfterLastProperty());
    }
    if (get_IsCurrentBeforeFirst() != oldIsCurrentBeforeFirst)
    {
        OnPropertyChanged(get_IsCurrentBeforeFirstProperty());
    }
    if (m_currentPosition != oldCurrentPosition)
    {
        OnPropertyChanged(get_CurrentPositionProperty());
    }
    if (m_pCurrentItem != pOldCurrentItem)
    {
        OnPropertyChanged(get_CurrentItemProperty());
    }
}


bool CollectionView::PassesFilter(Object* pItem)
{
    if (get_CanFilter() && (get_Filter() != NULL))
    {
        return get_Filter()->Invoke(pItem);
    }
    return true;
}

 
ObjectPredicate* CollectionView::get_Filter()
{
    return m_pFilter;
}

void CollectionView::set_Filter(ObjectPredicate* pFilter)
{
    if(!get_CanFilter())
    {
        LibraryCriticalError();
    }
    m_pFilter = pFilter;
    RefreshOrDefer();
}

void CollectionView::AdjustCurrencyForAdd(int index)
{
    if (get_Count() == 1)
    {
        m_currentPosition = -1;
    }
    else if (index <= m_currentPosition)
    {
        m_currentPosition++;
        if (m_currentPosition < get_Count())
        {
            m_pCurrentItem = (*get_EnumerableWrapper())[m_currentPosition];
        }
    }
}

int CollectionView::get_Count()
{
    VerifyRefreshNotDeferred();
    return get_EnumerableWrapper()->get_Count();
}

bool CollectionView::get_IsEmpty()
{
    return get_EnumerableWrapper()->get_IsEmpty();
}


void CollectionView::AdjustCurrencyForRemove(int index)
{
    if (index < m_currentPosition)
    {
        m_currentPosition--;
    }
    else if (index == m_currentPosition)
    {
        m_currentElementWasRemovedOrReplaced = true;
    }
}

void CollectionView::AdjustCurrencyForReplace(int index)
{
    if (index == m_currentPosition)
    {
        m_currentElementWasRemovedOrReplaced = true;
    }
}

void CollectionView::AdjustCurrencyForMove(int oldIndex, int newIndex)
{
    if ((oldIndex >= m_currentPosition || newIndex >= m_currentPosition) && 
        (oldIndex <= m_currentPosition || newIndex <= m_currentPosition))
    {
        if (oldIndex <= m_currentPosition)
        {
            AdjustCurrencyForRemove(oldIndex);
        }
        else if (newIndex <= m_currentPosition)
        {
            AdjustCurrencyForAdd(newIndex);
        }
    }
}

void CollectionView::RefreshOrDefer()
{
    if (get_IsRefreshDeferred())
    {
        SetFlag(CollectionViewFlags::NeedsRefresh, true);
    }
    else
    {
        Refresh();
    }
}

 
void CollectionView::OnCollectionChanged(CollectionChangedEventArgs* pArgs)
{
    invoke_CollectionChanged(this, pArgs);

    if (pArgs->get_Action() != NotifyCollectionChangedAction::Replace && pArgs->get_Action() != NotifyCollectionChangedAction::Move)
    {
        OnPropertyChanged(get_CountProperty());
    }
    bool isEmpty = get_IsEmpty();
    if (isEmpty != CheckFlag(CollectionViewFlags::CachedIsEmpty))
    {
        SetFlag(CollectionViewFlags::CachedIsEmpty, isEmpty);
        OnPropertyChanged(get_IsEmptyProperty());
    }
}

void CollectionView::OnPropertyChanged(PropertyChangedEventArgs* pArgs)
{
    invoke_PropertyChanged(this, pArgs);
}

void CollectionView::OnPropertyChanged(const Property& property)
{
    PropertyChangedEventArgs eventArgs(property);
    OnPropertyChanged(&eventArgs);
}
  
void CollectionView::MoveCurrencyOffDeletedElement()
{
    int num = get_Count() - 1;
    int position = m_currentPosition < num ? m_currentPosition : num;
    OnCurrentChanging();
    _MoveCurrentToPosition(position);
    OnCurrentChanged();
}

void CollectionView::OnCurrentChanging()
{
    m_currentPosition = -1;

    CurrentChangingEventArgs args(false);
    OnCurrentChanging(&args);
}

void CollectionView::OnCurrentChanging(CurrentChangingEventArgs* pArgs)
{
    if(m_currentChangedMonitor.get_Busy())
    {
        if(pArgs->get_IsCancelable())
        {
            pArgs->set_Cancel(true);
        }
    }
    else
    {
        invoke_CurrentChanging(this, pArgs);
    }
}

void CollectionView::OnCurrentChanged()
{
    m_currentChangedMonitor.Enter();

    EventArgs eventArgs;
    invoke_CurrentChanged(this, &eventArgs);

    m_currentChangedMonitor.Exit();
}

void CollectionView::_MoveCurrentToPosition(int position)
{
    if (position < 0)
    {
        SetFlag(CollectionViewFlags::IsCurrentBeforeFirst, true);
        SetCurrent(NULL, -1);
    }
    else if (position >= get_Count())
    {
        SetFlag(CollectionViewFlags::IsCurrentAfterLast, true);
        SetCurrent(NULL, get_Count());
    }
    else
    {
        SetFlag((CollectionViewFlags::Enum)(CollectionViewFlags::IsCurrentAfterLast | CollectionViewFlags::IsCurrentBeforeFirst), false);
        SetCurrent((*get_EnumerableWrapper())[position], position);
    }
}
 
void CollectionView::Refresh()
{
    RefreshOverride();
    SetFlag(CollectionViewFlags::NeedsRefresh, false);
}

void CollectionView::RefreshOverride()
{
    if (get_SortDescriptions() != NULL && get_SortDescriptions()->get_Count() > 0)
    {
        LibraryCriticalError();
    }

    ObjectPtr pItem = m_pCurrentItem;

    bool oldIsCurrentAfterLast = CheckFlag(CollectionViewFlags::IsCurrentAfterLast);
    bool oldIsCurrentBeforeFirst = CheckFlag(CollectionViewFlags::IsCurrentBeforeFirst);
    int num = m_currentPosition;

    OnCurrentChanging();
    InvalidateEnumerableWrapper();
    if (get_IsEmpty() || oldIsCurrentBeforeFirst)
    {
        _MoveCurrentToPosition(-1);
    }
    else if (oldIsCurrentAfterLast)
    {
        _MoveCurrentToPosition(get_Count());
    }
    else if (pItem != NULL)
    {
        int position = get_EnumerableWrapper()->IndexOf(pItem);
        if (position < 0)
        {
            position = 0;
        }
        _MoveCurrentToPosition(position);
    }
    CollectionChangedEventArgs resetArgs(NotifyCollectionChangedAction::Reset);
    OnCollectionChanged(&resetArgs);
    OnCurrentChanged();

    if (get_IsCurrentAfterLast() != oldIsCurrentAfterLast)
    {
        OnPropertyChanged(get_IsCurrentAfterLastProperty());
    }
    if (get_IsCurrentBeforeFirst() != oldIsCurrentBeforeFirst)
    {
        OnPropertyChanged(get_IsCurrentBeforeFirstProperty());
    }
    if (num != get_CurrentPosition())
    {
        OnPropertyChanged(get_CurrentPositionProperty());
    }
    if (pItem != get_CurrentItem())
    {
        OnPropertyChanged(get_CurrentItemProperty());
    }
}
 
void CollectionView::InvalidateEnumerableWrapper()
{
    TRefCountedPtr<IndexedEnumerable> pEnumerableWrapper = m_pEnumerableWrapper;

    m_pEnumerableWrapper = NULL;

    if (pEnumerableWrapper != NULL)
    {
        pEnumerableWrapper->Invalidate();
    }
}

ObjectPtr CollectionView::GetItemAt(int index)
{
    if (index < 0)
    {
        LibraryCriticalError();
    }
    return (*get_EnumerableWrapper())[index];
}

int CollectionView::IndexOf(Object* pObject)
{
    VerifyRefreshNotDeferred();
    return get_EnumerableWrapper()->IndexOf(pObject);
}
 
bool CollectionView::get_IsCurrentAfterLast()
{
    VerifyRefreshNotDeferred();
    if (!get_IsEmpty())
    {
        return (get_CurrentPosition() >= get_Count());
    }
    return true;
}
 
bool CollectionView::get_IsCurrentBeforeFirst()
{
    VerifyRefreshNotDeferred();
    if (!get_IsEmpty())
    {
        return (get_CurrentPosition() < 0);
    }

    return true;
}
 

IndexedEnumerable* CollectionView::get_EnumerableWrapper()
{
    if (m_pEnumerableWrapper == NULL)
    {
        m_pEnumerableWrapper = object_allocate<IndexedEnumerable>(get_SourceCollection(), ObjectPredicate::Create(this, &CollectionView::PassesFilter));
    }
    return m_pEnumerableWrapper;
}

bool CollectionView::OKToChangeCurrent()
{
    CurrentChangingEventArgs args;
    OnCurrentChanging(&args);
    return !args.get_Cancel();
}
 
ObjectPtr CollectionView::GetEnumerator()
{
    VerifyRefreshNotDeferred();

    if (get_SortDescriptions() != NULL && get_SortDescriptions()->get_Count() > 0)
    {
        LibraryCriticalError();
    }
    return get_EnumerableWrapper()->GetEnumerator();
}

ObjectPtr CollectionView::DeferRefresh()
{
    m_deferLevel++;
    return (Object*) object_allocate<DeferHelper>(this);
}

void CollectionView::EndDefer()
{
    m_deferLevel--;
    if ((m_deferLevel == 0) && CheckFlag(CollectionViewFlags::NeedsRefresh))
    {
        Refresh();
    }
}

bool CollectionView::Contains(Object* pItem)
{
    VerifyRefreshNotDeferred();
    return (IndexOf(pItem) >= 0);
}
 
bool CollectionView::MoveCurrentTo(Object* pItem)
{
    VerifyRefreshNotDeferred();
    if (Object::Equals(get_CurrentItem(), pItem) && ((pItem != NULL) || get_IsCurrentInView()))
    {
        return get_IsCurrentInView();
    }
    int position = -1;
    if (PassesFilter(pItem))
    {
        position = IndexOf(pItem);
    }
    return MoveCurrentToPosition(position);
}

bool CollectionView::MoveCurrentToFirst()
{
    VerifyRefreshNotDeferred();
    return MoveCurrentToPosition(0);
}

bool CollectionView::MoveCurrentToLast()
{
    VerifyRefreshNotDeferred();
    return MoveCurrentToPosition(get_Count() - 1);
}

bool CollectionView::MoveCurrentToNext()
{
    VerifyRefreshNotDeferred();
    if(get_CurrentPosition() < get_Count())
    {
        return MoveCurrentToPosition(get_CurrentPosition() + 1);
    }
    return false;
}

bool CollectionView::MoveCurrentToPosition(int position)
{
    VerifyRefreshNotDeferred();
    if ((position < -1) || (position > get_Count()))
    {
        LibraryCriticalError();
    }
    if (((position != get_CurrentPosition()) || !get_IsCurrentInSync()) && OKToChangeCurrent())
    {
        bool isCurrentAfterLast = get_IsCurrentAfterLast();
        bool isCurrentBeforeFirst = get_IsCurrentBeforeFirst();
        _MoveCurrentToPosition(position);
        OnCurrentChanged();

        if (get_IsCurrentAfterLast() != isCurrentAfterLast)
        {
            OnPropertyChanged(get_IsCurrentAfterLastProperty());
        }
        if (get_IsCurrentBeforeFirst() != isCurrentBeforeFirst)
        {
            OnPropertyChanged(get_IsCurrentBeforeFirstProperty());
        }
        OnPropertyChanged(get_CurrentPositionProperty());
        OnPropertyChanged(get_CurrentItemProperty());
    }
    return get_IsCurrentInView();
}

bool CollectionView::MoveCurrentToPrevious()
{
    VerifyRefreshNotDeferred();
    if (get_CurrentPosition() >= 0)
    {
        return MoveCurrentToPosition(get_CurrentPosition() - 1);
    }
    return false;
}

bool CollectionView::get_IsCurrentInSync()
{
    if (get_IsCurrentInView())
    {
        return (GetItemAt(get_CurrentPosition()) == get_CurrentItem());
    }
    return (get_CurrentItem() == NULL);
}

bool CollectionView::get_IsCurrentInView()
{
    VerifyRefreshNotDeferred();
    if (0 <= get_CurrentPosition())
    {
        return (get_CurrentPosition() < get_Count());
    }
    return false;
}

SortDescriptionCollection* CollectionView::get_SortDescriptions()
{
    return SortDescriptionCollection::get_Empty();
}


}; // namespace AVUI
