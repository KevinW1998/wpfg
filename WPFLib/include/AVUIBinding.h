#pragma once

#include <AVUIObjectRef.h>
#include <AVUIBindingBase.h>

namespace AVUI {

class DataTransferEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(DataTransferEventArgs, RoutedEventArgs);
    DataTransferEventArgs(DependencyObject* pDO, const DependencyProperty* pDP) : m_pTargetObject(pDO), m_pDP(pDP) { }

private:
    const DependencyProperty* m_pDP;
    TRefCountedPtr<DependencyObject> m_pTargetObject;
};




class Binding : public BindingBase
{
public:
    Binding();
    Binding(const String& propertyPath);

    DECLARE_ELEMENT(Binding, BindingBase);
    IMPLEMENT_REFERENCE_PROPERTY(ElementName, String, false);
    IMPLEMENT_REFERENCE_PROPERTY(Path, PropertyPath, false);
    IMPLEMENT_REFERENCE_PROPERTY(RelativeSource, RelativeSource, false);
    IMPLEMENT_ENUM_PROPERTY(Mode, BindingMode, false);
    IMPLEMENT_ROUTED_EVENT(SourceUpdated, DataTransferEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(TargetUpdated, DataTransferEventArgs, RoutingStrategy::Bubble);

    Object* get_AsyncState() { return m_pAsyncState; }
    void set_AsyncState(Object* pAsyncState) { CheckSealed(); m_pAsyncState = pAsyncState; }

    bool get_BindsDirectlyToSource() { return m_bindsDirectlyToSource; }
    void set_BindsDirectlyToSource(bool bindsDirectlyToSource) { CheckSealed(); m_bindsDirectlyToSource = bindsDirectlyToSource; }

    Object* get_Converter() { return m_pValueConverter; }
    void set_Converter(Object* pValueConverter) { CheckSealed(); m_pValueConverter = pValueConverter; }

    Object* get_ConverterParameter() { return m_pConverterParameter; }
    void set_ConverterParameter(Object* pConverterParameter) { CheckSealed(); m_pConverterParameter = pConverterParameter; }

    String* get_ElementName() { return m_pElementSource; };
    void set_ElementName(String* pElementName);

    bool get_IsAsync() { return m_isAsync; }
    void set_IsAsync(bool isAsync) { CheckSealed(); m_isAsync = isAsync; }

    BindingMode::Enum get_Mode();
    void set_Mode(BindingMode::Enum mode);

    bool get_NotifyOnSourceUpdated();
    void set_NotifyOnSourceUpdated(bool notifyOnSourceUpdated);

    bool get_NotifyOnTargetUpdated();
    void set_NotifyOnTargetUpdated(bool notifyOnTargetUpdated);

    bool get_NotifyOnValidationError();
    void set_NotifyOnValidationError(bool notifyOnValidationError);    

    PropertyPath* get_Path() { return m_pPropertyPath; }
    void set_Path(PropertyPath* pPath);

    RelativeSource* get_RelativeSource() { return m_pRelativeSource; }
    void set_RelativeSource(RelativeSource* pRelativeSource);

    Object* get_Source() { return m_pObjectSource; }
    void set_Source(Object* pSource);

    ObjectRef* get_SourceReference() { if(m_pSource != get_UnsetSource()) return m_pSource; return NULL; }
    void set_SourceReference(ObjectRef* pSourceReference);

    bool get_TransfersDefaultValue() { return !m_doesNotTransferDefaultValue; }
    void set_TransfersDefaultValue(bool transfersDefaultValue) { CheckSealed(); m_doesNotTransferDefaultValue = !transfersDefaultValue; }

    bool get_TreeContextIsRequired();

    // Exception filter?

    UpdateSourceTrigger::Enum get_UpdateSourceTrigger();
    void set_UpdateSourceTrigger(UpdateSourceTrigger::Enum mode);

    // TODO -- Validation rules

    String* get_XPath() { return m_pXPath; };
    void set_XPath(String* pXPath) { CheckSealed(); m_pXPath = pXPath; }

    static Object* get_DoNothing() { static Object DoNothing; return &DoNothing; }
    static ObjectRef* get_UnsetSource() { static ExplicitObjectRef UnsetSource(NULL);  return &UnsetSource; }

    void UsePath(PropertyPath* pPath);

protected:

    TRefCountedPtr<BindingExpressionBase> CreateBindingExpressionOverride(DependencyObject* pTarget, const DependencyProperty* pDP, BindingExpressionBase* pOwner); 

private:
    void Initialize();
    void DetermineSource();
    Object* get_WorkerData() { return m_pWorkerData; }
    void set_WorkerData(Object* pWorkerData) { m_pWorkerData = pWorkerData; }

    class SourceProperties
    {
    public:
        enum Enum
        {
            None = 0,
            RelativeSource = 1,
            ElementName = 2,
            Source = 3,
            InternalSource = 4
        };
    };

    TRefCountedPtr<Object> m_pAsyncState;
    int m_attachedPropertiesInPath;
    bool m_bindsDirectlyToSource;
    TRefCountedPtr<Object> m_pValueConverter;
    TRefCountedPtr<Object> m_pConverterParameter;
    bool m_doesNotTransferDefaultValue;    
    StringPtr m_pElementSource;
    bool m_isAsync;
    TWeakPtr<Object> m_pObjectSource;
    TRefCountedPtr<PropertyPath> m_pPropertyPath;
    TRefCountedPtr<RelativeSource> m_pRelativeSource;
    TRefCountedPtr<ObjectRef> m_pSource;
    SourceProperties::Enum m_sourceInUse;
    //ValidationRuleCollection m_validationRules;
    TRefCountedPtr<Object> m_pWorkerData;
    StringPtr m_pXPath;
};

typedef TRefCountedPtr<Binding> BindingPtr;

};
