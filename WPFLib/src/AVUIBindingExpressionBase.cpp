#include <AVUICommon.h>
#include <AVUIBindingExpressionBase.h>
#include <AVUIDependencyObject.h>
#include <AVUIObjectCollection.h>
#include <AVUIBindingBase.h>
#include <AVUIDispatcher.h>
#include <AVUIDataBindEngine.h>

REGISTER_ELEMENT(AVUI::BindingExpressionBase)
REGISTER_ELEMENT(AVUI::WeakReferenceObject)

namespace AVUI {

BindingExpressionBase::BindingExpressionBase(BindingBase* pBinding, BindingExpressionBase* pParent) : Expression(ExpressionMode::SupportsUnboundSources)
{
    m_pValue = get_DefaultValueObject();
    if (pBinding == NULL)
    {
        LibraryCriticalError();
    }
    m_pBinding = pBinding;
    m_pParentBindingExpression = pParent;
    m_flags = (PrivateFlags::Enum) pBinding->get_Flags();
    m_status = BindingStatus::Unattached;

    if (pParent != NULL)
    {
        if (pParent->IsType(GET_TYPE(MultiBindingExpression)))
        {
            ChangeFlag(PrivateFlags::iInMultiBindingExpression, true);
        }
    }
}



void BindingExpressionBase::Attach(DependencyObject* pTarget, const DependencyProperty* pDP)
{
    set_IsAttaching(true);
    AttachOverride(pTarget, pDP);
    set_IsAttaching(false);
}

void BindingExpressionBase::AttachOverride(DependencyObject* pTarget, const DependencyProperty* pDP)
{
    m_pTargetElement = pTarget;
    m_pTargetProperty = pDP;
    m_pEngine = DataBindEngine::get_CurrentDataBindEngine();
}

void BindingExpressionBase::ChangeFlag(PrivateFlags::Enum flag, bool value)
{
    if (value)
    {
        m_flags = (PrivateFlags::Enum) (m_flags | flag);
    }
    else
    {
        m_flags = (PrivateFlags::Enum) (m_flags & ~flag);
    }
}

void BindingExpressionBase::ChangeSources(ItemStructList<WeakDependencySource>* pNewSources)
{
    DependencyObject* pTargetElement = get_TargetElement();
    if (pTargetElement != NULL)
    {
        if (get_IsInBindingExpressionCollection())
        {
            get_ParentBindingExpressionBase()->ChangeSourcesForChild(this, pNewSources);
        }
        else
        {
            ChangeSources(pTargetElement, get_TargetProperty(), pNewSources);
        }
    }
    if(pNewSources != NULL)
    {
        m_sources = *(pNewSources);
    }
    else
    {
        m_sources.Clear();
    }
}

void BindingExpressionBase::ChangeSources(DependencyObject* pTarget, const DependencyProperty* pDP, ItemStructList<WeakDependencySource>* pNewSources)
{
    ItemStructList<TRefCountedPtr<DependencySource> > sourceArray;

    if (pNewSources != NULL)
    {
        int length = 0;
        for (int i = 0; i < pNewSources->get_Count(); i++)
        {
            DependencyObject* pDependencyObject = (*pNewSources)[i].get_DependencyObject();
            if (pDependencyObject != NULL)
            {
                sourceArray.AddItem(object_allocate<DependencySource>(pDependencyObject, (*pNewSources)[i].get_DependencyProperty()));
            }
        }

        Expression::ChangeSources(pTarget, pDP, &sourceArray);
    }
    else
    {
        Expression::ChangeSources(pTarget, pDP, NULL);
    }
}

void BindingExpressionBase::ChangeValue(Object* pNewValue, bool notify)
{
    Object* pOldValue = (m_pValue != get_DefaultValueObject()) ? (Object*) m_pValue : DependencyProperty::get_UnsetValue();

    m_pValue = pNewValue;

    if (notify)
    {
        BindingValueChangedEventArgs e(pOldValue, pNewValue);
        invoke_ValueChanged(this, &e);
    }
}

TRefCountedPtr<ItemStructList<WeakDependencySource> > BindingExpressionBase::CombineSources(int index, ICollection* pBindingExpressions, int count, ItemStructList<WeakDependencySource>* pNewSources)
{
    if (index == count)
    {
        count++;
    }

    TRefCountedPtr<ItemStructList<WeakDependencySource> > pCollection = object_allocate<ItemStructList<WeakDependencySource> >();

    for (int i = 0; i < count; i++)
    {
        BindingExpressionBase* pBase2 = object_cast<BindingExpressionBase>(pBindingExpressions->GetItemAt(i));
        ItemStructList<WeakDependencySource>* pSourceArray = (i == index) ? pNewSources : ((pBase2 != NULL) ? pBase2->get_WeakSources() : NULL);
        int num2 = (pSourceArray == NULL) ? 0 : pSourceArray->get_Count();

        for (int j = 0; j < num2; j++)
        {
            WeakDependencySource* pItem = &((*pSourceArray)[j]);
            for (int k = 0; k < pCollection->get_Count(); k++)
            {
                WeakDependencySource* pSource2 = &((*pCollection)[k]);
                if ((pItem->get_DependencyObject() == pSource2->get_DependencyObject()) && (pItem->get_DependencyProperty() == pSource2->get_DependencyProperty()))
                {
                    pItem = NULL;
                    break;
                }
            }
            if (pItem != NULL)
            {
                pCollection->AddItem(*pItem);
            }
        }
    }

    if(pCollection->get_Count() > 0)
    {
        return pCollection;
    }

    return NULL;
}

TRefCountedPtr<Object> BindingExpressionBase::ConvertFallbackValue(Object* pValue, const DependencyProperty* pDP, Object* pSender)
{
    if (pValue == DependencyProperty::get_UnsetValue() || pDP->IsValidValue(pValue))
    {
        return pValue;
    }
    TRefCountedPtr<Object> pDefaultValueObject = NULL;
    bool flag = false;

    const TypeConverter* pConverter = pDP->get_PropertyType().get_TypeConverter();
    if (pConverter != NULL && pConverter->CanConvertFrom(pValue->get_Type()))
    {
        pDefaultValueObject = pConverter->ConvertFrom(pValue);

        if (pDP->IsValidValue(pDefaultValueObject))
        {
            flag = true;
        }
    }
    if (!flag)
    {
        pDefaultValueObject = get_DefaultValueObject();
    }
    return pDefaultValueObject;
}

TRefCountedPtr<Expression> BindingExpressionBase::Copy(DependencyObject* pDO, const DependencyProperty& dp)
{
    return (Expression*) (get_ParentBindingBase()->CreateBindingExpression(pDO, &dp));
}

TRefCountedPtr<Object> BindingExpressionBase::CreateReference(Object* pItem)
{                       // TODO BindingListCollectionView
    if (pItem != NULL) // && !pItem->IsType(GET_TYPE(BindingListCollectionView)))
    {
        return (Object*) object_allocate<WeakReferenceObject>(pItem);
    }

    return pItem;
}


TRefCountedPtr<BindingExpressionBase> BindingExpressionBase::CreateUntargetedBindingExpression(DependencyObject* pDO, BindingBase* pBinding)
{
    return pBinding->CreateBindingExpression(pDO, get_NoTargetProperty());
}

void BindingExpressionBase::Detach()
{
    if ((m_status != BindingStatus::Detached) && !get_IsDetaching())
    {
        set_IsDetaching(true);
        DetachOverride();
        set_IsDetaching(false);
    }
}
 
void BindingExpressionBase::DetachOverride()
{
    m_pEngine = NULL;
    m_pTargetElement = NULL;
    m_pTargetProperty = NULL;
    SetStatus(BindingStatus::Detached);
}

void BindingExpressionBase::Dirty()
{
    if (!get_IsInTransfer())
    {
        set_NeedsUpdate(true);
        if (get_IsUpdateOnPropertyChanged())
        {
            Update(true);
        }
    }
}
 
void BindingExpressionBase::EndSourceUpdate()
{
    ChangeFlag((PrivateFlags::Enum)(PrivateFlags::iNeedUpdate | PrivateFlags::iInUpdate), false);
}

 
TRefCountedPtr<Object> BindingExpressionBase::GetReference(Object* pReference)
{
    if (pReference == NULL)
    {
        return NULL;
    }

    WeakReferenceObject* pWR = object_cast<WeakReferenceObject>(pReference);

    if(pWR != NULL)
    {
        return pWR->get_Data();
    }

    return pReference;
}


TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > BindingExpressionBase::GetSources()
{
    int num3 = m_sources.get_Count();
    if (num3 == 0)
    {
        return NULL;
    }

    TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > pArray = object_allocate<ItemStructList<TRefCountedPtr<DependencySource> > >();

    int length = 0;
    for (int i = 0; i < num3; i++)
    {
        DependencyObject* pDO = m_sources[i].get_DependencyObject();
        if (pDO != NULL)
        {
            pArray->AddItem(object_allocate<DependencySource>(pDO,  m_sources[i].get_DependencyProperty()));
        }
    }
    return pArray;
}

void BindingExpressionBase::OnAttach(DependencyObject* pDO, const DependencyProperty& dp)
{
    Attach(pDO, &dp);
}

void BindingExpressionBase::OnDetach(DependencyObject* pDO, const DependencyProperty& dp)
{
    Detach();
}

void BindingExpressionBase::ResolvePropertyDefaultSettings(BindingMode::Enum mode, UpdateSourceTrigger::Enum updateTrigger, DependencyPropertyMetadata* pMetaData)
{
    if (mode == BindingMode::Default)
    {
        BindingFlags::Enum oneWay = BindingFlags::OneWay;
        if (pMetaData != NULL && pMetaData->get_BindsTwoWayByDefault())
        {
            oneWay = BindingFlags::TwoWay;
        }
        ChangeFlag(PrivateFlags::iPropagationMask, false);
        ChangeFlag((PrivateFlags::Enum) oneWay, true);
    }
    if (updateTrigger == UpdateSourceTrigger::Default)
    {
        UpdateSourceTrigger::Enum updateSourceTrigger = get_IsInMultiBindingExpression() ? UpdateSourceTrigger::Explicit : ((pMetaData != NULL) ? pMetaData->get_DefaultUpdateSourceTrigger() : UpdateSourceTrigger::PropertyChanged);
        ChangeFlag(PrivateFlags::iUpdateMask, false);
        ChangeFlag((PrivateFlags::Enum) BindingBase::FlagsFrom(updateSourceTrigger), true);
    }
}

void BindingExpressionBase::SetStatus(BindingStatus::Enum status)
{
    m_status = status;
}

 
bool BindingExpressionBase::TestFlag(PrivateFlags::Enum flag)
{
    return ((m_flags & flag) != 0);
}

TRefCountedPtr<Object> BindingExpressionBase::UseFallbackValue()
{
    TRefCountedPtr<Object> pFallbackValue = get_FallbackValue();
    if (pFallbackValue == get_DefaultValueObject())
    {
        pFallbackValue = DependencyProperty::get_UnsetValue();
    }
    if (pFallbackValue != DependencyProperty::get_UnsetValue())
    {
        set_UsingFallbackValue(true);
        return pFallbackValue;
    }
    if (get_Status() == BindingStatus::Active)
    {
        SetStatus(BindingStatus::UpdateTargetError);
    }
    if (!get_IsInBindingExpressionCollection())
    {
        pFallbackValue = get_DefaultValue();
    }
    return pFallbackValue;
}


Object* BindingExpressionBase::get_DefaultValue()
{
    DependencyObject* pTargetElement = get_TargetElement();
    if (pTargetElement != NULL)
    {
        return get_TargetProperty()->GetMetadata(pTargetElement->get_Type())->get_DefaultValue();
    }
    return DependencyProperty::get_UnsetValue();
}

Dispatcher* BindingExpressionBase::get_Dispatcher()
{
    return get_Engine()->get_Dispatcher();
}

TRefCountedPtr<Object> BindingExpressionBase::get_FallbackValue()
{
    return ConvertFallbackValue(get_ParentBindingBase()->get_FallbackValue(), get_TargetProperty(), this);
}

bool BindingExpressionBase::get_IsDynamic()
{
    if (!TestFlag(PrivateFlags::iSourceToTarget))
    {
        return false;
    }
    if (get_IsInMultiBindingExpression())
    {
        // TODO Multibinding
        // return get_ParentMultiBindingExpression()->get_IsDynamic();
    }
    return true;
}

bool BindingExpressionBase::get_IsReflective()
{
    if (!TestFlag(PrivateFlags::iTargetToSource))
    {
        return false;
    }
    if (get_IsInMultiBindingExpression())
    {
        // TODO Multibinding
        //return get_ParentMultiBindingExpression()->get_IsReflective();
    }
    return true;
}

DependencyObject* BindingExpressionBase::get_TargetElement()
{
    if (m_pTargetElement != NULL)
    {
        DependencyObject* pTarget = object_cast<DependencyObject>(m_pTargetElement);
        if (pTarget != NULL)
        {
            return pTarget;
        }
        m_pTargetElement = NULL;
        Detach();
    }
    return NULL;
}

Object* BindingExpressionBase::get_Value()
{
    if (m_pValue == get_DefaultValueObject())
    {
        ChangeValue(UseFallbackValue(), false);
    }
    return m_pValue;
}

void BindingExpressionBase::set_Value(Object* pNewValue)
{
    ChangeValue(pNewValue, true);
    Dirty();
}

}; // namespace AVUI

