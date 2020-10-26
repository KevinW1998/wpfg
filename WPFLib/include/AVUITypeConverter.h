#pragma once

#include <AVUIType.h>
#include <AVUINullable.h>

namespace AVUI {

class Object;
class String;

class TypeConverter
{
public:
    virtual bool CanConvertFrom(const Type& type) const = 0;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const = 0;
    virtual bool CanConvertTo(const Type& type) const = 0;
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const = 0;
};



template<class T>
class StdValueTypeConverter : public TypeConverter
{
public:
    virtual bool CanConvertFrom(const Type& type) const
    {
        return type == GET_TYPE(String);
    }

    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const
    {
        if(!pObjectFrom)
        {
            LibraryCriticalError();
            return NULL;
        }

        if(!CanConvertFrom(pObjectFrom->get_Type()))
        {
            LibraryCriticalError();
            return NULL;
        }

        const String* pString = (const String*) pObjectFrom;

        return ConvertFromString(pString);
    }

    virtual bool CanConvertTo(const Type& type) const
    {
        return type == GET_TYPE(String);
    }

    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const
    {
        if(pObjectFrom == NULL)
        {
            LibraryCriticalError();
            return NULL;
        }

        if(!CanConvertTo(type))
        {
            return NULL;
        }

        T value = UnboxValue<T>(pObjectFrom);

        return ConvertToString(value);
    }

protected:

    virtual ObjectPtr ConvertFromString(const String* pString) const = 0;
    virtual ObjectPtr ConvertToString(const T& value) const = 0;
};


template <class T> 
class NullableTypeConverter : public TypeConverter
{
public:
    NullableTypeConverter() { }

    virtual bool CanConvertFrom(const Type& type) const { return get_Type((T*)0).get_TypeConverter()->CanConvertFrom(type); }
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const 
    { 
        if(pObjectFrom == NULL)
        {
            LibraryCriticalError();
        }

        if(!CanConvertFrom(pObjectFrom->get_Type()))
        {
            LibraryCriticalError();
            return NULL;
        }

        ObjectPtr objectValue = get_Type((T*)0).get_TypeConverter()->ConvertFrom(pObjectFrom);

        const T& value = UnboxValue<T>(objectValue);
        return BoxValue(Nullable<T>(value));
    }
    virtual bool CanConvertTo(const Type& type) const { return false; }
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { return NULL; }
};

}; // namespace AVUI
