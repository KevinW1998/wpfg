#include <AVUICommon.h>
#include <AVUIInnerItemCollectionView.h>
#include <AVUIItemCollection.h>
#include <AVUIUIElement.h>
#include <AVUISortDescription.h>
#include <AVUIPropertyPath.h>
#include <AVUIStringUtil.h>

REGISTER_ELEMENT(AVUI::InnerItemCollectionView);

REGISTER_VALUE_TYPE_BEGIN(AVUI::SortDescription)
REGISTER_VALUE_TYPE_END()

REGISTER_INTERFACE_TYPE(IComparable);

namespace AVUI {

void InnerItemCollectionView::_MoveCurrentToPosition(int position)
{
    if (position < 0)
    {
        CollectionView::SetCurrent(NULL, -1);
    }
    else if (position >= get_ViewCount())
    {
        CollectionView::SetCurrent(NULL, get_ViewCount());
    }
    else
    {
        CollectionView::SetCurrent((*m_pViewList)[position], position);
    }
}

void InnerItemCollectionView::_RemoveAt(int index, int indexR, Object* pItem)
{
    if (index >= 0)
    {
        m_pViewList->RemoveAt(index);
    }
    if (indexR >= 0)
    {
        m_pRawList->RemoveAt(indexR);
    }
    ClearModelParent(pItem);
    if (index >= 0)
    {
        AdjustCurrencyForRemove(index);
        CollectionChangedEventArgs args(NotifyCollectionChangedAction::Remove, pItem, index);
        OnCollectionChanged(&args);
        if (m_currentElementWasRemoved)
        {
            MoveCurrencyOffDeletedElement();
        }
    }
}

void InnerItemCollectionView::Move(int oldIndex, int newIndex)
{
    ObjectPtr pItem = GetItemAt(oldIndex);

    RemoveAt(oldIndex);
    Insert(newIndex, pItem);
}


void InnerItemCollectionView::AdjustCurrencyForRemove(int index)
{
    if (index >= 0)
    {
        if (index < get_CurrentPosition())
        {
            int newPosition = get_CurrentPosition() - 1;
            CollectionView::SetCurrent((*m_pViewList)[newPosition], newPosition);
        }
        else if (index == get_CurrentPosition())
        {
            m_currentElementWasRemoved = true;
        }
    }
}

void InnerItemCollectionView::MoveCurrencyOffDeletedElement()
{
    int num = get_ViewCount() - 1;
    int position = (get_CurrentPosition() < num) ? get_CurrentPosition() : num;
    m_currentElementWasRemoved = false;
    CollectionView::OnCurrentChanging();
    _MoveCurrentToPosition(position);
    OnCurrentChanged();
}

void InnerItemCollectionView::ClearModelParent(Object* pItem)
{
    DependencyObject* pDO = object_cast<DependencyObject>(pItem);
    if(pDO != NULL)
    {
        get_ModelParentFE()->RemoveLogicalChild(pItem);
    }
}
 
void InnerItemCollectionView::Add(Object* pItem)
{
    TRefCountedPtr<DependencyObject> pPristineDO = AssertPristineModelChild(pItem);

    m_pViewList->AddItem(pItem);

    int index = m_pViewList->get_Count() - 1;

    if (get_IsCachedMode())
    {
        m_pRawList->AddItem(pItem);
    }
    if (pPristineDO != NULL)
    {
        SetModelParent(pItem);
    }
    AdjustCurrencyForAdd(index);
    SetIsModified();

    CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Add, pItem, index);
    OnCollectionChanged(&eventArgs);
}

void InnerItemCollectionView::AdjustCurrencyForAdd(int index)
{
    if (index >= 0)
    {
        if (get_ViewCount() == 1)
        {
            CollectionView::SetCurrent(NULL, -1);
        }
        else if (index <= get_CurrentPosition())
        {
            int newPosition = get_CurrentPosition() + 1;
            if (newPosition < get_ViewCount())
            {
                CollectionView::SetCurrent((*m_pViewList)[newPosition], newPosition);
            }
            else
            {
                CollectionView::SetCurrent(NULL, get_ViewCount());
            }
        }
    }
}

