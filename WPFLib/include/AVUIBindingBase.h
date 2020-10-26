#pragma once

#include <AVUIEventableObject.h>

namespace AVUI {

class PropertyPath;


class BindingFlags
{
public:
    enum Enum
    {
        Default = 0xc04,
        IllegalInput = 0x40000000,
        NotifyOnSourceUpdated = 0x800000,
        NotifyOnTargetUpdated = 8,
        NotifyOnValidationError = 0x200000,
        OneTime = 0,
        OneWay = 1,
        OneWayToSource = 2,
        PathGeneratedInternally = 0x2000,
        PropagationMask = 7,
        PropDefault = 4,
        TwoWay = 3,
        UpdateDefault = 0xc00,
        UpdateExplicitly = 0x800,
        UpdateMask = 0xc00,
        UpdateOnLostFocus = 0x400,
        UpdateOnPropertyChanged = 0
    };
};

class BindingExpressionBase;

class BindingBase : public EventableObject
{
public:
    BindingBase();
    ~BindingBase();

    DECLARE_ELEMENT(BindingBase, Object);

    void ChangeFlag(BindingFlags::Enum flags, bool value);
    void ChangeFlagsWithinMask(BindingFlags::Enum mask, BindingFlags::Enum flags);
    void CheckSealed();
    void ClearFlag(BindingFlags::Enum flag);
    static BindingFlags::Enum FlagsFrom(BindingMode::Enum bindingMode);
    static BindingFlags::Enum FlagsFrom(UpdateSourceTrigger::Enum updateSourceTrigger);
    BindingFlags::Enum GetFlagsWithinMask(BindingFlags::Enum mask);
    void SetFlag(BindingFlags::Enum flag);
    bool TestFlag(BindingFlags::Enum flag);

    TRefCountedPtr<BindingExpressionBase> CreateBindingExpression(DependencyObject* pTargetObject, const DependencyProperty* pTargetProperty);
    TRefCountedPtr<BindingExpressionBase> CreateBindingExpression(DependencyObject* pTargetObject, const DependencyProperty* pTargetProperty, BindingExpressionBase* pOwner);

    Object* get_FallbackValue() { return m_pFallbackValue; }
    void set_FallbackValue(Object* pFallbackValue) { CheckSealed(); m_pFallbackValue = pFallbackValue; }
    BindingFlags::Enum get_Flags() { return m_flags; }
 
protected:
    virtual TRefCountedPtr<BindingExpressionBase> CreateBindingExpressionOverride(DependencyObject* pTargetObject, const DependencyProperty* pTargetProperty, BindingExpressionBase* pOwner) = 0; 

private:
    TRefCountedPtr<Object> m_pFallbackValue;
    BindingFlags::Enum m_flags;
    bool m_isSealed;

};


};
