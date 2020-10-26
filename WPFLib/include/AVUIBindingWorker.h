#pragma once

namespace AVUI {
 
class BindingExpression;
class CollectionView;
class Dispatcher;
class DataBindEngine;
class Binding;

class BindingWorker : public Object
{
public:
    BindingWorker(BindingExpression* pB) : m_pBindingExpression(pB) { }

    virtual void AttachDataItem() { }
    virtual void DetachDataItem() { }

    virtual void OnCurrentChanged(CollectionView* pCollectionView, EventArgs* pArgs) { };
    virtual void OnSourceInvalidation(DependencyObject* pDO, const DependencyProperty* pDP, bool isASubPropertyChange) { };

    virtual TRefCountedPtr<Object> RawValue() { return NULL; }
    virtual void RefreshValue() { }
    virtual void UpdateValue(Object* pValue) { }
    virtual bool UsesDependencyProperty(DependencyObject* pDO, const DependencyProperty* pDP) { return false; }

    virtual bool get_CanUpdate() { return false; }
    virtual const Type* get_SourcePropetyType() { return NULL; }

    bool get_IsReflective();
    BindingExpression* get_ParentBindingExpression();
    virtual const Type* get_SourcePropertyType() { return NULL; }
    const Type* get_TargetPropertyType();

protected:
    void SetTransferIsPending(bool value);
    TRefCountedPtr<Object> get_DataItem();
    Dispatcher* get_Dispatcher();
    DataBindEngine* get_DataBindEngine();
    bool get_IgnoreSourcePropertyChange();
    bool get_IsDynamic();
    Binding* get_ParentBinding();
    BindingStatus::Enum get_Status();
    void set_Status(BindingStatus::Enum bindingStatus);
    DependencyObject* get_TargetElement();
    const DependencyProperty* get_TargetProperty();

private:
    TWeakPtr<BindingExpression> m_pBindingExpression;
};


}; // namespace AVUI
