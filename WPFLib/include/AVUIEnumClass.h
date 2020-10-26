#pragma once

#include <AVUITypeConverter.h>

namespace AVUI {

class EnumClass : public TypeConverter
{
public:
    ObjectPtr FromString(const char*) const;

protected:
    EnumClass(const Type& type) { };
    ~EnumClass() { };

    struct EnumStringMapping
    {
        int value;
        const WCHAR* wzName;
    };

    virtual const EnumClass::EnumStringMapping* get_Mapping() const = 0;
    virtual unsigned int get_MappingCount() const = 0;

    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const { return false; };
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { LibraryCriticalError(); return NULL; }
};


}; // namespace AVUI
