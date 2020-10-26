#include <AVUICommon.h>
#include <AVUIIndexedEnumerable.h>
#include <AVUIINotifyCollectionChanged.h>
#include <AVUIObjectCollection.h>
#include <AVUICollectionView.h>

REGISTER_ELEMENT_BEGIN(AVUI::IndexedEnumerable)

REGISTER_ELEMENT_END()

namespace AVUI {


IndexedEnumerable::FilteredEnumerator::FilteredEnumerator(IndexedEnumerable* pIndexedEnumerable, Object* pEnumerable, ObjectPredicate* pFilterCallback) :
                                            m_pIndexedEnumerable(pIndexedEnumerable), m_pEnumerable(pEnumerable), m_pFilterCallback(pFilterCallback)
{
    m_pEnumerator = interface_cast<IEnumerable>(m_pEnumerable)->GetEnumerator();
    m_pEnumeratorInterface = interface_cast<IEnumerator>(m_pEnumerator);
}

bool IndexedEnumerable::FilteredEnumerator::MoveNext()
{
    if(IsInvalid())
    {
        LibraryCriticalError();
    }
    if(m_pFilterCallback == NULL)
    {
        return m_pEnumeratorInterface->MoveNext();
    }
    bool hasMoved;
    while((hasMoved = m_pEnumeratorInterface->MoveNext()) && !m_pFilterCallback->Invoke(m_pEnumeratorInterface->get_Current()))
    {
    }
    return hasMoved;

}

bool IndexedEnumerable::FilteredEnumerator::IsInvalid()
{
    return m_pIndexedEnumerable->m_pEnumerable == NULL;
}

void IndexedEnumerable::FilteredEnumerator::Reset()
{
   if (m_pIndexedEnumerable->m_pEnumerable == NULL)
   {
       LibraryCriticalError();
   }

   m_pEnumerator = interface_cast<IEnumerable>(m_pEnumerable)->GetEnumerator();
   m_pEnumeratorInterface = interface_cast<IEnumerator>(m_pEnumerator);
}

ObjectPtr IndexedEnumerable::FilteredEnumerator::get_Current()
{
    return m_pEnumeratorInterface->get_Current();
}

IndexedEnumerable::IndexedEnumerable(Object* pCollection, ObjectPredicate* pFilterCallback)
{
    m_cachedIndex = -1;
    m_cachedVersion = -1;
    m_cachedCount = -1;
    m_pFilterCallback = pFilterCallback;

    SetCollection(pCollection);

    INotifyCollectionChanged* pNotifyCollectionChanged = interface_cast<INotifyCollectionChanged>(pCollection);
    if(pNotifyCollectionChanged)
    {
        pNotifyCollectionChanged->add_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &IndexedEnumerable::OnCollectionChanged));
    }
}

IndexedEnumerable::~IndexedEnumerable()
{
    INotifyCollectionChanged* pNotifyCollectionChanged = interface_cast<INotifyCollectionChanged>(m_pEnumerableObject);
    if(pNotifyCollectionChanged)
    {
        pNotifyCollectionChanged->remove_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &IndexedEnumerable::OnCollectionChanged));
    }
}

void IndexedEnumerable::OnCollectionChanged(Object* pSender, CollectionChangedEventArgs* pArgs)
{
    InvalidateEnumerator();
}

void IndexedEnumerable::InvalidateEnumerator()
{
   m_enumeratorVersion++;
   m_pEnumerator = NULL;
   m_pEnumeratorObject = NULL;
   ClearAllCaches();
}

void IndexedEnumerable::ClearAllCaches()
{
    m_pCachedItem = NULL;
    m_cachedIndex = -1;
    m_cachedCount = -1;
}

void IndexedEnumerable::SetCollection(Object* pCollection)
{
    if(pCollection == NULL)
    {
        LibraryCriticalError();
    }

    m_pEnumerable = interface_cast<IEnumerable>(pCollection);
    if(m_pEnumerable != NULL)
    {
        m_pEnumerableObject = pCollection;
    }
    else
    {
        m_pEnumerableObject = NULL;
    }

    m_pCollection = interface_cast<ICollection>(pCollection);
    if(m_pCollection != NULL)
    {
        m_pCollectionObject = pCollection;
    }
    else
    {
        m_pCollectionObject = NULL;
    }

    m_pCollectionView = object_cast<CollectionView>(pCollection);
}


void IndexedEnumerable::Invalidate()
{
    ClearAllCaches();

    INotifyCollectionChanged* pNotifyCollectionChanged = interface_cast<INotifyCollectionChanged>(m_pEnumerableObject);
    if(pNotifyCollectionChanged)
    {
        pNotifyCollectionChanged->remove_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &IndexedEnumerable::OnCollectionChanged));
    }
    m_pEnumerable = NULL;
    m_pEnumerableObject = NULL;

    m_pEnumerator = NULL;
    m_pEnumeratorObject = NULL;

    m_pChangeTracker = NULL;
    m_pChangeTrackerObject = NULL;

    m_pCollection = NULL;
    m_pCollectionObject = NULL;

    m_pFilterCallback = NULL;
}

ObjectPtr IndexedEnumerable::operator[](int index)
{
    ObjectPtr pRetObject;
    if(GetNativeItemAt(index, &pRetObject))
    {
        return pRetObject;
    }
    if (index < 0)
    {
        LibraryCriticalError();
    }
    int num = index - m_cachedIndex;
    if (num < 0)
    {
        UseNewEnumerator();
        num = index + 1;
    }
    if (EnsureCacheCurrent())
    {
        if (index == m_cachedIndex)
        {
            return m_pCachedItem;
        }
    }
    else
    {
        num = index + 1;
    }

    while ((num > 0) && m_pEnumerator->MoveNext())
    {
        num--;
    }
    if (num != 0)
    {
        // Out of range
        LibraryCriticalError();
    }

    CacheCurrentItem(index, m_pEnumerator->get_Current());
    return m_pCachedItem;
}

