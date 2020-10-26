#pragma once

#include <AVUIObject.h>
#include <AVUICollectionView.h>
#include <AVUIIEnumerable.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class ItemCollection;

class InnerItemCollectionView : public CollectionView, public ICollection
{
    friend class ItemCollection;
public:
    DECLARE_ELEMENT(InnerItemCollectionView, CollectionView);

    InnerItemCollectionView(int capacity, ItemCollection* pCollection) : CollectionView(EmptyEnumerable::get_Instance(), false)
    {
        m_pRawList = object_allocate<ItemStructList<ObjectPtr> >(capacity);
        m_pViewList = m_pRawList;
        m_pItemCollection = pCollection;
        m_currentElementWasRemoved = false;
        m_isModified = false;
    }

    void Add(Object* pItem);
    void Clear();
    bool Contains(Object* pItem);
    ObjectPtr GetItemAt(int index);
    void SetItemAt(int index, Object* pValue);
    int IndexOf(Object* pItem);
    void Insert(int index, Object* pItem);
    bool Remove(Object* pItem);
    void RemoveAt(int index);
    void Move(int oldIndex, int newIndex);

    bool MoveCurrentTo(Object* pItem);
    bool MoveCurrentToPosition(int position);
    bool get_IsEmpty() { return get_ViewCount() == 0; }
    int get_Count() { return get_ViewCount(); }

    virtual bool get_CanSort() { return true; }

    virtual SortDescriptionCollection* get_SortDescriptions();

protected:
    ObjectPtr GetEnumerator();
    void RefreshOverride();
 
private:

    int get_RawCount() { return m_pRawList->get_Count(); }
    void OnPropertyChanged(const Property& property);

    bool get_IsCurrentInView();
    ItemCollection* get_ItemCollection() { return m_pItemCollection; }
    bool get_IsCachedMode() { return m_pViewList != m_pRawList; }
    void ClearModelParent(Object* pItem);
    int get_ViewCount() const { return m_pViewList->get_Count(); }
    void _MoveCurrentToPosition(int position);     
    void _RemoveAt(int index, int indexR, Object* pItem);
    void AdjustCurrencyForRemove(int index);
    void MoveCurrencyOffDeletedElement();
    void AdjustCurrencyForAdd(int index);
    DependencyObject* AssertPristineModelChild(Object* pItem);
    void SetModelParent(Object* pItem);
    void ClearIsModified() { m_isModified = false; }
    void SortDescriptionsChanged(Object* pSender, CollectionChangedEventArgs* pArgs);

    UIElement* get_ModelParentFE();
 
    void SetIsModified() { if(get_IsCachedMode()) m_isModified = true; }

    void SetSortDescriptions(SortDescriptionCollection* pDescriptions);

    bool m_currentElementWasRemoved;
    bool m_isModified;

    TWeakPtr<ItemCollection> m_pItemCollection;
    TRefCountedPtr< ItemStructList<ObjectPtr> > m_pRawList;
    TRefCountedPtr< ItemStructList<ObjectPtr> > m_pViewList;
    TRefCountedPtr<SortDescriptionCollection> m_pSort;
};

}; // namespace AVUI
