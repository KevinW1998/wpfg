#include <AVUICommon.h>
#include <AVUIUncommonField.h>
#include <AVUIDependencyObject.h>

namespace AVUI {

void UncommonFieldHelper::ClearValueHelper(DependencyObject* pDO, int globalIndex)
{
    EntryIndex index = pDO->LookupEntry(globalIndex);

    if(index.Found)
    {
        pDO->RemoveEntry(index.Index);
    }
}

Object* UncommonFieldHelper::GetValueHelper(DependencyObject* pDO, int globalIndex)
{
    Object* pObject = DependencyProperty::get_UnsetValue();

    EntryIndex index = pDO->LookupEntry(globalIndex);

    if(index.Found)
    {
        pObject = pDO->get_EffectiveValues()[index.Index].get_LocalValue();
    }

    return pObject;
}


bool UncommonFieldHelper::SetValueHelper(DependencyObject* pDO, Object* pValue, Object* pDefaultValue, int globalIndex)
{
    EntryIndex index = pDO->LookupEntry(globalIndex);
    if(pValue != pDefaultValue)
    {
        pDO->SetEffectiveValue(index, NULL, globalIndex, NULL, pValue, BaseValueSource::Local);
        return true;
    }
    else
    {
        ClearValueHelper(pDO, globalIndex);
        return false;
    }
}





}; // namespace AVUI

