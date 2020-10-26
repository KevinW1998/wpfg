#include <AVUICommon.h>
#include <AVUIBindingOperations.h>
#include <AVUIBindingExpression.h>
#include <AVUIBinding.h>
#include <AVUIStyleHelper.h>
#include <AVUITemplateBinding.h>

namespace AVUI {


TRefCountedPtr<BindingExpression> BindingOperations::GetBindingExpression(DependencyObject* pTarget, const DependencyProperty* pDP)
{
    BindingExpressionBase* pBindingExpressionBase = GetBindingExpressionBase(pTarget, pDP);

    return object_cast<BindingExpression>(pBindingExpressionBase);
}

TRefCountedPtr<BindingExpressionBase> BindingOperations::GetBindingExpressionBase(DependencyObject* pTarget, const DependencyProperty* pDP)
{
    if (pTarget == NULL)
    {
        LibraryCriticalError();
    }
    if (pDP == NULL)
    {
        LibraryCriticalError();
    }
    return object_cast<BindingExpressionBase>(StyleHelper::GetExpression(pTarget, pDP));

}

TRefCountedPtr<BindingExpressionBase> BindingOperations::SetBinding(DependencyObject* pTarget, const DependencyProperty* pDP, BindingBase* pBinding)
{
    if (pTarget == NULL)
    {
        LibraryCriticalError();
    }
    if (pDP == NULL)
    {
        LibraryCriticalError();
    }
    if (pBinding == NULL)
    {
        LibraryCriticalError();
    }
    TRefCountedPtr<BindingExpressionBase> pBase2 = pBinding->CreateBindingExpression(pTarget, pDP);

    pTarget->SetValue(*pDP, pBase2);

    return pBase2;
}

bool BindingOperations::IsDataBound(DependencyObject* pTarget, const DependencyProperty* pDP)
{
    Expression* pExpression = StyleHelper::GetExpression(pTarget, pDP);

    return pExpression != NULL && pExpression->IsType(GET_TYPE(BindingExpressionBase));
}


void BindingOperations::ClearBinding(DependencyObject* pTarget, const DependencyProperty* pDP)
{
    if (IsDataBound(pTarget, pDP))
    {
        pTarget->ClearValue(*pDP);
    }
}

 


};

