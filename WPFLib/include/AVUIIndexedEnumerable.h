#pragma once

#include <AVUIObject.h>
#include <AVUIIEnumerable.h>
#include <AVUIIEnumerator.h>

namespace AVUI {

class CollectionView;
class ICollection;
class CollectionChangedEventArgs;

class IndexedEnumerable : public Object, public IEnumerable
{
    friend class FilteredEnumerator;
public:

    DECLARE_ELEMENT(IndexedEnumerable, Object);

    IndexedEnumerable(Object* pEnumerable, ObjectPredicate* pFilter = NULL);
    ~IndexedEnumerable();


    void Invalidate();

    ObjectPtr operator[](int idx);
    int get_Count();
    int IndexOf(Object* pItem);
    bool get_IsEmpty();
    ObjectPtr GetEnumerator();


    class FilteredEnumerator : public Object, public IEnumerator
    {
    public:
        FilteredEnumerator(IndexedEnumerable* pIndexedEnumerable, Object* pEnumerable, ObjectPredicate* pFilterCallback);
    
        bool MoveNext();
        void Reset();
        bool IsInvalid();
        ObjectPtr get_Current();
    private:
        TRefCountedPtr<Object> m_pEnumerable;
        TRefCountedPtr<Object> m_pEnumerator;
        IEnumerator* m_pEnumeratorInterface;
    
        TRefCountedPtr<ObjectPredicate> m_pFilterCallback;
        TRefCountedPtr<IndexedEnumerable> m_pIndexedEnumerable;
    };

private:
    void OnCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs);
    void InvalidateEnumerator();
    void ClearAllCaches();
    void SetCollection(Object* pCollection);
    bool GetNativeItemAt(int index, ObjectPtr* ppValue);
    void UseNewEnumerator();
    void CacheCurrentItem(int index, Object* pItem);
    bool EnsureCacheCurrent();
    int EnsureEnumerator();
    bool GetNativeCount(int* pValue);
    bool GetNativeIndexOf(Object* pItem, int* pValue);
    bool GetNativeIsEmpty(bool* pIsEmpty);

    int m_cachedCount;
    int m_cachedIndex;
    Nullable<bool> m_cachedIsEmpty;
    ObjectPtr m_pCachedItem;
    int m_cachedVersion;

    IEnumerator* m_pChangeTracker;
    ObjectPtr m_pChangeTrackerObject;

    ICollection* m_pCollection;
    ObjectPtr m_pCollectionObject;

    TRefCountedPtr<CollectionView> m_pCollectionView;

    IEnumerable* m_pEnumerable;
    ObjectPtr m_pEnumerableObject;

    IEnumerator* m_pEnumerator;
    ObjectPtr m_pEnumeratorObject;

    int m_enumeratorVersion;
    TRefCountedPtr<ObjectPredicate> m_pFilterCallback;
};

}; // namespace AVUI
