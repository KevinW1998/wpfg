#include <AVUICommon.h>
#include <AVUITemplateBinding.h>
#include <AVUIDependencyObject.h>

REGISTER_ELEMENT_BEGIN(AVUI::TemplateBinding)

REGISTER_ELEMENT_END()

namespace AVUI {

TemplateBinding::TemplateBinding() : m_pSourceProperty(NULL), m_isAttached(false), m_pTargetProperty(NULL) { }
TemplateBinding::TemplateBinding(DependencyObject* pSource, const DependencyProperty* pProperty) : m_pSourceObject(pSource), m_pSourceProperty(pProperty), m_pTargetProperty(NULL)
{

}

TemplateBinding::~TemplateBinding() { }

ObjectPtr TemplateBinding::GetValue(DependencyObject* pDO, const DependencyProperty& dp)
{
    if(m_pSourceObject != NULL && m_pSourceProperty != NULL)
    {
        ObjectPtr pValue =  m_pSourceObject->GetValue(*m_pSourceProperty);

        if(pValue != NULL && !pValue->get_Type().IsTypeOrBase(dp.get_PropertyType()))
        {
            if(dp.get_PropertyType() == GET_TYPE(Object))
            {
                return pValue;
            }            
            else if(!dp.get_PropertyType().get_IsEnumType() || pValue->get_Type() != GET_TYPE(int))
            {
                return pValue->get_Type().get_TypeConverter()->ConvertTo(pValue, dp.get_PropertyType());
            }
        }

        return pValue;
    }

    return get_NoValue();
}

bool TemplateBinding::SetValue(DependencyObject* pDO, const DependencyProperty& dp, Object* pValue)
{
    return false;

    /*
    if(m_pSourceObject != NULL && m_pSourceProperty != NULL)
    {
        m_pSourceObject->SetValue(dp, pValue);
        return true;
    }

    return false;
    */
}

void TemplateBinding::OnAttach(DependencyObject* pDO, const DependencyProperty& dp)
{
    MarkAttached();

    m_pTargetProperty = &dp;
    m_pTargetObject = pDO;

}

void TemplateBinding::MarkAttached()
{
    if(m_isAttached)
    {
        LibraryCriticalError();
    }

    m_isAttached = true;
}

void TemplateBinding::OnDetach(DependencyObject* pDO, const DependencyProperty& dp)
{
    if(!m_isAttached)
    {
        LibraryCriticalError();
    }

    m_isAttached = false;
    m_pSourceObject = NULL;
    m_pSourceProperty = NULL;
    m_pTargetProperty = NULL;
    m_pTargetObject = NULL;
}


TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > TemplateBinding::GetSources()
{
    TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencySource> > > pDependencyList = object_allocate<ItemStructList<TRefCountedPtr<DependencySource> > >();

    pDependencyList->AddItem(object_allocate<DependencySource>(m_pSourceObject, m_pSourceProperty));

    return pDependencyList;
}

void TemplateBinding::OnPropertyInvalidation(DependencyObject* pDO, const DependencyPropertyChangedEventArgs* pArgs)
{
}


}; // namespace AVUI