bool IndexedEnumerable::GetNativeItemAt(int index, ObjectPtr* ppValue)
{
    *ppValue = NULL;

    if(m_pCollection != NULL)
    {
        *ppValue = m_pCollection->GetItemAt(index);
        return true;
    }

    if(m_pCollectionView != NULL)
    {
        *ppValue = m_pCollectionView->GetItemAt(index);
        return true;
    }

    return false;
}

void IndexedEnumerable::UseNewEnumerator()
{
    m_enumeratorVersion++;

    m_pChangeTrackerObject = m_pEnumerable->GetEnumerator();
    m_pChangeTracker = interface_cast<IEnumerator>(m_pChangeTrackerObject);

    m_pEnumeratorObject = GetEnumerator();
    m_pEnumerator = interface_cast<IEnumerator>(m_pEnumeratorObject);
    m_cachedIndex = -1;
    m_pCachedItem = NULL;
}
 
void IndexedEnumerable::CacheCurrentItem(int index, Object* pItem)
{
    m_cachedIndex = index;
    m_pCachedItem = pItem;
    m_cachedVersion = m_enumeratorVersion;
}

bool IndexedEnumerable::EnsureCacheCurrent()
{
    int num = EnsureEnumerator();
    if (num != m_cachedVersion)
    {
        ClearAllCaches();
        m_cachedVersion = num;
    }
    return ((num == m_cachedVersion) && (m_cachedIndex >= 0));
}

int IndexedEnumerable::EnsureEnumerator()
{
    if (m_pEnumerator == NULL || m_pChangeTracker->IsInvalid())
    {
        UseNewEnumerator();
    }
    else
    {
        m_pChangeTracker->MoveNext();
    }

    return m_enumeratorVersion;
}

int IndexedEnumerable::get_Count()
{
    int num = 0;
    if (!GetNativeCount(&num))
    {
        if (m_cachedCount >= 0)
        {
            return m_cachedCount;
        }
        num = 0;

        ObjectPtr pEnumeratorObject = GetEnumerator();
        IEnumerator* pEnumerator = interface_cast<IEnumerator>(pEnumeratorObject);
        while (pEnumerator->MoveNext())
        {
            num++;
        }
        m_cachedCount = num;
        m_cachedIsEmpty = Nullable<bool>(m_cachedCount == 0);
    }
    return num;
}


bool IndexedEnumerable::GetNativeCount(int* pValue)
{
    *pValue = -1;
    if (m_pCollection != NULL)
    {
        *pValue = m_pCollection->get_Count();
        return true;
    }
    if (m_pCollectionView != NULL)
    {
        *pValue = m_pCollectionView->get_Count();
        return true;
    }
    return false;
}

bool IndexedEnumerable::GetNativeIndexOf(Object* pItem, int* pValue)
{
    *pValue = -1;
    if (m_pCollection != NULL && m_pFilterCallback == NULL)
    {
        *pValue = m_pCollection->IndexOf(pItem);
        return true;
    }
    if (m_pCollectionView != NULL)
    {
        *pValue = m_pCollectionView->IndexOf(pItem);
        return true;
    }
    return false;
}

int IndexedEnumerable::IndexOf(Object* pItem)
{
    int num;
    if (!GetNativeIndexOf(pItem, &num))
    {
        if (EnsureCacheCurrent() && (pItem == m_pCachedItem))
        {
            return m_cachedIndex;
        }
        num = -1;
        if (m_cachedIndex >= 0)
        {
            UseNewEnumerator();
        }
        for (int i = 0; m_pEnumerator->MoveNext(); i++)
        {
            if (Object::Equals(m_pEnumerator->get_Current(), pItem))
            {
                num = i;
                break;
            }
        }
        if (num >= 0)
        {
            CacheCurrentItem(num, m_pEnumerator->get_Current());
            return num;
        }
        ClearAllCaches();
        m_pEnumerator = NULL;
        m_pEnumeratorObject = NULL;
    }
    return num;
}

 
bool IndexedEnumerable::get_IsEmpty()
{
    bool isEmpty;
    if (GetNativeIsEmpty(&isEmpty))
    {
        return isEmpty;
    }
    if (!m_cachedIsEmpty.get_HasValue())
    {
        ObjectPtr pEnumeratorObject = GetEnumerator();
        IEnumerator* pEnumerator = interface_cast<IEnumerator>(pEnumeratorObject);
        m_cachedIsEmpty = Nullable<bool>(!pEnumerator->MoveNext());
        if (m_cachedIsEmpty.get_Value())
        {
            m_cachedCount = 0;
        }
    }
    return m_cachedIsEmpty.get_Value();
}
 
bool IndexedEnumerable::GetNativeIsEmpty(bool* pIsEmpty)
{
    *pIsEmpty = true;
    if (m_pCollection != NULL)
    {
        *pIsEmpty = m_pCollection->get_Count() == 0;
        return true;
    }
    if (m_pCollectionView != NULL)
    {
        *pIsEmpty = m_pCollectionView->get_IsEmpty();
        return true;
    }
    return false;
}

ObjectPtr IndexedEnumerable::GetEnumerator()
{
    TRefCountedPtr<FilteredEnumerator> pEnumerator = object_allocate<FilteredEnumerator>(this, m_pEnumerableObject, m_pFilterCallback);

    return (Object*) pEnumerator;
}


 
 
 

 
 

 



}; // namespace AVUI
