#include <AVUICommon.h>
#include <AVUIBrush.h>
#include <AVUISolidColorBrush.h>

REGISTER_ELEMENT_BEGIN(AVUI::Brush)

    REGISTER_TYPE_CONVERTER(BrushTypeConverter)
    REGISTER_PROPERTY(Transform)
    REGISTER_PROPERTY(RelativeTransform)
REGISTER_ELEMENT_END()

namespace AVUI {

void Brush::OnTransformChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    if(!e.IsSubPropertyChange)
    {
        ((Brush*) pDO)->m_pTransform = (Transform*) e.pNewValue;
    }
}

void Brush::OnRelativeTransformChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    if(!e.IsSubPropertyChange)
    {
        ((Brush*) pDO)->m_pRelativeTransform = (Transform*) e.pNewValue;
    }
}


bool BrushTypeConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}
ObjectPtr BrushTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    if(pObjectFrom == NULL)
    {
        LibraryCriticalError();
        return NULL;
    }

    if(!CanConvertFrom(pObjectFrom->get_Type()))
    {
        LibraryCriticalError();
        return NULL;
    }


    const TypeConverter* pTypeConverter = GET_TYPE(Color).get_TypeConverter();

    ObjectPtr pColorValue = pTypeConverter->ConvertFrom(pObjectFrom);

    TRefCountedPtr<SolidColorBrush> pBrush(new SolidColorBrush(), false);
    pBrush->set_Color(UnboxValue<Color>(pColorValue));

    return static_cast<Object*>(pBrush);
}

}; // namespace AVUI
