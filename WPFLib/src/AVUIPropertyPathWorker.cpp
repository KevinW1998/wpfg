#include <AVUICommon.h>
#include <AVUIPropertyPathWorker.h>
#include <AVUIBindingExpression.h>
#include <AVUIPropertyPath.h>
#include <AVUIClrBindingWorker.h>
#include <AVUICollectionView.h>
#include <AVUICollectionViewSource.h>
#include <AVUIHelper.h>
#include <AVUIObjectCollection.h>

REGISTER_ELEMENT_BEGIN(AVUI::PropertyPathWorker)

REGISTER_ELEMENT_END()


namespace AVUI {

PropertyPathWorker::PropertyPathWorker(PropertyPath* pPath)
{
    Initialize(pPath, DataBindEngine::get_CurrentDataBindEngine());
}

PropertyPathWorker::~PropertyPathWorker()
{
    for(int i = 0; i < m_arySVS.get_Count(); i++)
    {
        Object* pReference = BindingExpressionBase::GetReference(m_arySVS[i].Item);

        INotifyPropertyChanged* pSource = interface_cast<INotifyPropertyChanged>(pReference);
        if (pSource != NULL)
        {
            pSource->remove_PropertyChanged(PropertyChangedEventHandler::Create(this, &PropertyPathWorker::OnItemPropertyChanged));
        }

    }

    if(m_pContextHelper != NULL)
    {
        m_pContextHelper->SetOwner(NULL);
    }
}

void PropertyPathWorker::Initialize(PropertyPath* pPath, DataBindEngine* pEngine)
{
    m_isDynamic = false;
    m_pParent = pPath;
    m_arySVS.AddEmptyItems(pPath->get_Length());

    m_pEngine = pEngine;
    for (int i = m_arySVS.get_Count() - 1; i >= 0; i--)
    {
        m_arySVS[i].Item = BindingExpressionBase::CreateReference(BindingExpression::get_NullDataItem());
    }
}


void PropertyPathWorker::AttachToRootItem(Object* pRootItem)
{
    m_pRootItem = BindingExpressionBase::CreateReference(pRootItem);
    UpdateSourceValueState(-1, NULL);
}

void PropertyPathWorker::DetachFromRootItem()
{
    m_pRootItem = BindingExpression::get_NullDataItem();
    UpdateSourceValueState(-1, NULL);
    m_pRootItem = NULL;
}

DependencyObject* PropertyPathWorker::get_TreeContext()
{
    return object_cast<DependencyObject>(BindingExpressionBase::GetReference(m_pTreeContext));
}

void PropertyPathWorker::set_TreeContext(DependencyObject* pTreeContext)
{
    m_pTreeContext = BindingExpressionBase::CreateReference(pTreeContext);
}


void PropertyPathWorker::UpdateSourceValueState(int k, CollectionView* pCollectionView)
{
    return UpdateSourceValueState(k, pCollectionView, BindingExpression::get_NullDataItem(), false);
}

void PropertyPathWorker::UpdateSourceValueState(int k, CollectionView* pCollectionView, Object* pNewValue, bool isASubPropertyChange)
{
    TRefCountedPtr<DependencyObject> pObj2 = NULL;
    if (m_pHost != NULL)
    {
        pObj2 = m_pHost->CheckTarget();
        if ((m_pRootItem != BindingExpression::get_NullDataItem()) && pObj2 == NULL)
        {
            return;
        }
    }
    int num = k;
    TRefCountedPtr<Object> pParent = NULL;
    m_status = PropertyPathStatus::Active;
    m_dependencySourcesChanged = false;
    if (pCollectionView != NULL)
    {
        ReplaceItem(k, pCollectionView->get_CurrentItem(), get_NoParent());
    }
    k++;
    while (k < m_arySVS.get_Count())
    {
        isASubPropertyChange = false;
        CollectionView* pOldCollectionView = m_arySVS[k].CollectionViewValue;
        pParent = (pNewValue == BindingExpression::get_NullDataItem()) ? (Object*) RawValue(k - 1) : pNewValue;
        pNewValue = BindingExpression::get_NullDataItem();
        if (pParent == get_AsyncRequestPending())
        {
            m_status = PropertyPathStatus::AsyncRequestPending;
            break;
        }
        ReplaceItem(k, BindingExpression::get_NullDataItem(), pParent);
        CollectionView* pNewCollectionView = m_arySVS[k].CollectionViewValue;
        if ((pOldCollectionView != pNewCollectionView) && (m_pHost != NULL))
        {
            m_pHost->ReplaceCurrentItem(pOldCollectionView, pNewCollectionView);
        }
        k++;
    }
    if (m_pHost != NULL)
    {
        m_pHost->NewValueAvailable(m_dependencySourcesChanged, num < 0, isASubPropertyChange);
    }
}

void PropertyPathWorker::GetInfo(int k, Object* pItem, SourceValueState* pSVS)
{
    TRefCountedPtr<Object> pReference = BindingExpressionBase::GetReference(m_arySVS[k].Item);
    const Type* pType = (pReference != NULL) ? &(pReference->get_Type()) : NULL;
    const Type* pNewType = pItem != NULL ? &(pItem->get_Type()) : NULL;

    if (pNewType == pType && pReference != BindingExpression::get_NullDataItem())
    {
        *pSVS = m_arySVS[k];
        pSVS->Item = BindingExpressionBase::CreateReference(pItem);
        return;
    }

    const Type* pPropertyType = NULL;

    if (pNewType == NULL && GetSourceValueInfo(k)->Type != SourceValueType::Direct)
    {
        pSVS->Info = NULL;
        pSVS->TypeValue = NULL;
        pSVS->Item = BindingExpressionBase::CreateReference(pItem);
        return;
    }

    TRefCountedPtr<Object> pAccessor;
    switch (GetSourceValueInfo(k)->Type)
    {
        case SourceValueType::Property:
            pAccessor = m_pParent->ResolvePropertyName(k, pItem, pNewType, get_TreeContext());

            if(pAccessor != NULL)
            {
                if(pAccessor->IsType(GET_TYPE(DependencyProperty)))
                {
                    if (get_IsDynamic())
                    {
                        m_dependencySourcesChanged = true;
                    }
                }

                pPropertyType = &(((const Property*)(Object*)pAccessor)->get_PropertyType());
            }

            break;

        case SourceValueType::Indexer:
        {
            ICollection* pCollection = interface_cast<ICollection>(pItem);

            if(pCollection != NULL)
            {
                pAccessor = BindingExpressionBase::CreateReference(pItem);
                pPropertyType = &(GET_TYPE(Object));
                pSVS->Args = m_pParent->ResolveIndexerParams(k, get_TreeContext());
            }
        }


        default:
            break;
    }

    pSVS->Info = pAccessor;
    pSVS->TypeValue = pPropertyType;
    pSVS->Item = BindingExpressionBase::CreateReference(pItem);
}


Object* PropertyPathWorker::GetAccessor(int level)
{
    return m_arySVS[level].Info;
}

Object* PropertyPathWorker::GetItem(int level)
{
    return BindingExpressionBase::GetReference(m_arySVS[level].Item);
}

SourceValueInfo* PropertyPathWorker::GetSourceValueInfo(int level)
{
    return m_pParent->GetSourceValueInfo(level);
}
 
const Type* PropertyPathWorker::GetType(int level)
{
    return m_arySVS[level].TypeValue;
}

// Indexers are definitely broken! Figure out how this has to work.
TRefCountedPtr<Object> PropertyPathWorker::GetValue(Object* pItem, int level)
{
    const DependencyProperty* pDP;
    const Property* pProperty;

    Object* pUnsetValue = DependencyProperty::get_UnsetValue();

    SetPropertyInfo(m_arySVS[level].Info, &pProperty, &pDP);

    switch (GetSourceValueInfo(level)->Type)
    {
    case SourceValueType::Property:
        if (pProperty == NULL)
        {
            if (pDP == NULL)
            {
                return pUnsetValue;
            }
            DependencyObject* pDO = (DependencyObject*) pItem;
            if (level != (get_Length() - 1) || m_pHost == NULL || m_pHost->get_TransfersDefaultValue())
            {
                return pDO->GetValue(*pDP);
            }
            if (!Helper::HasDefaultValue(*pDP, pDO, object_cast<UIElement>(pDO)))
            {
                return pDO->GetValue(*pDP);
            }
            return BindingExpression::get_IgnoreDefaultValue();
        }
        return pProperty->GetValue(pItem);

    case SourceValueType::Indexer:
        {   
            ICollection* pCollection = interface_cast<ICollection>(pItem);
            if(pCollection != NULL)
            {
                return pCollection->GetItemAt(m_arySVS[level].Args);
            }
            break;
            //return info.GetValue(item, BindingFlags.GetProperty, null, this._arySVS[level].args, CultureInfo.InvariantCulture);
        }

    case SourceValueType::Direct:
        return pItem;
    }
    return pUnsetValue;
}

int PropertyPathWorker::LevelForPropertyChange(Object* pItem, const Property* pProperty)
{
    for (int i = 0; i < m_arySVS.get_Count(); i++)
    {
        if (BindingExpressionBase::GetReference(m_arySVS[i].Item) == pItem)
        {
            if(pProperty == (Object*) m_arySVS[i].Info)
            {
                return i;
            }
        }
    }
    return -1;
}

 


void PropertyPathWorker::OnCurrentChanged(CollectionView* pCollectionView)
{
    for (int i = 0; i < get_Length(); i++)
    {
        if (m_arySVS[i].CollectionViewValue == pCollectionView)
        {
            m_pHost->CancelPendingTasks();
            UpdateSourceValueState(i, pCollectionView);
            return;
        }
    }
}


int PropertyPathWorker::get_Length()
{
    return m_pParent->get_Length();
}

void PropertyPathWorker::OnDependencyPropertyChanged(DependencyObject* pDO, const DependencyProperty* pDP, bool isASubPropertyChange)
{
    for (int i = 0; i < m_arySVS.get_Count(); i++)
    {
        if (((Object*)m_arySVS[i].Info) == pDP && BindingExpressionBase::GetReference(m_arySVS[i].Item) == pDO)
        {
            UpdateSourceValueState(i, NULL, BindingExpression::get_NullDataItem(), isASubPropertyChange);
            return;
        }
    }
}

void PropertyPathWorker::OnNewValue(int level, Object* pValue)
{
    m_status = PropertyPathStatus::Active;
    if (level < (get_Length() - 1))
    {
        UpdateSourceValueState(level, NULL, pValue, false);
    }
}

TRefCountedPtr<Object> PropertyPathWorker::RawValue()
{
    TRefCountedPtr<Object> pRawValue = RawValue(get_Length() - 1);
    if (pRawValue == get_AsyncRequestPending())
    {
        pRawValue = DependencyProperty::get_UnsetValue();
    }
    return pRawValue;
}

TRefCountedPtr<Object> PropertyPathWorker::RawValue(int k)
{
    if (k < 0)
    {
        return BindingExpressionBase::GetReference(m_pRootItem);
    }
    if (k < m_arySVS.get_Count())
    {
        Object* pReference = BindingExpressionBase::GetReference(m_arySVS[k].Item);
        Object* pInfo = m_arySVS[k].Info;
        if (pReference != BindingExpression::get_NullDataItem() && pInfo != NULL && (pReference != NULL || pInfo == DependencyProperty::get_UnsetValue()))
        {

            /* TBD -- Async
            object unsetValue = DependencyProperty::get_UnsetValue();
            if ((!(info is DependencyProperty) && (this.SVI[k].type != SourceValueType.Direct)) && ((this._host != null) && this._host.AsyncGet(reference, k)))
            {
                this._status = PropertyPathStatus.AsyncRequestPending;
                return AsyncRequestPending;
            }
            */

            return GetValue(pReference, k);
        }
    }
    return DependencyProperty::get_UnsetValue();
}
 

void PropertyPathWorker::RefreshValue()
{
    for (int i = 1; i < m_arySVS.get_Count(); i++)
    {
        if (!Object::Equals(BindingExpressionBase::GetReference(m_arySVS[i].Item), RawValue(i - 1)))
        {
            UpdateSourceValueState(i - 1, NULL);
            return;
        }
    }
    UpdateSourceValueState(get_Length() - 1, NULL);
}
 
void PropertyPathWorker::ReplaceItem(int k, Object* pNewO, Object* pParent)
{
    SourceValueState svs;
    Object* pReference = BindingExpressionBase::GetReference(m_arySVS[k].Item);
    if (get_IsDynamic() && (GetSourceValueInfo(k)->Type != SourceValueType::Direct))
    {        
        INotifyPropertyChanged* pSource = interface_cast<INotifyPropertyChanged>(pReference);
        if (pSource != NULL)
        {
            pSource->remove_PropertyChanged(PropertyChangedEventHandler::Create(this, &PropertyPathWorker::OnItemPropertyChanged));
        }

        if (m_arySVS[k].Info != NULL && m_arySVS[k].Info->IsType(GET_TYPE(DependencyProperty)))
        {
            m_dependencySourcesChanged = true;
        }
    }
    if (pNewO == NULL || pParent == DependencyProperty::get_UnsetValue() || pParent == BindingExpression::get_NullDataItem())
    {
        m_arySVS[k].Item = BindingExpressionBase::CreateReference(pNewO);
        if (pParent == DependencyProperty::get_UnsetValue() || pParent == BindingExpression::get_NullDataItem())
        {
            m_arySVS[k].CollectionViewValue = NULL;
        }
    }
    else
    {
        if (pNewO != BindingExpression::get_NullDataItem())
        {
            GetInfo(k, pNewO, &svs);
            svs.CollectionViewValue = m_arySVS[k].CollectionViewValue;
        }
        else
        {
            DrillIn::Enum drillIn = GetSourceValueInfo(k)->DrillIn;
            TRefCountedPtr<CollectionView> pItem = NULL;
            if (drillIn != DrillIn::Always)
            {
                GetInfo(k, pParent, &svs);
            }
            if (svs.Info == NULL)
            {
                pItem = CollectionViewSource::GetDefaultCollectionView(pParent, get_TreeContext());
                if (pItem != NULL && drillIn != DrillIn::Always && pItem != pParent)
                {
                    GetInfo(k, pItem, &svs);
                }
            }
            if (svs.Info == NULL && drillIn != DrillIn::Never && pItem != NULL)
            {
                pNewO = pItem->get_CurrentItem();
                if (pNewO != NULL)
                {
                    GetInfo(k, pNewO, &svs);
                    svs.CollectionViewValue = pItem;
                }
                else
                {
                    svs = m_arySVS[k];
                    svs.CollectionViewValue = pItem;
                    svs.Item = BindingExpressionBase::CreateReference(BindingExpression::get_NullDataItem());
                    if (svs.Info == NULL)
                    {
                        svs.Info = DependencyProperty::get_UnsetValue();
                    }
                }
            }
        }
        if (svs.Info == NULL)
        {
            svs.Item = BindingExpressionBase::CreateReference(BindingExpression::get_NullDataItem());
            m_arySVS[k] = svs;
            m_status = PropertyPathStatus::PathError;
        }
        else
        {
            m_arySVS[k] = svs;
            TRefCountedPtr<Object> pNewORef = BindingExpressionBase::GetReference(svs.Item);

            pNewO = pNewORef;
            if (get_IsDynamic() && GetSourceValueInfo(k)->Type != SourceValueType::Direct)
            {
                INotifyPropertyChanged* pChanged = interface_cast<INotifyPropertyChanged>(pNewO);

                if (pChanged != NULL)
                {
                    pChanged->add_PropertyChanged(PropertyChangedEventHandler::Create(this, &PropertyPathWorker::OnItemPropertyChanged));
                }
            }
            if (m_pHost != NULL && k == (get_Length() - 1))
            {
                m_pHost->SetupDefaultValueConverter(svs.TypeValue);
                if (m_pHost->get_IsReflective())
                {
                    CheckReadOnly(pNewO, svs.Info);
                }
            }
        }
    }
}

TRefCountedPtr<Object> PropertyPathWorker::SetContext(Object* pRootItem)
{
    if (m_pContextHelper == NULL)
    {
        m_pContextHelper = object_allocate<ContextHelper>(this);
    }
    m_pContextHelper->SetContext(pRootItem);
    return (Object*) m_pContextHelper;
}

void PropertyPathWorker::SetValue(Object* pItem, Object* pValue)
{
    const Property* pProperty;
    const DependencyProperty* pDP;

    int index = m_arySVS.get_Count() - 1;

    SetPropertyInfo(m_arySVS[index].Info, &pProperty, &pDP);

    switch (GetSourceValueInfo(index)->Type)
    {
    case SourceValueType::Property:
        if (pProperty != NULL)
        {
            pProperty->SetValue(pItem, pValue);
        }
        else if (pDP != NULL)
        {
            ((DependencyObject*) pItem)->SetValue(*pDP, pValue);
        }
        return;

    case SourceValueType::Indexer:

//        info.SetValue(pItem, value, BindingFlags.SetProperty, null, this._arySVS[index].args, CultureInfo.InvariantCulture);
        return;
    }
}

bool PropertyPathWorker::ShouldConvertIndexerToProperty(Object* pItem, const Type* pNewType, String* pName)
{
    /*
    if (HashHelper.IsDataRowView(item, newType))
    {
        return (TypeDescriptor.GetProperties(item)[name] != null);
    }
    */
    return false;

}

bool PropertyPathWorker::UsesDependencyProperty(DependencyObject* pDO, const DependencyProperty* pDP)
{
    for (int i = 0; i < m_arySVS.get_Count(); i++)
    {
        if(((Object*)m_arySVS[i].Info) == pDP && BindingExpressionBase::GetReference(m_arySVS[i].Item) == pDO)
        {
            return true;
        }
    }
    return false;
}

void PropertyPathWorker::SetPropertyInfo(Object* pProp, const Property** ppProperty, const DependencyProperty** ppDP)
{
    *ppProperty = NULL;
    *ppDP = NULL;

    *ppDP = object_cast<DependencyProperty>(pProp);
    if(*ppDP == NULL)
    {
        *ppProperty = object_cast<Property>(pProp);
    }
}

void PropertyPathWorker::CheckReadOnly(Object* pItem, Object* pInfo)
{
}

void PropertyPathWorker::OnItemPropertyChanged(Object* pSender, PropertyChangedEventArgs* pArgs)
{
    m_pHost->OnSourcePropertyChanged(pSender, &(pArgs->get_Property()));

// 
}


}; // namespace AVUI

