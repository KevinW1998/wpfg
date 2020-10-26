#include <AVUICommon.h>

#include <AVUITypeConverter.h>
#include <AVUIBuiltinTypes.h>
#include <AVUIBuiltinTypeConverters.h>
#include <AVUIStringUtil.h>

#pragma warning(disable:4723)

namespace AVUI {

ObjectPtr PointTypeConverter::ConvertFromString(const String* pString) const
{
    float x, y;
    if(StringUtil::Scan(pString->get_Ptr(), L"%f, %f", &x, &y) != 2)
    {
        LibraryCriticalError();
    }
    return BoxValue(Point(x, y));
}

ObjectPtr PointTypeConverter::ConvertToString(const Point& value) const
{
    return NULL;
}

ObjectPtr RectTypeConverter::ConvertFromString(const String* pString) const
{
    float x, y, width, height;
    if(StringUtil::Scan(pString->get_Ptr(), L"%f, %f, %f, %f", &x, &y, &width, &height) != 4)
    {
        LibraryCriticalError();
    }
    return BoxValue(Rect(x, y, width, height));
}

ObjectPtr RectTypeConverter::ConvertToString(const Rect& value) const
{
    WCHAR wzBuf[80];

    StringUtil::Print(wzBuf, sizeof(wzBuf) / sizeof(wzBuf[0]), L"%f, %f, %f, %f", value.get_Left(), value.get_Top(), value.get_Width(), value.get_Height());

    return (Object*) object_allocate<String>(wzBuf);
}

ObjectPtr BoolTypeConverter::ConvertFromString(const String* pString) const
{
    if(!StringUtil::CompareCaseInsensitive(pString->get_Ptr(), L"true"))
    {
        return BoxValue(true);
    }

    if(!StringUtil::CompareCaseInsensitive(pString->get_Ptr(), L"false"))
    {
        return BoxValue(false);
    }

    LibraryCriticalError();

    return NULL;
}

ObjectPtr BoolTypeConverter::ConvertToString(const bool& value) const
{
    if(value)
    {
        return (Object*) object_allocate<String>(L"true");
    }
    else
    {
        return (Object*) object_allocate<String>(L"false");
    }
}

ObjectPtr IntTypeConverter::ConvertFromString(const String* pString) const
{
    int value;
    if(StringUtil::Scan(pString->get_Ptr(), L"%d", &value) != 1)
    {
        LibraryCriticalError();
    }

    return BoxValue(value);
}

ObjectPtr IntTypeConverter::ConvertToString(const int& value) const
{
    WCHAR wzBuf[80];

    StringUtil::Print(wzBuf, sizeof(wzBuf) / sizeof(wzBuf[0]), L"%d", value);

    return (Object*) object_allocate<String>(wzBuf);
}

ObjectPtr FloatTypeConverter::ConvertFromString(const String* pString) const
{
    float value;
    if(StringUtil::Scan(pString->get_Ptr(), L"%f", &value) != 1)
    {
        LibraryCriticalError();
    }

    return BoxValue(value);
}

ObjectPtr FloatTypeConverter::ConvertToString(const float& value) const
{
    WCHAR wzBuf[80];

    StringUtil::Print(wzBuf, sizeof(wzBuf) / sizeof(wzBuf[0]), L"%.2f", value);

    return (Object*) object_allocate<String>(wzBuf);
}

ObjectPtr DoubleTypeConverter::ConvertFromString(const String* pString) const
{
    double value;
    if(StringUtil::Scan(pString->get_Ptr(), L"%lf", &value) != 1)
    {
        LibraryCriticalError();
    }

    return BoxValue(value);
}

ObjectPtr DoubleTypeConverter::ConvertToString(const double& value) const
{
    WCHAR wzBuf[80];

    StringUtil::Print(wzBuf, sizeof(wzBuf) / sizeof(wzBuf[0]), L"%.2lf", value);

    return (Object*) object_allocate<String>(wzBuf);
}

ObjectPtr UnsignedIntTypeConverter::ConvertFromString(const String* pString) const
{
    unsigned int value;

    if(StringUtil::Scan(pString->get_Ptr(), L"%u", &value) != 1)
    {
        LibraryCriticalError();
    }

    return BoxValue(value);
}

ObjectPtr UnsignedIntTypeConverter::ConvertToString(const unsigned int& value) const
{
    WCHAR wzBuf[80];

    StringUtil::Print(wzBuf, sizeof(wzBuf) / sizeof(wzBuf[0]), L"%ud", value);

    return (Object*) object_allocate<String>(wzBuf);
}


bool AnsiStringTypeConverter::CanConvertFrom(const Type& type) const
{
    return false;
}

ObjectPtr AnsiStringTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    return NULL;
}

bool AnsiStringTypeConverter::CanConvertTo(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr AnsiStringTypeConverter::ConvertTo(const Object* pObjectFrom, const Type& type) const
{
    return (Object*) object_allocate<String>(UnboxValue<const char*>(pObjectFrom));
}

bool UnicodeStringTypeConverter::CanConvertFrom(const Type& type) const
{
    return false;
}

ObjectPtr UnicodeStringTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    return NULL;
}

bool UnicodeStringTypeConverter::CanConvertTo(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr UnicodeStringTypeConverter::ConvertTo(const Object* pObjectFrom, const Type& type) const
{
    return (Object*) object_allocate<String>(UnboxValue<const WCHAR*>(pObjectFrom));
}




}; // end namespace AVUI
