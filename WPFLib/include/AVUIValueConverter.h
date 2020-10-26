#pragma once

namespace AVUI {

class DataBindEngine;

class IValueConverter
{
public:
    virtual TRefCountedPtr<Object> Convert(Object* pValue, const Type* pTargetType, Object* pParameter) = 0;
    virtual TRefCountedPtr<Object> ConvertBack(Object* pValue, const Type* pTargetType, Object* pParameter) = 0;
};

class DefaultValueConverter : public Object
{
public:
    DECLARE_ELEMENT(DefaultValueConverter, Object);

    DefaultValueConverter(const TypeConverter* pTypeConverter, const Type* pSourceType, const Type* pTargetType, bool shouldConvertFrom, bool shouldConvertTo, DataBindEngine* pEngine);
    TRefCountedPtr<Object> ConvertFrom(Object* pObject, const Type* pDestinationType, DependencyObject* pTargetElement);
    TRefCountedPtr<Object> ConvertTo(Object* pObject, const Type* pDestinationType, DependencyObject* pTargetElement);

    static TRefCountedPtr<Object> Create(const Type* pSourceType, const Type* pTargetType, bool targetToSource, DataBindEngine* pEngine);

    static Object* get_ValueConverterNotNeeded() { static Object ValueConverterNotNeeded; return &ValueConverterNotNeeded; }

    DataBindEngine* get_Engine() { return m_pEngine; }

protected:
    const Type* m_pSourceType;
    const Type* m_pTargetType;

private:
    TRefCountedPtr<Object> ConvertHelper(Object* pObject, const Type* pDestinationType, DependencyObject* pTargetElement, bool isForward);

    DataBindEngine* m_pEngine;
    bool m_shouldConvertFrom;
    bool m_shouldConvertTo;
    const TypeConverter* m_pTypeConverter;

};

class ObjectSourceConverter : public DefaultValueConverter, public IValueConverter
{
public:
    DECLARE_ELEMENT(ObjectSourceConverter, DefaultValueConverter);

    ObjectSourceConverter(const Type* pTargetType, DataBindEngine* pEngine);

    virtual TRefCountedPtr<Object> Convert(Object* pValue, const Type* pTargetType, Object* pParameter);
    virtual TRefCountedPtr<Object> ConvertBack(Object* pValue, const Type* pTargetType, Object* pParameter);
};


class ObjectTargetConverter : public DefaultValueConverter, public IValueConverter
{
public:
    DECLARE_ELEMENT(ObjectTargetConverter, DefaultValueConverter);

    ObjectTargetConverter(const Type* pSourceType, DataBindEngine* pEngine);

    virtual TRefCountedPtr<Object> Convert(Object* pValue, const Type* pTargetType, Object* pParameter);
    virtual TRefCountedPtr<Object> ConvertBack(Object* pValue, const Type* pTargetType, Object* pParameter);
};


class SourceDefaultValueConverter : public DefaultValueConverter, public IValueConverter
{
public:
    DECLARE_ELEMENT(SourceDefaultValueConverter, DefaultValueConverter);

    SourceDefaultValueConverter(const TypeConverter* pTypeConverter, const Type* pSourceType, const Type* pTargetType, bool shouldConvertFrom, bool shouldConvertTo, DataBindEngine* pEngine);
    virtual TRefCountedPtr<Object> Convert(Object *pValue, const Type *pTargetType, Object *pParameter);
    virtual TRefCountedPtr<Object> ConvertBack(Object *pValue, const Type *pTargetType, Object *pParameter);
};


class TargetDefaultValueConverter : public DefaultValueConverter, public IValueConverter
{
public:
    DECLARE_ELEMENT(TargetDefaultValueConverter, DefaultValueConverter);

    TargetDefaultValueConverter(const TypeConverter* pTypeConverter, const Type* pSourceType, const Type* pTargetType, bool shouldConvertFrom, bool shouldConvertTo, DataBindEngine* pEngine);
    virtual TRefCountedPtr<Object> Convert(Object *pValue, const Type *pTargetType, Object *pParameter);
    virtual TRefCountedPtr<Object> ConvertBack(Object *pValue, const Type *pTargetType, Object *pParameter);
};


};