DependencyObject* InnerItemCollectionView::AssertPristineModelChild(Object* pItem)
{
    UIElement* pUIElement = object_cast<UIElement>(pItem);

    if(pUIElement == NULL)
    {
        return NULL;
    }

    if(pUIElement->get_Parent() != NULL)
    {
        LibraryCriticalError();
        //pUIElement->set_LogicalParent(NULL);
        // this is normally an error -- deviation
    }
    return pUIElement;
}

 
void InnerItemCollectionView::SetModelParent(Object* pItem)
{
    DependencyObject* pDO = object_cast<DependencyObject>(pItem);

    if(pDO == NULL)
    {
        return;
    }

    if(get_ModelParentFE() != NULL)
    {
        get_ModelParentFE()->AddLogicalChild(pItem);
    }
}

UIElement* InnerItemCollectionView::get_ModelParentFE()
{
    return get_ItemCollection()->get_ModelParentFE();
}

void InnerItemCollectionView::Clear()
{
    for (int i = m_pRawList->get_Count() - 1; i >= 0; i--)
    {
        ClearModelParent((*m_pRawList)[i]);
    }
    m_pRawList->Clear();
    CollectionView::RefreshOrDefer();
}
 
bool InnerItemCollectionView::Contains(Object* pItem)
{
    return m_pViewList->Contains(pItem);
}

ObjectPtr InnerItemCollectionView::GetEnumerator()
{
    return m_pViewList->GetEnumerator();
}

ObjectPtr InnerItemCollectionView::GetItemAt(int index)
{
    return (*m_pViewList)[index];
}

void InnerItemCollectionView::SetItemAt(int index, Object* pValue)
{
    DependencyObject* pDO = AssertPristineModelChild(pValue);
    int currentPosition = get_CurrentPosition();

    ObjectPtr pOldValue = (*m_pViewList)[index];

    (*m_pViewList)[index] = pValue;

    int num = -1;
    if (get_IsCachedMode())
    {
        num = m_pRawList->IndexOf(pOldValue);
        (*m_pRawList)[num] = pValue;
    }
    bool flag = true;
    if (pDO != NULL)
    {
        SetModelParent(pValue);
        ClearModelParent(pOldValue);
    }

    CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Replace, pValue, pOldValue, index);

    OnCollectionChanged(&eventArgs);
    SetIsModified();
}


int InnerItemCollectionView::IndexOf(Object* pItem)
{
    return m_pViewList->IndexOf(pItem);
}

void InnerItemCollectionView::Insert(int index, Object* pItem)
{
    DependencyObject* pDO = AssertPristineModelChild(pItem);

    m_pViewList->Insert(index, pItem);
    if (get_IsCachedMode())
    {
        m_pRawList->AddItem(pItem);
    }

    if (pDO != NULL)
    {
        SetModelParent(pItem);
    }

    AdjustCurrencyForAdd(index);
    SetIsModified();

    CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Add, pItem, index);
    OnCollectionChanged(&eventArgs);
}
 
bool InnerItemCollectionView::MoveCurrentTo(Object *pItem)
{
    if (!Object::Equals(get_CurrentItem(), pItem) || ((pItem == NULL) && !get_IsCurrentInView()))
    {
        return MoveCurrentToPosition(IndexOf(pItem));
    }
    return get_IsCurrentInView();
}


bool InnerItemCollectionView::get_IsCurrentInView()
{
    if (0 <= get_CurrentPosition())
    {
        return (get_CurrentPosition() < get_ViewCount());
    }
    return false;
}
 
