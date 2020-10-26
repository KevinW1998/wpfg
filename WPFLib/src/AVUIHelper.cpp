#include <AVUICommon.h>
#include <AVUIHelper.h>
#include <AVUIDependencyObject.h>
#include <AVUIUIElement.h>
#include <AVUITemplate.h>

namespace AVUI
{

bool Helper::HasDefaultValue(const DependencyProperty& dp, DependencyObject* pDO, UIElement* pUIElement)
{
    bool hasModifiers;
    Object* pObjValue;
    DependencyPropertyMetadata* pMetadata = dp.GetMetadata(pDO->get_Type());

    if (pDO->GetValueSource(dp, pMetadata, &hasModifiers) != BaseValueSource::Default)
    {
        return false;
    }

    EffectiveValueEntry entry(dp);
    entry.set_Value(DependencyProperty::get_UnsetValue());

    pUIElement->GetRawValue(dp, pMetadata, &entry);
    pObjValue = entry.get_Value();

    return (pObjValue == DependencyProperty::get_UnsetValue() || Object::Equals(pMetadata->get_DefaultValue(), pObjValue));
}


TRefCountedPtr<Object> Helper::FindNameInTemplate(String* pName, DependencyObject* pTemplatedParent)
{
    UIElement* pUIElement = object_cast<UIElement>(pTemplatedParent);

    return pUIElement->get_TemplateInternal()->FindName(*pName, pTemplatedParent);
}

DependencyObject* Helper::GetTemplatedParent(DependencyObject* pObject)
{
    if(pObject == NULL || !pObject->IsType(GET_TYPE(UIElement)))
    {
        return pObject;
    }

    return ((UIElement*)pObject)->get_TemplatedParent();
}

DependencyObject* Helper::FindMentor(DependencyObject* pDO)
{
    if(pDO == NULL)
    {
        return pDO;
    }

    if(pDO->IsType(GET_TYPE(UIElement)))
    {
        return pDO;
    }

    return pDO->get_InheritanceContext();
}


};
