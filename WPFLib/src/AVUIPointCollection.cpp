#include <AVUICommon.h>
#include <AVUIPointCollection.h>
#include <AVUIPointCollectionConverter.h>
#include <AVUITokenizerHelper.h>

REGISTER_ELEMENT_BEGIN(AVUI::PointCollection)

    REGISTER_INTERFACE(ICollection)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_TYPE_CONVERTER(PointCollectionConverter);
REGISTER_ELEMENT_END()


namespace AVUI
{

bool PointCollectionConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr PointCollectionConverter::ConvertFrom(const Object* pObjectFrom) const
{
    const String* pString = object_cast<String>(pObjectFrom);

    if(pString == NULL)
    {
        LibraryCriticalError();
    }

    TRefCountedPtr<PointCollection> pCollection = object_allocate<PointCollection>();

    TokenizerHelper helper(pString);

    while (helper.NextToken())
    {
        float x = Float::Parse(helper.GetCurrentToken());
        float y = Float::Parse(helper.NextTokenRequired());

        Point pointd(x, y);

        pCollection->AddItem(pointd);
    }
    return (Object*) pCollection;
}

bool PointCollectionConverter::CanConvertTo(const Type& type) const
{
    return false;
}
ObjectPtr PointCollectionConverter::ConvertTo(const Object* pObjectFrom, const Type& type) const
{
    LibraryCriticalError();
    return NULL;
}

}
