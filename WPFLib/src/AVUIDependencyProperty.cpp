#include <AVUICommon.h>
#include <AVUIDependencyProperty.h>
#include <AVUIDependencyObject.h>


REGISTER_ELEMENT(AVUI::Property)
REGISTER_ELEMENT(AVUI::DependencyProperty)
REGISTER_ELEMENT(AVUI::SimpleProperty)

namespace AVUI {

DependencyProperty::DependencyProperty(const PropertyName& propertyName, const Type& definingType, const Type& propertyType, 
                                         Object* pDefaultValue, DependencyPropertyMetadata* pPropertyMetadata) :
    Property(propertyName, definingType, propertyType, DependencyProperty::GetDependencyPropertyValue, 
             (pPropertyMetadata->get_Flags() & PropertyFlag::ReadOnly) ? NULL : DependencyProperty::SetDependencyPropertyValue
            ),
             m_pPropertyMetadata(pPropertyMetadata)
{
    pPropertyMetadata->set_DefaultValue(pDefaultValue);
    pPropertyMetadata->set_MetadataType(definingType);
}

SimpleProperty::SimpleProperty(const PropertyName& propertyName, const Type& definingType, const Type& propertyType,
                   bool isContentProperty, pfnGetValue getValue, pfnSetValue setValue) :
                   Property(propertyName, definingType, propertyType, getValue, setValue), 
                   m_isContentProperty(isContentProperty)
{
}

void DependencyProperty::SetDependencyPropertyValue(const Property* pProperty, Object* pObject, Object* pValue)
{
    object_cast<DependencyObject>(pObject)->SetValue(*(object_cast<DependencyProperty>(pProperty)), pValue);
}
ObjectPtr DependencyProperty::GetDependencyPropertyValue(const Property* pProperty, Object* pObject)
{
    return object_cast<DependencyObject>(pObject)->GetValue(*(object_cast<DependencyProperty>(pProperty)));
}

bool DependencyProperty::IsValidValue(Object* pValue) const
{
    if(pValue == NULL)
    {
        return true;
    }
    if(get_PropertyType().get_IsEnumType() && pValue->get_Type() == GET_TYPE(int))
    {
        return true;
    }

    if(pValue->get_Type().IsTypeOrBase(get_PropertyType()))
    {
        return true;
    }

    if(get_PropertyType() == GET_TYPE(Object) && pValue->get_Type().get_IsValueType())
    {
        return true;
    }

    return false;
}

DependencyPropertyMetadata* DependencyProperty::GetMetadata(const Type& metadataType) const
{
    if(m_pPropertyMetadata->m_pMetadataNext == NULL)
    {
        return m_pPropertyMetadata;
    }

    // otherwise, we need to walk this backwards..

    const Type* pTypeCur = &metadataType;

    while(pTypeCur != NULL)
    {
      DependencyPropertyMetadata* pMetadataCur = m_pPropertyMetadata;
      while(pMetadataCur != NULL)
      {
          if(pMetadataCur->get_MetadataType() == *pTypeCur)
          {
              return pMetadataCur;
          }
          pMetadataCur = pMetadataCur->m_pMetadataNext;
      }

      pTypeCur = pTypeCur->get_BaseType();
    }

    return get_DefaultMetadata();

}

const DependencyProperty& DependencyProperty::AddOwner(const Type& addingType, Object* pDefaultValue, DependencyPropertyMetadata* pPropertyMetadata) const
{
    pPropertyMetadata->set_DefaultValue(pDefaultValue);
    pPropertyMetadata->set_MetadataType(addingType);

    DependencyPropertyMetadata* pMetadataCur = m_pPropertyMetadata;
    while(pMetadataCur->m_pMetadataNext != NULL)
    {
        pMetadataCur = pMetadataCur->m_pMetadataNext;
    }

    pMetadataCur->m_pMetadataNext = pPropertyMetadata;

    return (*this);
}

void DependencyProperty::OverrideMetadata(const Type& addingType, Object* pNewDefaultValue, DependencyPropertyMetadata* pPropertyMetadata) const
{
    pPropertyMetadata->set_DefaultValue(pNewDefaultValue);
    pPropertyMetadata->set_MetadataType(addingType);

    DependencyPropertyMetadata* pMetadataCur = m_pPropertyMetadata;
    while(pMetadataCur->m_pMetadataNext != NULL)
    {
        pMetadataCur = pMetadataCur->m_pMetadataNext;
    }

    pMetadataCur->m_pMetadataNext = pPropertyMetadata;
}

void DependencyProperty::CleanupAllocatedData() const
{
    DependencyPropertyMetadata* pMetadataCur = m_pPropertyMetadata;
    while(pMetadataCur != NULL)
    {
        pMetadataCur->set_DefaultValue(NULL);
        pMetadataCur = pMetadataCur->m_pMetadataNext;
    }

}


void* GetPtrFromTypeHelper(void* p)
{
    return reinterpret_cast<AVUI::BoxedValueType*>(p)->GetContainedValueOffset();
}

}; // namespace AVUI
