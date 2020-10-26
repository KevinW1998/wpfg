#include <AVUICommon.h>
#include <AVUIBindingWorker.h>
#include <AVUIBindingExpression.h>

namespace AVUI {

bool BindingWorker::get_IsReflective()
{
    return get_ParentBindingExpression()->get_IsReflective();
}

BindingExpression* BindingWorker::get_ParentBindingExpression()
{
    return m_pBindingExpression;
}

void BindingWorker::SetTransferIsPending(bool value)
{
    get_ParentBindingExpression()->set_IsTransferPending(value);
}

TRefCountedPtr<Object> BindingWorker::get_DataItem()
{
    return get_ParentBindingExpression()->get_DataItem();
}

Dispatcher* BindingWorker::get_Dispatcher()
{
    return get_ParentBindingExpression()->get_Dispatcher();
}

DataBindEngine* BindingWorker::get_DataBindEngine()
{
    return get_ParentBindingExpression()->get_Engine();
}

bool BindingWorker::get_IgnoreSourcePropertyChange()
{
    return get_ParentBindingExpression()->get_IgnoreSourcePropertyChange();
}

bool BindingWorker::get_IsDynamic()
{
    return get_ParentBindingExpression()->get_IsDynamic();
}

Binding* BindingWorker::get_ParentBinding()
{
    return get_ParentBindingExpression()->get_ParentBinding();
}

BindingStatus::Enum BindingWorker::get_Status()
{
    return get_ParentBindingExpression()->get_Status();
}

void BindingWorker::set_Status(BindingStatus::Enum bindingStatus)
{
    return get_ParentBindingExpression()->SetStatus(bindingStatus);
}

DependencyObject* BindingWorker::get_TargetElement()
{
    return get_ParentBindingExpression()->get_TargetElement();
}

const DependencyProperty* BindingWorker::get_TargetProperty()
{
    return get_ParentBindingExpression()->get_TargetProperty();
}
const Type* BindingWorker::get_TargetPropertyType()
{
    return &(get_TargetProperty()->get_PropertyType());
}


}; // namespace AVUI

