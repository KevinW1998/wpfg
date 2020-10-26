#pragma once

#include <AVUIValueConverter.h>

namespace AVUI {

class DynamicValueConverter : public Object, public IValueConverter
{
public:
    DynamicValueConverter(bool targetToSourceNeeded);
    DynamicValueConverter(bool targetToSourceNeeded, const Type* pSourceType, const Type* pTargetType);

    DECLARE_ELEMENT(DynamicValueConverter, Object);

    TRefCountedPtr<Object> Convert(Object* pValue, const Type* pTargetType);


    virtual TRefCountedPtr<Object> Convert(Object* pValue, const Type* pTargetType, Object* pParameter);
    virtual TRefCountedPtr<Object> ConvertBack(Object* pValue, const Type* pTargetType, Object* pParameter);

private:

    bool CanConvert(const Type* pSourceType, const Type* pTargetType);
    void EnsureConverter(const Type* pSourceType, const Type* pTargetType);

    IValueConverter* m_pValueConverterInterface;
    TRefCountedPtr<Object> m_pValueConverter;
    DataBindEngine* m_pEngine;
    const Type* m_pSourceType;
    bool m_targetToSourceNeeded;
    const Type* m_pTargetType;
};


};
