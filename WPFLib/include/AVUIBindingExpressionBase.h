#pragma once

#include <AVUIExpression.h>

namespace AVUI {

class BindingBase;
class DataBindEngine;
class ICollection;
class BindingListCollectionView;
class Dispatcher;
class MultiBindingExpression;

class WeakReferenceObject : public Object
{
public:

    DECLARE_ELEMENT(WeakReferenceObject, Object);

    WeakReferenceObject(Object* pO) : m_pData(pO) { }
    Object* get_Data() { return m_pData; }

private:

    TWeakPtr<Object> m_pData;

};


class BindingValueChangedEventArgs : public EventArgs
{
public:
    BindingValueChangedEventArgs(Object* pOldValue, Object* pNewValue) { }

};

class BindingExpressionBase : public Expression
{
private:

    class PrivateFlags
    {
    public:
        enum Enum
        {
            iAttaching = 0x400000,
            iDefaultValueConverter = 0x10,
            iDetaching = 0x10000,
            iIllegalInput = 0x40000000,
            iInMultiBindingExpression = 0x80000,
            iInPriorityBindingExpression = 0x40000,
            iInTransfer = 0x20,
            iInUpdate = 0x40,
            iNeedDataTransfer = 0x100,
            iNeedsCollectionView = 0x20000,
            iNeedUpdate = 0x1000,
            iNotifyOnSourceUpdated = 0x800000,
            iNotifyOnTargetUpdated = 8,
            iNotifyOnValidationError = 0x200000,
            iPathGeneratedInternally = 0x2000,
            iPropagationMask = 7,
            iPropDefault = 4,
            iResolveNamesInTemplate = 0x8000,
            iSourceToTarget = 1,
            iTargetToSource = 2,
            iTransferDeferred = 0x200,
            iTransferPending = 0x80,
            iUpdateDefault = 0xc00,
            iUpdateExplicitly = 0x800,
            iUpdateMask = 0xc00,
            iUpdateOnLostFocus = 0x400,
            iUsingFallbackValue = 0x100000,
            iUsingMentor = 0x4000
        };
    };

public:
    DECLARE_ELEMENT(BindingExpressionBase, Expression);
    BindingExpressionBase(BindingBase* pBinding, BindingExpressionBase* pParent);


    virtual void Activate() { }
    void Attach(DependencyObject* pDO) { Attach(pDO, get_NoTargetProperty()); }
    void Attach(DependencyObject* pTarget, const DependencyProperty* pDP);
    virtual void AttachOverride(DependencyObject* pTarget, const DependencyProperty* pDP);
    void BeginSourceUpdate() { ChangeFlag(PrivateFlags::iInUpdate, true); }
    void ChangeFlag(PrivateFlags::Enum flag, bool value);
    void ChangeSources(ItemStructList<WeakDependencySource>* pNewSources);
    virtual void ChangeSourcesForChild(BindingExpressionBase* pBindingExpression, ItemStructList<WeakDependencySource>* pNewSources) = 0;
    void ChangeValue(Object* pNewValue, bool notify);
    virtual TRefCountedPtr<Expression> Copy(DependencyObject* pDO, const DependencyProperty& dp);
    virtual void Deactivate() { }
    void Detach();
    virtual void DetachOverride();
    void Dirty();
    void EndSourceUpdate();
    virtual TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > GetSources();
    virtual void InvalidateChild(BindingExpressionBase* pBindingExpression) = 0;
    virtual void OnAttach(DependencyObject* pDO, const DependencyProperty& dp);
    virtual void OnDetach(DependencyObject* pDO, const DependencyProperty& dp);
    virtual void ReplaceChild(BindingExpressionBase* pBindingExpression) = 0;
    void ResolvePropertyDefaultSettings(BindingMode::Enum mode, UpdateSourceTrigger::Enum updateTrigger, DependencyPropertyMetadata* pMetaData);
    void SetStatus(BindingStatus::Enum status);
    bool TestFlag(PrivateFlags::Enum flag);
    virtual void Update(bool synchronous) { };
    virtual void UpdateSource() { };
    virtual void UpdateTarget() { };
    TRefCountedPtr<Object> UseFallbackValue();
 

