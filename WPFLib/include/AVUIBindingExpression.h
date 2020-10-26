#pragma once

#include <AVUIExpression.h>
#include <AVUIDependencyObject.h>
#include <AVUIDependencyProperty.h>
#include <AVUIBindingExpressionBase.h>
#include <AVUIDataBindEngine.h>

namespace AVUI {

class PropertyPath;
class RelativeSource;
class DependencyObject;
class Binding;
class DataSourceProvider;
class DynamicValueConverter;
class BindingWorker;
class CollectionViewSource;
class CurrentChangingEventArgs;

class IValueConverter;

class BindingExpression : public BindingExpressionBase, public IDataBindEngineClient
{
private:
    class AttachAttempt
    {
    public:
        enum Enum
        {
            First,
            Again,
            Last
        };
    };

public:
    BindingExpression(Binding* pBinding, BindingExpressionBase* pOwner);
    ~BindingExpression();

    DECLARE_ELEMENT(BindingExpression, BindingExpressionBase);

    virtual void Activate();
    void Activate(Object* pItem);
    virtual void AttachOverride(DependencyObject* pTarget, const DependencyProperty* pDP);
    void AttachToContext(AttachAttempt::Enum attempt); 
    void CancelPendingTasks();
    virtual void ChangeSourcesForChild(BindingExpressionBase* pBindingExpression, ItemStructList<WeakDependencySource>* pNewSources) { };
    void ChangeWorkerSources(ItemStructList<WeakDependencySource>* pNewWorkerSources, int n);
    TRefCountedPtr<Object> ChangingValue();
    static TRefCountedPtr<BindingExpression> CreateBindingExpression(DependencyObject* pDO, const DependencyProperty* pDP, Binding* pBinding, BindingExpressionBase* pParent);
    virtual void Deactivate();
    virtual void DetachOverride();
    virtual ObjectPtr GetValue(DependencyObject* pDO, const DependencyProperty& dp);
    static bool HasLocalDataContext(DependencyObject* pDO);
    virtual void InvalidateChild(BindingExpressionBase* pBindingExpression) { };

 


    static Object* get_NullDataItem() { static Object NullDataItem; return &NullDataItem; }
    static Object* get_IgnoreDefaultValue() { static Object IgnoreDefaultValue; return &IgnoreDefaultValue; }

    CollectionViewSource* get_CollectionViewSource() { return (CollectionViewSource*) (Object*) BindingExpressionBase::GetReference(m_pCollectionViewSource); }
    void set_CollectionViewSource(CollectionViewSource* pSource) { m_pCollectionViewSource = BindingExpressionBase::CreateReference((Object*)pSource); }
    DependencyObject* get_ContextElement() { return object_cast<DependencyObject>(m_pCtxElement); }
    Object* get_Converter() { return m_pValueConverter; }
    IValueConverter* get_ConverterInterface();
    void set_Converter(Object* pConverter) { m_pValueConverter = pConverter; }
    const Type* get_ConverterSourceType() { return m_pSourceType    ; }
    Object* get_DataItem() { return BindingExpressionBase::GetReference(m_pDataItem); }
    Object* get_DataSource() { return NULL; } // TODO -- DataSource
    DynamicValueConverter* get_DynamicConverter();
    bool get_IgnoreSourcePropertyChange();
    Binding* get_ParentBinding() { return (Binding*) get_ParentBindingBase(); }
    PropertyPath* get_Path();
    BindingWorker* get_Worker() { return m_pBindingWorker; }
    void OnPropertyInvalidation(DependencyObject* pDO, const DependencyPropertyChangedEventArgs* pArgs);
    static void OnSourceUpdated(DependencyObject* pDO, const DependencyProperty* pDP);
    static void OnTargetUpdated(DependencyObject* pDO, const DependencyProperty* pDP);
    virtual void ReplaceChild(BindingExpressionBase* pBindingExpression) { };
    void ScheduleTransfer(bool isASubPropertyChange);
    void SetupDefaultValueConverter(const Type* pType);
    virtual bool SetValue(DependencyObject* pDO, const DependencyProperty& dp, Object* pValue);
    void TransferValue(Object* pNewValue, bool isASubPropertyChange);
    virtual void Update(bool synchronous);
    virtual void UpdateSource();
    virtual void UpdateTarget();

 

    void OnCollectionViewCurrentChanged(Object* pSender, EventArgs* pArgs);
    void OnCollectionViewCurrentChanging(Object* pSender, CurrentChangingEventArgs* pArgs);

private:

    void UpdateValue();
    void TransferValue();
    void SetDataItem(Object* pNewItem);
    void Replace();
    void OnSourceUpdated();
    void OnTargetUpdated();
    void OnDataContextChanged(DependencyObject* pContextElement);

    virtual bool ClientAttachToContext(bool lastChance) { AttachToContext(lastChance ? AttachAttempt::Last : AttachAttempt::Again); return get_Status() != BindingStatus::Unattached; }
    virtual void ClientOnTargetUpdated() { OnTargetUpdated(); }
    virtual void ClientTransferValue()  { TransferValue(); }
    virtual void ClientUpdateValue() { UpdateValue(); }
    virtual DependencyObject* get_ClientTargetElement() { return get_TargetElement(); }


    void CreateWorker();
    TRefCountedPtr<Object> DereferenceDataProvider(Object* pItem);
    void DetachFromContext();
    bool ExceptionFilterExists() { return false; } // TODO -- Exception filtering?
    TRefCountedPtr<Object> GetDataSourceForDataContext(DependencyObject* pDO);
    void HandlePropertyInvalidation(DependencyObject* pDO, const DependencyPropertyChangedEventArgs* pArgs);
    Object* HandlePropertyInvalidationOperation(Object* pObj);
    void Invalidate(bool isASubPropertyChange);

 

 

 

 

    TRefCountedPtr<Object> ConvertBackHelper(IValueConverter* pConverter, Object* pValue, const Type* pSourceType, Object* pParameter);
    TRefCountedPtr<Object> ConvertHelper(IValueConverter* pConverter, Object* pValue, const Type* pTargetType, Object* pParameter);

    void OnTargetLostFocus(Object* pSender, RoutedEventArgs* pArgs);
    void OnTargetInheritanceContextChanged(Object* pSender, EventArgs* pArgs);

    bool get_CanActivate() { return get_Status() != BindingStatus::Unattached; }

    class ChangingValueHelper : public Object
    {
    public:
        ChangingValueHelper(BindingExpression* pB) : m_pB(pB) { pB->CancelPendingTasks(); }
        virtual ~ChangingValueHelper() { m_pB->TransferValue(); }

    private:
        BindingExpression* m_pB;
    };

    TRefCountedPtr<Object> m_pCollectionViewSource;
    TWeakPtr<Object> m_pCtxElement;
    TRefCountedPtr<Object> m_pDataItem;
    TRefCountedPtr<DataSourceProvider> m_pDataProvider;
    TRefCountedPtr<DynamicValueConverter> m_pDynamicConverter;
    const Type* m_pSourceType;
    TRefCountedPtr<Object> m_pValueConverter;
    TRefCountedPtr<BindingWorker> m_pBindingWorker;
};

class BindingExpressionUncommonField : public UncommonField<BindingExpression>
{
public:
    void ClearValue(DependencyObject* pInstance);
    void SetValue(DependencyObject* pInstance, BindingExpression* pBindingExpr);
};


typedef TRefCountedPtr<BindingExpression> BindingExpressionPtr;

};
