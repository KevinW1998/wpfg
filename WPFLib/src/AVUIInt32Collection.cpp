#include <AVUICommon.h>
#include <AVUIInt32Collection.h>
#include <AVUIInt32CollectionConverter.h>
#include <AVUITokenizerHelper.h>

REGISTER_ELEMENT_BEGIN(AVUI::Int32Collection)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_TYPE_CONVERTER(Int32CollectionConverter);
    REGISTER_INTERFACE(ICollection)
REGISTER_ELEMENT_END()

namespace AVUI
{

bool Int32CollectionConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr Int32CollectionConverter::ConvertFrom(const Object* pObjectFrom) const
{
    const String* pString = object_cast<String>(pObjectFrom);

    if(pString == NULL)
    {
        LibraryCriticalError();
    }

    TRefCountedPtr<Int32Collection> pCollection = object_allocate<Int32Collection>();

    TokenizerHelper helper(pString);

    while (helper.NextToken())
    {
        int num = Int::Parse(helper.GetCurrentToken());

        pCollection->AddItem(num);
    }
    return (Object*) pCollection;
}
bool Int32CollectionConverter::CanConvertTo(const Type& type) const
{
    return false;
}
ObjectPtr Int32CollectionConverter::ConvertTo(const Object* pObjectFrom, const Type& type) const
{
    LibraryCriticalError();
    return NULL;
}

};