bool InnerItemCollectionView::MoveCurrentToPosition(int position)
{
    if (position < -1 || position > get_ViewCount())
    {
        LibraryCriticalError();
    }
    if ((position != get_CurrentPosition()) && CollectionView::OKToChangeCurrent())
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

void InnerItemCollectionView::OnPropertyChanged(const Property& property)
{
    PropertyChangedEventArgs eventArgs(property);
    CollectionView::OnPropertyChanged(&eventArgs);
}
 
void InnerItemCollectionView::RefreshOverride()
{
    bool isEmpty = get_IsEmpty();
    ObjectPtr pCurrentItem = get_CurrentItem();
    bool isCurrentAfterLast = get_IsCurrentAfterLast();
    bool isCurrentBeforeFirst = get_IsCurrentBeforeFirst();

    int currentPosition = get_CurrentPosition();
    CollectionView::OnCurrentChanging();

    if ((get_SortDescriptions()->get_Count() > 0) || (get_Filter() != NULL))
    {
        if (get_Filter() == NULL)
        {
            m_pViewList = object_allocate< ItemStructList<ObjectPtr> >((*m_pRawList));
        }
        else
        {
            m_pViewList = object_allocate< ItemStructList<ObjectPtr> >();
            for (int i = 0; i < m_pRawList->get_Count(); i++)
            {
                if (get_Filter()->Invoke((*m_pRawList)[i]))
                {
                    m_pViewList->AddItem((*m_pRawList)[i]);
                }
            }
        }
        if (m_pSort != NULL && m_pSort->get_Count() > 0 && get_ViewCount() > 0)
        {
            // Deviation, implement this here, as it simplifies sort scenarios
            SortItemStructList<ObjectPtr>(m_pViewList, object_allocate<SortFieldComparer>(m_pSort));
        }
    }
    else
    {
        m_pViewList = m_pRawList;
    }

    if (get_IsEmpty() || isCurrentBeforeFirst)
    {
        _MoveCurrentToPosition(-1);
    }
    else if (isCurrentAfterLast)
    {
        _MoveCurrentToPosition(get_ViewCount());
    }
    else if (pCurrentItem != NULL)
    {
        int position = m_pViewList->IndexOf(pCurrentItem);
        if (position < 0)
        {
            position = 0;
        }
        _MoveCurrentToPosition(position);
    }
    ClearIsModified();

    CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Reset);

    OnCollectionChanged(&eventArgs);
    OnCurrentChanged();

    if (get_IsCurrentAfterLast() != isCurrentAfterLast)
    {
        OnPropertyChanged(get_IsCurrentAfterLastProperty());
    }
    if (get_IsCurrentBeforeFirst() != isCurrentBeforeFirst)
    {
        OnPropertyChanged(get_IsCurrentBeforeFirstProperty());
    }
    if (get_CurrentPosition() != currentPosition)
    {
        OnPropertyChanged(get_CurrentPositionProperty());
    }
    if (get_CurrentItem() != pCurrentItem)
    {
        OnPropertyChanged(get_CurrentItemProperty());
    }
}
 
bool InnerItemCollectionView::Remove(Object* pItem)
{
    int index = m_pViewList->IndexOf(pItem);
    int indexR = -1;
    if (get_IsCachedMode())
    {
        indexR = m_pRawList->IndexOf(pItem);
    }
    _RemoveAt(index, indexR, pItem);

    return true;
}
 
void InnerItemCollectionView::RemoveAt(int index)
{
    if ((0 > index) || (index >= get_ViewCount()))
    {
        LibraryCriticalError();
    }
    ObjectPtr pObj = GetItemAt(index);
    int indexR = -1;
    if (get_IsCachedMode())
    {
        indexR = m_pRawList->IndexOf(pObj);
    }
    _RemoveAt(index, indexR, pObj);
}

SortDescriptionCollection* InnerItemCollectionView::get_SortDescriptions()
{
    if (m_pSort == NULL)
    {
        SetSortDescriptions(object_allocate<SortDescriptionCollection>());
    }
    return m_pSort;
}

void InnerItemCollectionView::SetSortDescriptions(SortDescriptionCollection* pDescriptions)
{
    if (m_pSort != NULL)
    {
        m_pSort->remove_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &InnerItemCollectionView::SortDescriptionsChanged));
    }
    m_pSort = pDescriptions;
    if (m_pSort != NULL)
    {
        if(m_pSort->get_Count() > 0)
        {
            LibraryCriticalError();
        }
        m_pSort->add_CollectionChanged(NotifyCollectionChangedEventHandler::Create(this, &InnerItemCollectionView::SortDescriptionsChanged));
    }
}

void InnerItemCollectionView::SortDescriptionsChanged(Object* pSender, CollectionChangedEventArgs* pArgs)
{
    RefreshOrDefer();
}

SortDescriptionCollection* SortDescriptionCollection::get_Empty()
{
    static EmptySortDescriptionCollection s_empty;

    return &s_empty;
}