    static TRefCountedPtr<ItemStructList<WeakDependencySource> > CombineSources(int index, ICollection* pBindingExpressions, int count, ItemStructList<WeakDependencySource>* pNewSources);
    static TRefCountedPtr<Object> ConvertFallbackValue(Object* pValue, const DependencyProperty* pDP, Object* pSender);
    static TRefCountedPtr<Object> CreateReference(Object* pItem);
    static TRefCountedPtr<Object> GetReference(Object* pReference);
    static TRefCountedPtr<BindingExpressionBase> CreateUntargetedBindingExpression(DependencyObject* pDO, BindingBase* pBinding);

 

    Object* get_DefaultValue();
    Dispatcher* get_Dispatcher();
    DataBindEngine* get_Engine() { return m_pEngine; }
    TRefCountedPtr<Object> get_FallbackValue();
    virtual bool get_HasError() { return false;} // TODO Validation
    bool get_IsAttaching() { return TestFlag(PrivateFlags::iAttaching); }
    void set_IsAttaching(bool isAttaching) { ChangeFlag(PrivateFlags::iAttaching, isAttaching); }
    bool get_IsDetaching() { return TestFlag(PrivateFlags::iDetaching); }
    void set_IsDetaching(bool isDetaching) { ChangeFlag(PrivateFlags::iDetaching, isDetaching); }
    bool get_IsDynamic();
    bool get_IsInBindingExpressionCollection() { return TestFlag((PrivateFlags::Enum)(PrivateFlags::iInMultiBindingExpression | PrivateFlags::iInPriorityBindingExpression)); }
    bool get_IsInMultiBindingExpression() { return TestFlag(PrivateFlags::iInMultiBindingExpression); }
    bool get_IsInPriorityBindingExpression() { return TestFlag(PrivateFlags::iInPriorityBindingExpression); }
    bool get_IsInTransfer() { return TestFlag(PrivateFlags::iInTransfer); }
    void set_IsInTransfer(bool isInTransfer) { ChangeFlag(PrivateFlags::iInTransfer, isInTransfer); }
    bool get_IsInUpdate() { return TestFlag(PrivateFlags::iInUpdate); }
    void set_IsInUpdate(bool isInUpdate) { ChangeFlag(PrivateFlags::iInUpdate, isInUpdate); }
    bool get_IsOneWayToSource() { return ((m_flags & PrivateFlags::iPropagationMask) == PrivateFlags::iTargetToSource); }
    bool get_IsReflective();
    void set_IsReflective(bool isReflective) { ChangeFlag(PrivateFlags::iTargetToSource, isReflective); }
    bool get_IsTransferPending() { return TestFlag(PrivateFlags::iTransferPending); }
    void set_IsTransferPending(bool isTransferPending) { ChangeFlag(PrivateFlags::iTransferPending, isTransferPending); }
    bool get_IsUpdateOnLostFocus() { return TestFlag(PrivateFlags::iUpdateOnLostFocus); }
    bool get_IsUpdateOnPropertyChanged() { return ((m_flags & PrivateFlags::iUpdateMask) == 0); }
    bool get_NeedsDataTransfer() { return TestFlag(PrivateFlags::iNeedDataTransfer); }
    void set_NeedsDataTransfer(bool needDataTransfer) { ChangeFlag(PrivateFlags::iNeedDataTransfer, needDataTransfer); }
    bool get_NeedsUpdate() { return TestFlag(PrivateFlags::iNeedUpdate); }
    void set_NeedsUpdate(bool needUpdate) { ChangeFlag(PrivateFlags::iNeedUpdate, needUpdate); }
    bool get_NotifyOnSourceUpdated() { return TestFlag(PrivateFlags::iNotifyOnSourceUpdated); }
    void set_NotifyOnSourceUpdated(bool notifyOnSourceUpdated) { ChangeFlag(PrivateFlags::iNotifyOnSourceUpdated, notifyOnSourceUpdated); }
    bool get_NotifyOnTargetUpdated() { return TestFlag(PrivateFlags::iNotifyOnTargetUpdated); }
    void set_NotifyOnTargetUpdated(bool notifyOnTargetUpdated) { ChangeFlag(PrivateFlags::iNotifyOnTargetUpdated, notifyOnTargetUpdated); }
    bool get_NotifyOnValidationError() { return TestFlag(PrivateFlags::iNotifyOnValidationError); }
    void set_NotifyOnValidationError(bool notifyOnValidationError) { ChangeFlag(PrivateFlags::iNotifyOnValidationError, notifyOnValidationError); }
    BindingBase* get_ParentBindingBase() { return m_pBinding; }
    BindingExpressionBase* get_ParentBindingExpressionBase() { return m_pParentBindingExpression; }
    MultiBindingExpression* get_ParentMultiBindingExpression() { return object_cast<MultiBindingExpression>(m_pParentBindingExpression); }
    bool get_ResolveNamesInTemplate() { return TestFlag(PrivateFlags::iResolveNamesInTemplate); }
    void set_ResolveNamesInTemplate(bool resolveNamesInTemplate) { ChangeFlag(PrivateFlags::iResolveNamesInTemplate, resolveNamesInTemplate); }
    BindingStatus::Enum get_Status() { return m_status; }
    DependencyObject* get_TargetElement();
    const DependencyProperty* get_TargetProperty() { return m_pTargetProperty; }
    bool get_TransferIsDeferred() { return TestFlag(PrivateFlags::iTransferDeferred); }
    void set_TransferIsDeferred(bool transferIsDeferred) { ChangeFlag(PrivateFlags::iTransferDeferred, transferIsDeferred); }
    bool get_UseDefaultValueConverter() { return TestFlag(PrivateFlags::iDefaultValueConverter); }
    void set_UseDefaultValueConverter(bool useDefaultValueConverter) { ChangeFlag(PrivateFlags::iDefaultValueConverter, useDefaultValueConverter); }
    bool get_UsingFallbackValue() { return TestFlag(PrivateFlags::iUsingFallbackValue); }
    void set_UsingFallbackValue(bool useFallbackValue) { ChangeFlag(PrivateFlags::iUsingFallbackValue, useFallbackValue); }
    bool get_UsingMentor() { return TestFlag(PrivateFlags::iUsingMentor); }
    void set_UsingMentor(bool usingMentor) { ChangeFlag(PrivateFlags::iUsingMentor, usingMentor); }
    Object* get_Value();
    void set_Value(Object* pNewValue);
    ItemStructList<WeakDependencySource>* get_WeakSources() { return &m_sources; }

    IMPLEMENT_EVENT(ValueChanged, BindingValueChangedEventArgs);

    static Object* get_DefaultValueObject() { static Object DefaultValueObject; return &DefaultValueObject; }
    static DependencyProperty* get_NoTargetProperty() { static DependencyPropertyMetadata NTMD(PropertyFlag::None); static DependencyProperty NoTargetProperty("NoTarget", GET_TYPE(BindingExpressionBase), GET_TYPE(Object),
                                                                         NULL, &NTMD);  return &NoTargetProperty; }

private:

    void ChangeSources(DependencyObject* pTarget, const DependencyProperty* pDP, ItemStructList<WeakDependencySource>* pNewSources);



    TRefCountedPtr<BindingBase> m_pBinding;
    DataBindEngine* m_pEngine;
    PrivateFlags::Enum m_flags;
    BindingExpressionBase* m_pParentBindingExpression;
    ItemStructList<WeakDependencySource> m_sources;
    BindingStatus::Enum m_status;
    TWeakPtr<Object> m_pTargetElement;
    const DependencyProperty* m_pTargetProperty;
    TRefCountedPtr<Object> m_pValue;

};

}; // namespace AVUI

