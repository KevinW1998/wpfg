#include <AVUICommon.h>
#include <AVUIPoint3DCollection.h>
#include <AVUIPoint3DCollectionConverter.h>
#include <AVUITokenizerHelper.h>

REGISTER_ELEMENT_BEGIN(AVUI::Point3DCollection)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_TYPE_CONVERTER(Point3DCollectionConverter);
    REGISTER_INTERFACE(ICollection)
REGISTER_ELEMENT_END()

namespace AVUI
{

bool Point3DCollectionConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr Point3DCollectionConverter::ConvertFrom(const Object* pObjectFrom) const
{
    const String* pString = object_cast<String>(pObjectFrom);

    if(pString == NULL)
    {
        LibraryCriticalError();
    }

    TRefCountedPtr<Point3DCollection> pCollection = object_allocate<Point3DCollection>();

    TokenizerHelper helper(pString);

    while (helper.NextToken())
    {
        float x = Float::Parse(helper.GetCurrentToken());
        float y = Float::Parse(helper.NextTokenRequired());
        float z = Float::Parse(helper.NextTokenRequired());

        Point3D pointd(x, y, z);

        pCollection->AddItem(pointd);
    }
    return (Object*) pCollection;
}
bool Point3DCollectionConverter::CanConvertTo(const Type& type) const
{
    return false;
}
ObjectPtr Point3DCollectionConverter::ConvertTo(const Object* pObjectFrom, const Type& type) const
{
    LibraryCriticalError();
    return NULL;
}

}
