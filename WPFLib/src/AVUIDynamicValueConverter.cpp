#include <AVUICommon.h>
#include <AVUIDynamicValueConverter.h>
#include <AVUIDataBindEngine.h>

REGISTER_ELEMENT_BEGIN(AVUI::DynamicValueConverter)

    REGISTER_INTERFACE(IValueConverter)
REGISTER_ELEMENT_END()

namespace AVUI {

DynamicValueConverter::DynamicValueConverter(bool targetToSourceNeeded)
{
    m_pEngine = NULL;
    m_pSourceType = NULL;
    m_targetToSourceNeeded = targetToSourceNeeded;
    m_pTargetType = NULL;
    m_pValueConverterInterface = NULL;
}

DynamicValueConverter::DynamicValueConverter(bool targetToSourceNeeded, const Type* pSourceType, const Type* pTargetType)
{
    m_pEngine = NULL;
    m_pSourceType = NULL;
    m_targetToSourceNeeded = targetToSourceNeeded;
    m_pTargetType = NULL;
    m_pValueConverterInterface = NULL;

    EnsureConverter(pSourceType, pTargetType);
}

TRefCountedPtr<Object> DynamicValueConverter::Convert(Object* pValue, const Type* pTargetType)
{
    return Convert(pValue, pTargetType, NULL);
}

TRefCountedPtr<Object> DynamicValueConverter::Convert(Object* pValue, const Type* pTargetType, Object* pParameter)
{
    if (pValue == NULL)
    {
        return NULL;
    }

    const Type* pSourceType = &(pValue->get_Type());
    EnsureConverter(pSourceType, pTargetType);
    if (m_pValueConverter == NULL)
    {
        return NULL;
    }
    return m_pValueConverterInterface->Convert(pValue, pTargetType, pParameter);
}

TRefCountedPtr<Object> DynamicValueConverter::ConvertBack(Object* pValue, const Type* pSourceType, Object* pParameter)
{
    if (pValue == NULL)
    {
        return NULL;
    }

    const Type* pTargetType = &(pValue->get_Type());
    EnsureConverter(pSourceType, pTargetType);
    if (m_pValueConverter == NULL)
    {
        return NULL;
    }
    return m_pValueConverterInterface->ConvertBack(pValue, m_pSourceType, pParameter);
}

bool DynamicValueConverter::CanConvert(const Type* pSourceType, const Type* pTargetType)
{
    if (m_pSourceType != NULL && m_pTargetType != NULL && m_pSourceType == pSourceType)
    {
        return m_pTargetType == pTargetType;
    }
    return false;
}

void DynamicValueConverter::EnsureConverter(const Type* pSourceType, const Type* pTargetType)
{
    if (m_pValueConverter == NULL || !CanConvert(pSourceType, pTargetType))
    {
        if (m_pEngine == NULL)
        {
            m_pEngine = DataBindEngine::get_CurrentDataBindEngine();
        }

        m_pValueConverter = m_pEngine->GetDefaultValueConverter(pSourceType, pTargetType, m_targetToSourceNeeded);
        m_pSourceType = pSourceType;
        m_pTargetType = pTargetType;
        m_pValueConverterInterface = interface_cast<IValueConverter>(m_pValueConverter);
    }

}

}; // namespace AVUI


