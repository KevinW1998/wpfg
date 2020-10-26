#include <AVUICommon.h>
#include <AVUIBindingExpression.h>
#include <AVUIBinding.h>
#include <AVUIBindingWorker.h>
#include <AVUIDynamicValueConverter.h>
#include <AVUIUIElement.h>
#include <AVUIHelper.h>
#include <AVUICollectionViewSource.h>
#include <AVUICollectionView.h>
#include <AVUIContentPresenter.h>
#include <AVUIDataBindEngine.h>
#include <AVUIPropertyPath.h>
#include <AVUIClrBindingWorker.h>
#include <AVUIObjectCollection.h>
#include <AVUIBindingOperations.h>

REGISTER_ELEMENT_BEGIN(AVUI::BindingExpression)

REGISTER_ELEMENT_END()

namespace AVUI {

BindingExpression::BindingExpression(Binding* pBinding, BindingExpressionBase* pOwner) : BindingExpressionBase(pBinding, pOwner)
{
    set_UseDefaultValueConverter(get_ParentBinding()->get_Converter() == NULL);
}

BindingExpression::~BindingExpression()
{
    if(get_Engine())
    {
        get_Engine()->CancelTasks(this);
    }
}

void BindingExpression::Activate()
{
    if (get_CanActivate())
    {
        if (m_pCtxElement == NULL)
        {
            if (get_Status() == BindingStatus::Inactive)
            {
                DependencyObject* pTargetElement = get_TargetElement();
                if (pTargetElement != NULL)
                {
                    if (get_UsingMentor())
                    {
                        pTargetElement = Helper::FindMentor(pTargetElement);
                        if (pTargetElement == NULL)
                        {
                            SetStatus(BindingStatus::PathError);
                            return;
                        }
                    }
                    Activate(get_ParentBinding()->get_SourceReference()->GetDataObject(pTargetElement, get_ResolveNamesInTemplate()));
                }
            }
        }
        else
        {
            DependencyObject* pContextElement = get_ContextElement();
            if (pContextElement == NULL)
            {
                SetStatus(BindingStatus::PathError);
            }
            else
            {
                Object* pObjA = pContextElement->GetValue(UIElement::get_DataContextProperty());
                if ((get_Status() == BindingStatus::Inactive) || !Object::Equals(pObjA, get_DataItem()))
                {
                    Activate(pObjA);
                }                 
            }
        }
    }

}

void BindingExpression::Activate(Object* pItem)
{
    TRefCountedPtr<DependencyObject> pTargetElement = get_TargetElement();
    if (pTargetElement != NULL)
    {
        Deactivate();

        CollectionViewSource* pSource = object_cast<CollectionViewSource>(pTargetElement);
        set_CollectionViewSource(pSource);
        if (pSource != NULL)
        {
            pItem = pSource->get_CollectionView();
            ChangeWorkerSources(NULL, 0);
        }
        else
        {
            pItem = DereferenceDataProvider(pItem);
        }
        m_pDataItem = BindingExpressionBase::CreateReference(pItem);
        if (get_Worker() == NULL)
        {
            CreateWorker();
        }
        SetStatus(BindingStatus::Active);
        get_Worker()->AttachDataItem();
        if (!get_IsOneWayToSource())
        {
            TransferValue();      
        }
        else
        {
            UpdateValue();
        }
    }
}

void BindingExpression::AttachOverride(DependencyObject* pTarget, const DependencyProperty* pDP)
{
    TRefCountedPtr<Object> pTargetRef = pTarget;
                                  
    BindingExpressionBase::AttachOverride(pTarget, pDP);

    if (get_ParentBinding()->get_SourceReference() == NULL || (get_ParentBinding()->get_SourceReference()->get_UsesMentor() && (Helper::FindMentor(pTarget) != pTarget)))
    {
        pTarget->add_InheritanceContextChanged(EventHandler::Create(this, &BindingExpression::OnTargetInheritanceContextChanged));
        set_UsingMentor(true);
    }
    if (get_IsUpdateOnLostFocus())
    {
        if(pTarget != NULL && pTarget->IsType(GET_TYPE(UIElement)))
        {
            ((UIElement*)pTarget)->add_LostFocus(RoutedEventHandler::Create(this, &BindingExpression::OnTargetLostFocus));
        }
    }                             
    AttachToContext(AttachAttempt::First);
    if (get_Status() == BindingStatus::Unattached)
    {
        get_Engine()->AddTask(this, TaskOps::AttachToContext);
    }
}

void BindingExpression::AttachToContext(AttachAttempt::Enum attempt)
{
    TRefCountedPtr<DependencyObject> pTargetElement = get_TargetElement();
    if (pTargetElement != NULL)
    {
        if (attempt == AttachAttempt::First)
        {
            ObjectRef* pSourceReference = get_ParentBinding()->get_SourceReference();
            if (pSourceReference != NULL && pSourceReference->TreeContextIsRequired(pTargetElement))
            {                     
                return;           
            }
        }
        bool flag = (attempt == AttachAttempt::Last);

        // Original conditional -- XMLNS not used
        //         if ((flag || !this.ParentBinding.TreeContextIsRequired) || ((targetElement.GetValue(XmlAttributeProperties.XmlnsDictionaryProperty) != null) && (targetElement.GetValue(XmlAttributeProperties.XmlNamespaceMapsProperty) != null)))

        if (flag || !get_ParentBinding()->get_TreeContextIsRequired())
        {
            DependencyObject* pD = !get_UsingMentor() ? (DependencyObject*)pTargetElement : Helper::FindMentor(pTargetElement);
            if (pD == NULL)
            {
                if (flag)
                {
                    SetStatus(BindingStatus::PathError);
                }
            }
            else
            {
                DependencyObject* pCurrent = NULL;
                bool flag2 = true;
                if (get_ParentBinding()->get_SourceReference() == NULL)
                {
                    CollectionViewSource* pSource;
                    pCurrent = pD;
                    if (
                         get_TargetProperty() == &UIElement::get_DataContextProperty() || 
                         (get_TargetProperty() == &ContentPresenter::get_ContentProperty() && pTargetElement->IsType(GET_TYPE(ContentPresenter))) || 
                        (get_UsingMentor() && ((pSource = object_cast<CollectionViewSource>(pTargetElement)) != NULL) && (pSource->get_PropertyForInheritanceContext() == &(UIElement::get_DataContextProperty()))))
                    {
                        pCurrent = UIElement::GetFrameworkParent(pCurrent);
                        flag2 = pCurrent != NULL;
                    }
                }
                else
                {
                    RelativeObjectRef* pRef3 = object_cast<RelativeObjectRef>(get_ParentBinding()->get_SourceReference());
                    if (pRef3 != NULL && pRef3->get_ReturnsDataContext())
                    {
                        TRefCountedPtr<Object> pObj5 = pRef3->GetObject(pD);
                        pCurrent = object_cast<DependencyObject>(pObj5);
                        flag2 = pObj5 != DependencyProperty::get_UnsetValue();
                    }
                }
                if (!flag2)
                {
                    if (flag)
                    {
                        SetStatus(BindingStatus::PathError);
                    }
                }
                else
                {
                    TRefCountedPtr<Object> pDataObject;
                    if (pCurrent != NULL)
                    {
                        pDataObject = pCurrent->GetValue(UIElement::get_DataContextProperty());
                        if (pDataObject == NULL && !flag && !HasLocalDataContext(pCurrent))
                        {
                            return;
                        }
                    }
                    else
                    {
                        ObjectRef* pRef4 = get_ParentBinding()->get_SourceReference();
                        if (pRef4 != NULL)
                        {
                            pDataObject = pRef4->GetDataObject(pD, get_ResolveNamesInTemplate());
                            if (pDataObject == DependencyProperty::get_UnsetValue())
                            {
                                if (flag)
                                {
                                    SetStatus(BindingStatus::PathError);
                                }
                                return;
                            }
                        }
                        else
                        {
                            pDataObject = NULL;
                        }
                    }
                    if (pCurrent != NULL)
                    {
                        m_pCtxElement = pCurrent;
                    }
                    ChangeWorkerSources(NULL, 0);
                    if (!get_UseDefaultValueConverter())
                    {
                        m_pValueConverter = get_ParentBinding()->get_Converter();
                        if (m_pValueConverter == NULL)
                        {
                            LibraryCriticalError();
                        }
                    }
                    SetStatus(BindingStatus::Inactive);
                    if (get_IsInPriorityBindingExpression())
                    {
                        // TODO -- PriorityBindingExpression
                        // get_ParentPriorityBindingExpression()->InvalidateChild(this);
                    }
                    else
                    {
                        Activate(pDataObject);
                    }
                }
            }
        }
    }
}

void BindingExpression::CancelPendingTasks()
{
    get_Engine()->CancelTasks(this);
}

void BindingExpression::ChangeWorkerSources(ItemStructList<WeakDependencySource>* pNewWorkerSources, int n)
{
    ItemStructList<WeakDependencySource> workerSources;

    int destinationIndex = 0;
    DependencyObject* pContextElement = get_ContextElement();
    CollectionViewSource* pCollectionViewSource = get_CollectionViewSource();

    if(pContextElement != NULL)
    {
        workerSources.AddItem(WeakDependencySource(pContextElement, &UIElement::get_DataContextProperty()));
    }

    if(pCollectionViewSource != NULL)
    {
        workerSources.AddItem(WeakDependencySource(pCollectionViewSource, &CollectionViewSource::get_ViewProperty()));
    }

    if(pNewWorkerSources != NULL)
    {
        for(int idx = 0; idx < pNewWorkerSources->get_Count(); idx++)
        {
            workerSources.AddItem((*pNewWorkerSources)[idx]);
        }
    }
    ChangeSources(&workerSources);
}

TRefCountedPtr<Object> BindingExpression::ChangingValue()
{
    return (Object*) object_allocate<ChangingValueHelper>(this);
}

TRefCountedPtr<Object> BindingExpression::ConvertBackHelper(IValueConverter* pConverter, Object* pValue, const Type* pSourceType, Object* pParameter)
{
    return pConverter->ConvertBack(pValue, pSourceType, pParameter);
}

TRefCountedPtr<Object> BindingExpression::ConvertHelper(IValueConverter* pConverter, Object* pValue, const Type* pTargetType, Object* pParameter)
{
    return pConverter->Convert(pValue, pTargetType, pParameter);
}

TRefCountedPtr<BindingExpression> BindingExpression::CreateBindingExpression(DependencyObject* pDO, const DependencyProperty* pDP, Binding* pBinding, BindingExpressionBase* pParent)
{
    DependencyPropertyMetadata* pMetadata = pDP->GetMetadata(pDO->get_Type());
    if ((pMetadata != NULL && !pMetadata->get_IsDataBindingAllowed()) || pDP->get_IsReadOnly())
    {
        LibraryCriticalError();
    }
    TRefCountedPtr<BindingExpression> pExpression = object_allocate<BindingExpression>(pBinding, pParent);
    pExpression->ResolvePropertyDefaultSettings(pBinding->get_Mode(), pBinding->get_UpdateSourceTrigger(), pMetadata);
    if (pExpression->get_IsReflective() && pBinding->get_XPath() == NULL && (pBinding->get_Path() == NULL || String::IsNullOrEmpty(pBinding->get_Path()->get_Path())))
    {
        LibraryCriticalError();
    }
    return pExpression;
}

void BindingExpression::CreateWorker()
{
    m_pBindingWorker = object_allocate<ClrBindingWorker>(this, get_Engine());
}

void BindingExpression::Deactivate()
{
    if (get_Status() != BindingStatus::Inactive)
    {
        CancelPendingTasks();
        if (get_Worker() != NULL)
        {
            get_Worker()->DetachDataItem();
        }
        ChangeValue(BindingExpressionBase::get_DefaultValueObject(), false);
        m_pDataItem = NULL;
        SetStatus(BindingStatus::Inactive);
    }
}

TRefCountedPtr<Object> BindingExpression::DereferenceDataProvider(Object* pItem)
{
    if (get_ParentBinding()->get_BindsDirectlyToSource())
    {
        return pItem;
    }

    return pItem;

    /* TODO -- DataSourceProvider
    DataSourceProvider provider = item as DataSourceProvider;
    if (provider != this._dataProvider)
    {
        if (this._dataProvider != null)
        {
            if (base.TraceLog != null)
            {
                base.TraceLog.Add("-OnDataChanged to {0}", new object[] { TraceLog.IdFor(this._dataProvider) });
            }
            DataChangedEventManager.RemoveListener(this._dataProvider, this);
        }
        this._dataProvider = provider;
        if (this._dataProvider != null)
        {
            if (base.TraceLog != null)
            {
                base.TraceLog.Add("+OnDataChanged to {0}", new object[] { TraceLog.IdFor(this._dataProvider) });
            }
            DataChangedEventManager.AddListener(this._dataProvider, this);
            this._dataProvider.InitialLoad();
        }
    }
    if (this._dataProvider == null)
    {
        return item;
    }
    return this._dataProvider.Data;
    */
}

void BindingExpression::DetachFromContext()
{
    /* TODO -- DataProvider
    if (m_pDataProvider != NULL)
    {
        DataChangedEventManager.RemoveListener(this._dataProvider, this);
    }
    */
    if (!get_UseDefaultValueConverter())
    {
        m_pValueConverter = NULL;
    }
    if (!get_IsInBindingExpressionCollection())
    {
        ChangeSources(NULL);
    }
    if (get_UsingMentor())
    {
        DependencyObject* pTargetElement = get_TargetElement();
        if (pTargetElement != NULL)
        {
            pTargetElement->remove_InheritanceContextChanged(EventHandler::Create(this, &BindingExpression::OnTargetInheritanceContextChanged));
        }
    }
    m_pCtxElement = NULL;
}

void BindingExpression::DetachOverride()
{
    Deactivate();
    DetachFromContext();
    if (get_IsUpdateOnLostFocus())
    {
        DependencyObject* pTargetElement = get_TargetElement();
        if (pTargetElement != NULL && pTargetElement->IsType(GET_TYPE(UIElement)))
        {
            ((UIElement*)pTargetElement)->remove_LostFocus(RoutedEventHandler::Create(this, &BindingExpression::OnTargetLostFocus));
        }

    }

    ChangeValue(DependencyProperty::get_UnsetValue(), false);
    BindingExpressionBase::DetachOverride();
}

TRefCountedPtr<Object> BindingExpression::GetDataSourceForDataContext(DependencyObject* pDO)
{
    TRefCountedPtr<BindingExpression> pBindingExpression = NULL;
    for (DependencyObject* pObj2 = pDO; pObj2 != NULL; pObj2 = UIElement::GetFrameworkParent(pObj2))
    {
        if (HasLocalDataContext(pObj2))
        {
            pBindingExpression = BindingOperations::GetBindingExpression(pObj2, &UIElement::get_DataContextProperty());
            break;
        }
    }
    if (pBindingExpression != NULL)
    {
        return pBindingExpression->get_DataSource();
    }
    return NULL;
}

ObjectPtr BindingExpression::GetValue(DependencyObject* pDO, const DependencyProperty& dp)
{
    return get_Value();
}

void BindingExpression::HandlePropertyInvalidation(DependencyObject* pDO, const DependencyPropertyChangedEventArgs* pArgs)
{
    const DependencyProperty* pDP = &(pArgs->DependencyPropertyChanged);
    if (pDP == &(UIElement::get_DataContextProperty()))
    {
        DependencyObject* pContextElement = get_ContextElement();
        if (pDO == pContextElement)
        {
            OnDataContextChanged(pContextElement);
        }
    }
    if (pDP == &(CollectionViewSource::get_ViewProperty()))
    {
        CollectionViewSource* pCollectionViewSource = get_CollectionViewSource();
        if (pDO == pCollectionViewSource)
        {
            Activate(pCollectionViewSource);
        }
    }
    if (get_Worker() != NULL)
    {
        get_Worker()->OnSourceInvalidation(pDO, pDP, pArgs->IsSubPropertyChange);
    }
}


Object* BindingExpression::HandlePropertyInvalidationOperation(Object* pObj)
{
    ICollection* pCollection = interface_cast<ICollection>(pObj);

    HandlePropertyInvalidation((DependencyObject*) (Object*) pCollection->GetItemAt(0), (DependencyPropertyChangedEventArgs*) (Object*) pCollection->GetItemAt(1));

    return NULL;
}

bool BindingExpression::HasLocalDataContext(DependencyObject* pDO)
{
    return (pDO->ReadLocalValue(UIElement::get_DataContextProperty()) != DependencyProperty::get_UnsetValue());
}

 
void BindingExpression::Invalidate(bool isASubPropertyChange)
{
    if (!get_IsAttaching())
    {
        DependencyObject* pTargetElement = get_TargetElement();
        if (pTargetElement != NULL)
        {
            if (get_IsInBindingExpressionCollection())
            {
                get_ParentBindingExpressionBase()->InvalidateChild(this);
            }
            else if (get_TargetProperty() != BindingExpressionBase::get_NoTargetProperty())
            {
                if (!isASubPropertyChange)
                {
                    pTargetElement->InvalidateProperty(*get_TargetProperty());
                }
                else
                {
                    pTargetElement->NotifySubPropertyChange(*get_TargetProperty());
                }
            }
        }
    }
}

 


void BindingExpression::OnTargetLostFocus(Object* pSender, RoutedEventArgs* pArgs)
{
    Update(true);
}

void BindingExpression::OnTargetInheritanceContextChanged(Object* pSender, EventArgs* pArgs)
{
    if (get_Status() != BindingStatus::Unattached)
    {
        AttachToContext(AttachAttempt::Last);
    }
}

void BindingExpression::OnCollectionViewCurrentChanged(Object* pSender, EventArgs* pArgs)
{
    get_Worker()->OnCurrentChanged(object_cast<CollectionView>(pSender), pArgs);
}

void BindingExpression::OnCollectionViewCurrentChanging(Object* pSender, CurrentChangingEventArgs* pArgs)
{
    Update(true);
}


DynamicValueConverter* BindingExpression::get_DynamicConverter()
{
    if (m_pDynamicConverter == NULL)
    {
        m_pDynamicConverter = object_allocate<DynamicValueConverter>(get_IsReflective(), get_Worker()->get_SourcePropertyType(), get_Worker()->get_TargetPropertyType());
    }
    return m_pDynamicConverter;
}

bool BindingExpression::get_IgnoreSourcePropertyChange()
{
    if (!get_IsTransferPending())
    {
        if (!get_IsInUpdate())
        {
            return false;
        }
        if (get_Converter() != NULL)
        {
            return get_IsUpdateOnPropertyChanged();
        }
    }
    return true;
}

PropertyPath* BindingExpression::get_Path()
{
    return get_ParentBinding()->get_Path();
}

void BindingExpression::OnDataContextChanged(DependencyObject* pContextElement)
{
    if (!get_IsInUpdate() && get_CanActivate())
    {
        Object* pObjB = pContextElement->GetValue(UIElement::get_DataContextProperty());
        if (!Object::Equals(get_DataItem(), pObjB))
        {
            Activate(pObjB);
        }
    }
}

void BindingExpression::OnPropertyInvalidation(DependencyObject* pDO, const DependencyPropertyChangedEventArgs* pArgs)
{
    if (pDO == NULL)
    {
        LibraryCriticalError();
    }

    const DependencyProperty* pDP = &(pArgs->DependencyPropertyChanged);

    bool flag = !get_IgnoreSourcePropertyChange();
    if ((pDP == &UIElement::get_DataContextProperty()) && (pDO == get_ContextElement()))
    {
        flag = true;
    }
    else if (pDP == &CollectionViewSource::get_ViewProperty() && pDO == get_CollectionViewSource())
    {
        flag = true;
    }
    else if (flag)
    {
        flag = (get_Worker() != NULL) && get_Worker()->UsesDependencyProperty(pDO, pDP);
    }
    if (flag)
    {
        HandlePropertyInvalidation(pDO, pArgs);
    }
}

void BindingExpression::OnSourceUpdated()
{
    if (get_NotifyOnSourceUpdated())
    {
        DependencyObject* pTargetElement = get_TargetElement();
        if (pTargetElement != NULL && !get_IsInMultiBindingExpression()) // && (!get_IsInPriorityBindingExpression() || this == get_ParentPriorityBindingExpression()->get_ActiveBindingExpression())) // TODO -- PriorityBinding
        {
            OnSourceUpdated(pTargetElement, get_TargetProperty());
        }
    }
}

void BindingExpression::OnSourceUpdated(DependencyObject* pDO, const DependencyProperty* pDP)
{
    DataTransferEventArgs args(pDO, pDP);

    args.set_RoutedEvent(&Binding::get_SourceUpdatedEvent());

    UIElement* pObj2 = object_cast<UIElement>(pDO);
    if (pObj2 == NULL && pDO != NULL)
    {
        pObj2 = object_cast<UIElement>(Helper::FindMentor(pDO));
    }
    pObj2->RaiseEvent(&args);
}
 

void BindingExpression::OnTargetUpdated()
{
    if (get_NotifyOnTargetUpdated())
    {
        DependencyObject* pTargetElement = get_TargetElement();
        if (pTargetElement != NULL && !get_IsInMultiBindingExpression()) // && (!base.IsInPriorityBindingExpression || (this == base.ParentPriorityBindingExpression.ActiveBindingExpression)))
        {
            if (get_IsAttaching() && (this == pTargetElement->ReadLocalValue(*get_TargetProperty())))
            {
                get_Engine()->AddTask(this, TaskOps::RaiseTargetUpdatedEvent);
            }
            else
            {
                OnTargetUpdated(pTargetElement, get_TargetProperty());
            }
        }
    }
}

void BindingExpression::OnTargetUpdated(DependencyObject* pDO, const DependencyProperty* pDP)
{
    DataTransferEventArgs args(pDO, pDP);

    args.set_RoutedEvent(&Binding::get_TargetUpdatedEvent());

    UIElement* pObj2 = object_cast<UIElement>(pDO);
    if (pObj2 == NULL && pDO != NULL)
    {
        pObj2 = object_cast<UIElement>(Helper::FindMentor(pDO));
    }
    pObj2->RaiseEvent(&args);
}

void BindingExpression::Replace()
{
    DependencyObject* pTargetElement = get_TargetElement();
    if (pTargetElement != NULL)
    {
        if (get_IsInBindingExpressionCollection())
        {
            get_ParentBindingExpressionBase()->ReplaceChild(this);
        }
        else
        {
            BindingOperations::SetBinding(pTargetElement, get_TargetProperty(), get_ParentBinding());
        }
    }
}

void BindingExpression::ScheduleTransfer(bool isASubPropertyChange)
{
    if (isASubPropertyChange && get_Converter() != NULL)
    {
        isASubPropertyChange = false;
    }
    TransferValue(get_NullDataItem(), isASubPropertyChange);
}

void BindingExpression::SetDataItem(Object* pNewItem)
{
   m_pDataItem = BindingExpressionBase::CreateReference(pNewItem);
}

void BindingExpression::SetupDefaultValueConverter(const Type* pType)
{
    if (get_UseDefaultValueConverter())
    {
        if (get_IsInMultiBindingExpression())
        {
            set_Converter(NULL);
            m_pSourceType = pType;
        }
        else if (pType != NULL && pType != m_pSourceType)
        {
            m_pSourceType = pType;
            set_Converter(get_Engine()->GetDefaultValueConverter(m_pSourceType, &(get_TargetProperty()->get_PropertyType()), get_IsReflective()));
            if (get_Converter() == NULL)
            {
                LibraryCriticalError();
            }
            if (get_Converter() == DefaultValueConverter::get_ValueConverterNotNeeded())
            {
                set_Converter(NULL);
            }
        }
    }
}

bool BindingExpression::SetValue(DependencyObject* pDO, const DependencyProperty& dp, Object* pValue)
{
    if (get_IsReflective())
    {
        set_Value(pValue);
        return true;
    }
    return false;
}

void BindingExpression::TransferValue()
{
    TransferValue(get_NullDataItem(), false);
}

void BindingExpression::TransferValue(Object* pNewValue, bool isASubPropertyChange)
{
    TRefCountedPtr<DependencyObject> pTargetElement = get_TargetElement();

    if (pTargetElement == NULL)
    {
        return;
    }
    if (get_Worker() == NULL)
    {
        return;
    }
    set_IsTransferPending(false);
    set_IsInTransfer(true);
    set_UsingFallbackValue(false);

    TRefCountedPtr<Object> pRawValue = (pNewValue == get_NullDataItem()) ? (Object*) get_Worker()->RawValue() : pNewValue;

    /* TODO - DB / NULLABLE
    if ((pRawValue == DBNull.Value) && ((this.Converter == null) || base.UseDefaultValueConverter))
    {
        Type propertyType = base.TargetProperty.PropertyType;
        if (propertyType.IsGenericType && (propertyType.GetGenericTypeDefinition() == typeof(Nullable<>)))
        {
            pRawValue = null;
        }
        else
        {
            pRawValue = DependencyProperty.UnsetValue;
        }
    }
    else 
    */

    if(pRawValue != DependencyProperty::get_UnsetValue() && get_Converter() != NULL)
    {
        if (!get_UseDefaultValueConverter())
        {
            const Type* pTargetType = &(get_TargetProperty()->get_PropertyType());
            pRawValue = get_ConverterInterface()->Convert(pRawValue, pTargetType, get_ParentBinding()->get_ConverterParameter());
            if (pRawValue != NULL && pRawValue != Binding::get_DoNothing() && pRawValue != DependencyProperty::get_UnsetValue() && !pRawValue->IsType(*pTargetType))
            {
                pRawValue = ConvertHelper(get_DynamicConverter(), pRawValue, pTargetType, get_TargetElement());
            }
        }
        else
        {
            pRawValue = ConvertHelper(get_ConverterInterface(), pRawValue, &(get_TargetProperty()->get_PropertyType()), get_TargetElement());
        }
        if (pRawValue == Binding::get_DoNothing())
        {
            set_IsInTransfer(false);
            return;
        }
        if (pRawValue == DependencyProperty::get_UnsetValue())
        {
            SetStatus(BindingStatus::UpdateTargetError);
        }
    }
    if (!get_IsInMultiBindingExpression() && pRawValue != get_IgnoreDefaultValue() && pRawValue != DependencyProperty::get_UnsetValue() && !get_TargetProperty()->IsValidValue(pRawValue))
    {
        pRawValue = DependencyProperty::get_UnsetValue();
        if (get_Status() == BindingStatus::Active)
        {
            SetStatus(BindingStatus::UpdateTargetError);
        }
    }
    if (pRawValue == DependencyProperty::get_UnsetValue())
    {
        pRawValue = UseFallbackValue();
    }
    if (pRawValue == get_IgnoreDefaultValue())
    {
        pRawValue = Expression::get_NoValue();
    }
    ChangeValue(pRawValue, true);
    Invalidate(isASubPropertyChange);
    OnTargetUpdated();

//     Validation.ClearInvalid(this); TODO Validation

    set_IsInTransfer(false);
}

IValueConverter* BindingExpression::get_ConverterInterface()
{
    return interface_cast<IValueConverter>(get_Converter());
}

void BindingExpression::Update(bool synchronous)
{
    if (get_NeedsUpdate() && get_IsReflective() && !get_IsInTransfer() && get_Worker() != NULL && get_Worker()->get_CanUpdate())
    {
        if (synchronous)
        {
            UpdateValue();
        }
        else
        {
            get_Engine()->AddTask(this, TaskOps::UpdateValue);
        }
    }
}

void BindingExpression::UpdateSource()
{
    if (get_Status() == BindingStatus::Detached)
    {
        LibraryCriticalError();
    }
    set_NeedsUpdate(true);
    Update(true);
}

void BindingExpression::UpdateTarget()
{
    if (get_Status() == BindingStatus::Detached)
    {
        LibraryCriticalError();
    }
    if (get_Worker() != NULL)
    {
        get_Worker()->RefreshValue();
    }
}

// TODO -- Validation
void BindingExpression::UpdateValue()
{
    TRefCountedPtr<Object> pObj2 = get_Value();

    /*
    Collection<ValidationRule> validationRulesInternal = this.ParentBinding.ValidationRulesInternal;
    ExceptionValidationRule exceptionValidationRule = null;
    base.UpdateValidationError(null);
    if (validationRulesInternal != null)
    {
        foreach (ValidationRule rule2 in validationRulesInternal)
        {
            ExceptionValidationRule rule3 = rule2 as ExceptionValidationRule;
            if (rule3 != null)
            {
                exceptionValidationRule = rule3;
                continue;
            }
            ValidationResult result = rule2.Validate(obj2, culture);
            if (!result.IsValid)
            {
                base.UpdateValidationError(new ValidationError(rule2, this, result.ErrorContent, null));
                return;
            }
        }
    }
    */

    if (get_Converter() != NULL)
    {
        if (!get_UseDefaultValueConverter())
        {
            const Type* pSourcePropertyType = get_Worker()->get_SourcePropertyType();
            pObj2 = get_ConverterInterface()->ConvertBack(pObj2, pSourcePropertyType, get_ParentBinding()->get_ConverterParameter());
            if (pObj2 != NULL && pObj2 != Binding::get_DoNothing() && pObj2 != DependencyProperty::get_UnsetValue() && !pObj2->IsType(*pSourcePropertyType))
            {
                pObj2 = ConvertBackHelper(get_DynamicConverter(), pObj2, pSourcePropertyType, get_TargetElement());
            }
        }
        else
        {
            pObj2 = ConvertBackHelper(get_ConverterInterface(), pObj2, get_Worker()->get_SourcePropertyType(), get_TargetElement());
        }
    }
    if (pObj2 == DependencyProperty::get_UnsetValue())
    {
        SetStatus(BindingStatus::UpdateSourceError);
    }
    if (pObj2 != Binding::get_DoNothing() && pObj2 != DependencyProperty::get_UnsetValue())
    {
        BeginSourceUpdate();
        get_Worker()->UpdateValue(pObj2);
        EndSourceUpdate();

        OnSourceUpdated();
    }
    else
    {
        EndSourceUpdate();
    }
}

void BindingExpressionUncommonField::ClearValue(DependencyObject* pInstance)
{
    BindingExpression* pExpression = GetValue(pInstance);
    if (pExpression != NULL)
    {
        pExpression->Detach();
    }
    UncommonField<BindingExpression>::ClearValue(pInstance);
}

void BindingExpressionUncommonField::SetValue(DependencyObject* pInstance, BindingExpression* pBindingExpr)
{
    UncommonField<BindingExpression>::SetValue(pInstance, pBindingExpr);
    pBindingExpr->Attach(pInstance);
}



}; // namespace AVUI
