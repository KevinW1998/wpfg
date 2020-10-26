#include <AVUICommon.h>
#include <AVUITypeTypeConverter.h>
#include <AVUITypeManager.h>

namespace AVUI {

bool TypeTypeConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr TypeTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    String* pStringFrom = (String*) pObjectFrom;

    int indexNS = pStringFrom->IndexOf(L':');
    if(indexNS == -1)
    {
        return TypeManager::FindType(*pStringFrom);
    }

    String strNamespaced(pStringFrom->get_Ptr() + indexNS + 1);

    return TypeManager::FindType(strNamespaced);
}

bool TypeTypeConverter::CanConvertTo(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr TypeTypeConverter::ConvertTo(const Object* pObjectFrom, const Type& type) const
{
    return (Object*)object_allocate<String>(pObjectFrom->ToString());
}




}; // end namespace AVUI
