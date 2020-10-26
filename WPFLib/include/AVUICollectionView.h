
#pragma once

#include <AVUIEventableObject.h>
#include <AVUIIEnumerable.h>
#include <AVUIINotifyCollectionChanged.h>
#include <AVUIINotifyPropertyChanged.h>

namespace AVUI {

class IndexedEnumerable;
class SortDescriptionCollection;

class CurrentChangingEventArgs : public EventArgs
{
public:

    DECLARE_ELEMENT(CurrentChangingEventArgs, EventArgs);

    CurrentChangingEventArgs() : m_isCancelled(false), m_isCancelable(true) { }
    CurrentChangingEventArgs(bool isCancelable) : m_isCancelled(false), m_isCancelable(isCancelable) { }

    bool get_IsCancelable() { return m_isCancelable; }
    bool get_Cancel() { return m_isCancelled; }
    void set_Cancel(bool cancel) { if(!m_isCancelable && cancel) LibraryCriticalError(); m_isCancelable = cancel; }
private:
    bool m_isCancelable;
    bool m_isCancelled;
};

DEFINE_EVENTHANDLER(CurrentChanging);


class CollectionView : public EventableObject, public INotifyPropertyChanged, public INotifyCollectionChanged, public IEnumerable
{
public:
    CollectionView(Object* pEnumerable, bool shouldProcessCollectionChanged = true, int moveToFirst = 0);

    ~CollectionView() { };

    DECLARE_ELEMENT(CollectionView, Object);

    virtual ObjectPredicate* get_Filter();
    virtual void set_Filter(ObjectPredicate* pPredicate);
    virtual bool PassesFilter(Object* pItem);
    virtual int get_Count();
    virtual bool Contains(Object* pItem);
    virtual bool get_IsEmpty();
    virtual bool get_IsCurrentAfterLast();
    virtual bool get_IsCurrentBeforeFirst();
    virtual bool get_NeedsRefresh()  { return CheckFlag(CollectionViewFlags::NeedsRefresh); }
    virtual int IndexOf(Object* pObject);
    virtual Object* get_CurrentItem() { return m_pCurrentItem; }
    virtual int get_CurrentPosition() { return m_currentPosition; }
    virtual void Refresh();
    virtual ObjectPtr get_SourceCollection() { return m_pSourceCollection; }
    virtual ObjectPtr GetItemAt(int index);

    virtual bool MoveCurrentTo(Object* pItem);
    virtual bool MoveCurrentToFirst();
    virtual bool MoveCurrentToLast();
    virtual bool MoveCurrentToNext();
    virtual bool MoveCurrentToPosition(int position);
    virtual bool MoveCurrentToPrevious();


    IMPLEMENT_READONLY_REFERENCE_PROPERTY(CurrentItem, Object, false);
    IMPLEMENT_READONLY_VALUE_PROPERTY(IsEmpty, bool, false);
    IMPLEMENT_READONLY_VALUE_PROPERTY(IsCurrentAfterLast, bool, false);
    IMPLEMENT_READONLY_VALUE_PROPERTY(IsCurrentBeforeFirst, bool, false);
    IMPLEMENT_READONLY_VALUE_PROPERTY(Count, int, false);
    IMPLEMENT_READONLY_VALUE_PROPERTY(CurrentPosition, int, false);

    IMPLEMENT_EVENT(PropertyChanged, PropertyChangedEventArgs);
    IMPLEMENT_EVENT(CollectionChanged, CollectionChangedEventArgs);
    IMPLEMENT_EVENT(CurrentChanging, CurrentChangingEventArgs);
    IMPLEMENT_EVENT(CurrentChanged, EventArgs);

    ObjectPtr DeferRefresh();

    virtual bool get_CanSort() { return false; }
    virtual bool get_CanGroup() { return false; }
    virtual bool get_CanFilter() { return true; }

    virtual SortDescriptionCollection* get_SortDescriptions();

protected:
    void SetCurrent(Object* pNewItem, int newPosition) { m_currentPosition = newPosition; m_pCurrentItem = pNewItem; }
    void OnCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs);
    void OnCurrentChanged();
    virtual void OnCollectionChanged(CollectionChangedEventArgs* pArgs);
    virtual void OnPropertyChanged(PropertyChangedEventArgs* pArgs);
    void OnCurrentChanging();    
    virtual void OnCurrentChanging(CurrentChangingEventArgs* pArgs);

    virtual void ProcessCollectionChanged(CollectionChangedEventArgs* pArgs);
    void RefreshOrDefer();
    virtual void RefreshOverride();
    virtual ObjectPtr GetEnumerator();


    bool get_IsRefreshDeferred() { return m_deferLevel > 0; }
    void VerifyRefreshNotDeferred() { if(get_IsRefreshDeferred()) LibraryCriticalError(); }

    void InvalidateEnumerableWrapper();

    bool OKToChangeCurrent();

    bool get_IsCurrentInSync();

private:
    void EndDefer();
    bool get_IsCurrentInView();

    void OnPropertyChanged(const Property& property);
    void _MoveCurrentToPosition(int position);

    IndexedEnumerable* get_EnumerableWrapper(); 

    void SetFlag(CollectionViewFlags::Enum flag, bool value);
    bool CheckFlag(CollectionViewFlags::Enum flag) { return !!(m_flags & flag); }
    void AdjustCurrencyForAdd(int index);
    void AdjustCurrencyForRemove(int index);
    void AdjustCurrencyForReplace(int index);
    void AdjustCurrencyForMove(int oldIndex, int newIndex);
    void MoveCurrencyOffDeletedElement();

    class SimpleMonitor
    {
    public:
        SimpleMonitor() : m_entered(false) { }
        void Exit() { m_entered = false; }
        bool Enter() { if(m_entered) return false; m_entered = true; return true; }
        bool get_Busy() { return m_entered; }

    private:
        bool m_entered;
    };

    class DeferHelper : public Object
    {
    public:
        DeferHelper(CollectionView* pView) : m_pCollectionView(pView) { }
        virtual ~DeferHelper()
        {
            if(m_pCollectionView != NULL)
            {
                m_pCollectionView->EndDefer();
            }
        }
    private:
        TRefCountedPtr<CollectionView> m_pCollectionView;
        
    };
    


    mutable TRefCountedPtr<IndexedEnumerable> m_pEnumerableWrapper;
    ItemStructList<ObjectPtr> m_changeLog;
    SimpleMonitor m_currentChangedMonitor;
    unsigned int m_flags;
    ObjectPtr m_pSourceCollection;
    int m_currentPosition;
    ObjectPtr m_pCurrentItem;
    TRefCountedPtr<ObjectPredicate> m_pFilter;
    bool m_currentElementWasRemovedOrReplaced;
    int m_deferLevel;
};

}; // namespace AVUI
