#pragma once

#include <AVUITypeConverter.h>

namespace AVUI {

class PointTypeConverter : public StdValueTypeConverter<Point>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const Point& value) const;
};
class NullablePointTypeConverter : public NullableTypeConverter<Point> { };

class RectTypeConverter : public StdValueTypeConverter<Rect>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const Rect& value) const;
};

class BoolTypeConverter : public StdValueTypeConverter<bool>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const bool& value) const;
};
class NullableBoolTypeConverter : public NullableTypeConverter<bool> { };

class IntTypeConverter : public StdValueTypeConverter<int>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const int& value) const;
};

class NullableIntTypeConverter : public NullableTypeConverter<int> { };

class FloatTypeConverter : public StdValueTypeConverter<float>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const float& value) const;
};

class NullableFloatTypeConverter : public NullableTypeConverter<float> { };

class DoubleTypeConverter : public StdValueTypeConverter<double>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const double& value) const;
};

class UnsignedIntTypeConverter : public StdValueTypeConverter<unsigned int>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const unsigned int& value) const;
};

class AnsiStringTypeConverter : public TypeConverter
{
protected:
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const;
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const;
};

class UnicodeStringTypeConverter : public TypeConverter
{
protected:
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const;
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const;
};



};
