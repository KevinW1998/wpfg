#pragma once

#include <AVUITypeConverter.h>

namespace AVUI {

class TypeTypeConverter : public TypeConverter
{
protected:
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const;
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const;
};

};