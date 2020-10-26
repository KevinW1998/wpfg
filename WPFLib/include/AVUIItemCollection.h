#pragma once

#include <AVUICollectionView.h>
#include <AVUICollectionViewSource.h>
#include <AVUIObjectCollection.h>

namespace AVUI {
class UIElement;
class InnerItemCollectionView;

class CollectionChangedEventArgs;
class CurrentChangingEventArgs;
class PropertyChangedEventArgs;

class ItemCollection : public CollectionView, public ICollection
{
    friend class ItemsControl;
public:

    DECLARE_ELEMENT(ItemCollection, CollectionView);

    ItemCollection(DependencyObject* pModelParent, int capacity = 0x10);

    UIElement* get_ModelParentFE();

    void Add(Object* pNewItem);
    void Clear();

    bool get_IsUsingItemsSource() { return m_isUsingItemsSource; }

    bool get_IsEmpty();
    int get_Count();
    Object* get_CurrentItem();
    int get_CurrentPosition();
    ObjectPredicate* get_Filter();
    void set_Filter(ObjectPredicate* pPredicate);
    bool get_IsCurrentAfterLast();
    bool get_IsCurrentBeforeFirst();
    bool get_NeedsRefresh();

    bool Contains(Object* pItem);
    ObjectPtr DeferRefresh();
    ObjectPtr GetItemAt(int index);
    int IndexOf(Object* pObject);
    void Insert(int insertIndex, Object* pInsertItem);

    bool MoveCurrentTo(Object* pItem);
    bool MoveCurrentToFirst();
    bool MoveCurrentToLast();
    bool MoveCurrentToNext();
    bool MoveCurrentToPosition(int position);
    bool MoveCurrentToPrevious();
    bool PassesFilter(Object* pItem);
    bool Remove(Object* pObject);
    void RemoveAt(int idx);
    void Move(int oldIndex, int newIndex);

    virtual ObjectPtr GetEnumerator();

    virtual bool get_CanSort() { return (!EnsureCollectionView() || m_pCollectionView->get_CanSort()); }
    virtual bool get_CanGroup() { if(!EnsureCollectionView()) return false; return m_pCollectionView->get_CanGroup(); }
    virtual bool get_CanFilter() { return (!EnsureCollectionView() || m_pCollectionView->get_CanFilter()); }

    virtual SortDescriptionCollection* get_SortDescriptions();

protected:
    void RefreshOverride();

    bool get_IsRefreshDeferred() { return m_deferLevel > 0; }
    void VerifyRefreshNotDeferred() { if(get_IsRefreshDeferred()) LibraryCriticalError(); }

private:
    void OnInnerSortDescriptionsChanged(Object* pSender, CollectionChangedEventArgs* pArgs);

    void SetItemsSource(Object* pEnumerable);
    void EndDefer();

    void BeginInit();
    void EndInit();
    void ClearItemsSource();

    void ApplySortFilterAndGroup();
    void HookCollectionView(CollectionView* pView);
    void UnhookCollectionView(CollectionView* pView);

    bool EnsureCollectionView();
    void EnsureInternalView();
    void CheckIsUsingInnerView();
    void SetCollectionView(CollectionView* pView);

    void OnInnerCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs);
    void OnInnerCurrentChanging(Object* pSender, CurrentChangingEventArgs* pArgs);
    void OnInnerCurrentChanged(Object* pSender, EventArgs* pArgs);
    void OnInnerPropertyChanged(Object* pSender, PropertyChangedEventArgs* pArgs);
    void CloneList(ICollection* pClone, ICollection* pMaster);
    void SortDescriptionsChanged(Object* pSender, CollectionChangedEventArgs* pArgs);
    void SynchronizeSortDescriptions(CollectionChangedEventArgs* pArgs, SortDescriptionCollection* pOrigin, SortDescriptionCollection* pClone);

 

    class DeferHelper : public Object
    {
    public:
        DeferHelper(ItemCollection* pCollection) : m_pCollection(pCollection) { }
        virtual ~DeferHelper()
        {
            if(m_pCollection != NULL)
            {
                m_pCollection->EndDefer();
            }
        }
    private:
        TRefCountedPtr<ItemCollection> m_pCollection;

    };


    TRefCountedPtr<CollectionView> m_pCollectionView;
    int m_defaultCapacity;
    int m_deferLevel;
    TRefCountedPtr<ObjectPredicate> m_pFilter;
    // group by
    // group by monitor
    TRefCountedPtr<InnerItemCollectionView> m_pInternalView;
    bool m_isGroupingSet;
    bool m_isInitializing;
    bool m_isSortingSet;
    bool m_isUsingItemsSource;

    ObjectPtr m_pItemsSourceObject;
    IEnumerable* m_pItemsSource;

    TWeakPtr<DependencyObject> m_pModelParent;
    ObjectPtr m_pDeferInnerRefresh;
    TRefCountedPtr<SortDescriptionCollection> m_pSort;
};

}; // namespace AVUI
