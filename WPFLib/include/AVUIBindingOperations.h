#pragma once

namespace AVUI {

class DependencyObject;
class DependencyProperty;
class BindingExpression;
class BindingExpressionBase;
class BindingBase;

class BindingOperations
{
public:

    static TRefCountedPtr<BindingExpression> GetBindingExpression(DependencyObject* pTarget, const DependencyProperty* pDP);
    static TRefCountedPtr<BindingExpressionBase> GetBindingExpressionBase(DependencyObject* pTarget, const DependencyProperty* pDP);
    static TRefCountedPtr<BindingExpressionBase> SetBinding(DependencyObject* pTarget, const DependencyProperty* pDP, BindingBase* pBinding);
    static bool IsDataBound(DependencyObject* pTarget, const DependencyProperty* pDP);
    static void ClearBinding(DependencyObject* pTarget, const DependencyProperty* pDP);
};


};

