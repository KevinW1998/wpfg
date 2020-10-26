#include <AVUICommon.h>
#include <AVUIValueConverter.h>

REGISTER_ELEMENT_BEGIN(AVUI::DefaultValueConverter)
REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::ObjectSourceConverter)
    REGISTER_INTERFACE(IValueConverter)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::ObjectTargetConverter)
    REGISTER_INTERFACE(IValueConverter)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::SourceDefaultValueConverter)
    REGISTER_INTERFACE(IValueConverter)
REGISTER_ELEMENT_END()


REGISTER_ELEMENT_BEGIN(AVUI::TargetDefaultValueConverter)
    REGISTER_INTERFACE(IValueConverter)
REGISTER_ELEMENT_END()


namespace AVUI
{


DefaultValueConverter::DefaultValueConverter(const TypeConverter* pTypeConverter, const Type* pSourceType, const Type* pTargetType, bool shouldConvertFrom, bool shouldConvertTo, DataBindEngine* pEngine)
{
    m_pTypeConverter = pTypeConverter;
    m_pSourceType = pSourceType;
    m_pTargetType = pTargetType;
    m_shouldConvertFrom = shouldConvertFrom;
    m_shouldConvertTo = shouldConvertTo;
    m_pEngine = pEngine;
}

TRefCountedPtr<Object> DefaultValueConverter::ConvertFrom(Object* pObject, const Type* pDestinationType, DependencyObject* pTargetElement)
{
    return ConvertHelper(pObject, pDestinationType, pTargetElement, false);
}

TRefCountedPtr<Object> DefaultValueConverter::ConvertTo(Object* pObject, const Type* pDestinationType, DependencyObject* pTargetElement)
{
    return ConvertHelper(pObject, pDestinationType, pTargetElement, true);
}


TRefCountedPtr<Object> DefaultValueConverter::ConvertHelper(Object* pObject, const Type* pDestinationType, DependencyObject* pTargetElement, bool isForward)
{
    TRefCountedPtr<Object> pUnsetValue = DependencyProperty::get_UnsetValue();
    bool flag = isForward ? !m_shouldConvertTo : !m_shouldConvertFrom;

    if (!flag)
    {
        if (isForward)
        {
            pUnsetValue = m_pTypeConverter->ConvertTo(pObject, *pDestinationType);
        }
        else
        {
            pUnsetValue = m_pTypeConverter->ConvertFrom(pObject);
        }
    }
    if (flag && (((pObject != NULL) && pDestinationType->IsAssignableFrom(pObject->get_Type())) || (pObject == NULL && !pDestinationType->get_IsValueType())))
    {
        pUnsetValue = pObject;
        flag = false;
    }

    return pUnsetValue;
}

TRefCountedPtr<Object> DefaultValueConverter::Create(const Type* pSourceType, const Type* pTargetType, bool targetToSource, DataBindEngine* pEngine)
{
    bool flag3 = false;
    bool flag4 = false;

    if (pSourceType == pTargetType)
    {
        return get_ValueConverterNotNeeded();
    }
    if (pTargetType == &GET_TYPE(Object))
    {
        return (Object*)object_allocate<ObjectTargetConverter>(pSourceType, pEngine);
    }
    if (pSourceType == &GET_TYPE(Object))
    {
        return (Object*)object_allocate<ObjectSourceConverter>(pTargetType, pEngine);
    }

    /*
    if (SystemConvertConverter.CanConvert(sourceType, targetType))
    {
        return new SystemConvertConverter(sourceType, targetType);
    }
    Type underlyingType = Nullable.GetUnderlyingType(sourceType);
    if (underlyingType != null)
    {
        sourceType = underlyingType;
        flag3 = true;
    }
    underlyingType = Nullable.GetUnderlyingType(targetType);
    if (underlyingType != null)
    {
        targetType = underlyingType;
        flag4 = true;
    }
    if (flag3 || flag4)
    {
        return Create(sourceType, targetType, targetToSource, engine);
    }
    */

    const TypeConverter* pTypeConverter = pSourceType->get_TypeConverter();
    bool shouldConvertTo = (pTypeConverter != NULL) ? pTypeConverter->CanConvertTo(*pTargetType) : false;
    bool shouldConvertFrom = (pTypeConverter != NULL) ? pTypeConverter->CanConvertFrom(*pTargetType) : false;
    if ((shouldConvertTo || pTargetType->IsAssignableFrom(*pSourceType)) && (!targetToSource || shouldConvertFrom || pSourceType->IsAssignableFrom(*pTargetType)))
    {
        return (Object*)object_allocate<SourceDefaultValueConverter>(pTypeConverter, pSourceType, pTargetType, targetToSource && shouldConvertFrom, shouldConvertTo, pEngine);
    }

    pTypeConverter = pTargetType->get_TypeConverter();
    shouldConvertTo = (pTypeConverter != NULL) ? pTypeConverter->CanConvertTo(*pSourceType) : false;
    shouldConvertFrom = (pTypeConverter != NULL) ? pTypeConverter->CanConvertFrom(*pSourceType) : false;
    if ((shouldConvertFrom || pTargetType->IsAssignableFrom(*pSourceType)) && (!targetToSource || shouldConvertTo || pSourceType->IsAssignableFrom(*pTargetType)))
    {
        return (Object*)object_allocate<TargetDefaultValueConverter>(pTypeConverter, pSourceType, pTargetType, shouldConvertFrom, targetToSource && shouldConvertTo, pEngine);
    }

    /*
    if (typeof(IListSource).IsAssignableFrom(sourceType) && targetType.IsAssignableFrom(typeof(IList)))
    {
        return new ListSourceConverter();
    }
    if (!sourceType.IsInterface && !targetType.IsInterface)
    {
        return null;
    }
    return new InterfaceConverter(sourceType, targetType);
    */

    return NULL;
}










ObjectSourceConverter::ObjectSourceConverter(const Type* pTargetType, DataBindEngine* pEngine) : 
                       DefaultValueConverter(pTargetType->get_TypeConverter(), &GET_TYPE(Object), pTargetType, true, false, pEngine) { }

TRefCountedPtr<Object> ObjectSourceConverter::Convert(Object* pValue, const Type* pTargetType, Object* pParameter)
{
    if (pValue != NULL && m_pTargetType->IsAssignableFrom(pValue->get_Type()))
    {
        return pValue;
    }
    if (pValue == NULL && !m_pTargetType->get_IsValueType())
    {
        return pValue;
    }
    if (m_pTargetType == &GET_TYPE(String))
    {
        return (Object*)object_allocate<String>(pTargetType->ToString());
    }
    return ConvertFrom(pValue, m_pTargetType, object_cast<DependencyObject>(pParameter));
}

TRefCountedPtr<Object> ObjectSourceConverter::ConvertBack(Object* pValue, const Type* pTargetType, Object* pParameter)
{
    return pValue;
}



ObjectTargetConverter::ObjectTargetConverter(const Type* pSourceType, DataBindEngine* pEngine) : 
                       DefaultValueConverter(pSourceType->get_TypeConverter(), pSourceType, &GET_TYPE(Object), true, false, pEngine) { }


TRefCountedPtr<Object> ObjectTargetConverter::Convert(Object* pValue, const Type* pTargetType, Object* pParameter)
{
    return pValue;
}


TRefCountedPtr<Object> ObjectTargetConverter::ConvertBack(Object* pValue, const Type* pTargetType, Object* pParameter)
{
    if (pValue != NULL && m_pTargetType->IsAssignableFrom(pValue->get_Type()))
    {
        return pValue;
    }
    if (pValue == NULL && !m_pTargetType->get_IsValueType())
    {
        return pValue;
    }
    if (m_pTargetType == &GET_TYPE(String))
    {
        return (Object*)object_allocate<String>(pTargetType->ToString());
    }
    return ConvertFrom(pValue, m_pTargetType, object_cast<DependencyObject>(pParameter));
}



SourceDefaultValueConverter::SourceDefaultValueConverter(const TypeConverter* pTypeConverter, const Type* pSourceType, const Type* pTargetType, bool shouldConvertFrom, bool shouldConvertTo, DataBindEngine* pEngine) :
                       DefaultValueConverter(pTypeConverter, pSourceType, pTargetType, shouldConvertFrom, shouldConvertTo, pEngine) { }


TRefCountedPtr<Object> SourceDefaultValueConverter::Convert(Object *pValue, const Type *pTargetType, Object *pParameter)
{
    return ConvertTo(pValue, m_pTargetType, object_cast<DependencyObject>(pParameter));

}

TRefCountedPtr<Object> SourceDefaultValueConverter::ConvertBack(Object *pValue, const Type *pTargetType, Object *pParameter)
{
    return ConvertFrom(pValue, m_pSourceType, object_cast<DependencyObject>(pParameter));
}

TargetDefaultValueConverter::TargetDefaultValueConverter(const TypeConverter* pTypeConverter, const Type* pSourceType, const Type* pTargetType, bool shouldConvertFrom, bool shouldConvertTo, DataBindEngine* pEngine) :
                       DefaultValueConverter(pTypeConverter, pSourceType, pTargetType, shouldConvertFrom, shouldConvertTo, pEngine) { }

 

TRefCountedPtr<Object> TargetDefaultValueConverter::Convert(Object *pValue, const Type *pTargetType, Object *pParameter)
{
    return ConvertFrom(pValue, m_pTargetType, object_cast<DependencyObject>(pParameter));

}

TRefCountedPtr<Object> TargetDefaultValueConverter::ConvertBack(Object *pValue, const Type *pTargetType, Object *pParameter)
{
    return ConvertTo(pValue, m_pSourceType, object_cast<DependencyObject>(pParameter));
}



}