void SortFieldComparer::CreatePropertyInfo(SortDescriptionCollection* pSortFields)
{
    for (int i = 0; i < pSortFields->get_Count(); i++)
    {
        TRefCountedPtr<PropertyPath> pPath;
        SortDescription* pDescription = &(*pSortFields)[i];
        if (String::IsNullOrEmpty(pDescription->get_PropertyName()))
        {
            pPath = NULL;
        }
        else
        {
            pPath = object_allocate<PropertyPath>(pDescription->get_PropertyName());
        }

        SortPropertyInfo info;
        
        info.m_pInfo = pPath;
        info.m_descending = pDescription->get_Direction() == ListSortDirection::Descending;

        m_fields.AddItem(info);
    }
}

int SortFieldComparer::Compare(Object* pO1, Object* pO2)
{
    int num = 0;
    for (int i = 0; i < m_fields.get_Count(); i++)
    {
        TRefCountedPtr<Object> pA = m_fields[i].GetValue(pO1);
        TRefCountedPtr<Object> pB = m_fields[i].GetValue(pO2);
        num = ComparerCompare(pA, pB);
//        num = m_comparer.Compare(pA, pB);
        if (m_fields[i].m_descending)
        {
            num = -num;
        }
        if (num != 0)
        {
            return num;
        }
    }
    return num;
}


template<class T>
static int CompareBuiltinType(Object* pA, Object* pB)
{
    T a = UnboxValue<T>(pA);
    T b = UnboxValue<T>(pB);

    if(a < b)
    {
        return 1;
    }
    else if(a == b)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

// This should not exist, we should have a more in-depth culture specific comparer class
int SortFieldComparer::ComparerCompare(Object* pA, Object* pB)
{
    if (pA == pB)
    {
        return 0;
    }
    if (pA == NULL)
    {
        return -1;
    }
    if (pB == NULL)
    {
        return 1;
    }

    if(pA->get_Type() != pB->get_Type())
    {
        // Fundamentally incomparable
        return 0;
    }

    if(is_type<float>(pA))
    {
        return CompareBuiltinType<float>(pA, pB);
    }
    else if(is_type<double>(pA))
    {
        return CompareBuiltinType<double>(pA, pB);
    }
    else if(is_type<unsigned int>(pA))
    {
        return CompareBuiltinType<unsigned int>(pA, pB);
    }
    else if(is_type<int>(pA))
    {
        return CompareBuiltinType<int>(pA, pB);
    }
    else if(is_type<unsigned short>(pA))
    {
        return CompareBuiltinType<unsigned short>(pA, pB);
    }
    else if(is_type<short>(pA))
    {
        return CompareBuiltinType<short>(pA, pB);
    }
    else if(is_type<String>(pA))
    {
        return StringUtil::Compare(((String*)pA)->get_Ptr(), ((String*)pB)->get_Ptr());
    }

    IComparable* pComparable = interface_cast<IComparable>(pA);
    if (pComparable == NULL)
    {
        LibraryCriticalErrorMessage("Argument must implement IComparable.");
    }
    return pComparable->CompareTo(pB);
}
 
TRefCountedPtr<Object> SortFieldComparer::SortPropertyInfo::GetValue(Object* pObject)
{
    if (m_pInfo == NULL)
    {
        return pObject;
    }

    TRefCountedPtr<Object> pCtx = m_pInfo->SetContext(pObject);

    return m_pInfo->GetValue();
}

void SortDescriptionCollection::OnItemRemoved(int index)
{
    CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Remove, NULL, index);
    invoke_CollectionChanged(this, &eventArgs);
}
void SortDescriptionCollection::OnItemAdded(int insertPosition, const SortDescription& item)
{
    CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Add, BoxValue(item), insertPosition);
    invoke_CollectionChanged(this, &eventArgs);
}
void SortDescriptionCollection::OnItemMoved(int oldIndex, int newIndex)
{
    CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Move, BoxValue((*this)[newIndex]), oldIndex, newIndex);
    invoke_CollectionChanged(this, &eventArgs);
}
void SortDescriptionCollection::OnCleared()
{
    CollectionChangedEventArgs eventArgs(NotifyCollectionChangedAction::Reset);
    invoke_CollectionChanged(this, &eventArgs);
}


}; // namespace AVUI

