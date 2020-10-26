#include <AVUICommon.h>
#include <AVUIEnumClass.h>
#include <AVUIUIString.h>

namespace AVUI {

bool EnumClass::CanConvertFrom(const Type& type) const
{
    return (type == GET_TYPE(String));
}

ObjectPtr EnumClass::ConvertFrom(const Object* pObjectFrom) const
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

    const String* pStr = (const String*) pObjectFrom;

    unsigned int count = get_MappingCount();

    for(unsigned int i = 0; i < count; i++)
    {
        if(pStr->Equals(get_Mapping()[i].wzName))
        {
            return BoxValue(get_Mapping()[i].value);
        }
    }

    return NULL;
}



}; // namespace AVUI
