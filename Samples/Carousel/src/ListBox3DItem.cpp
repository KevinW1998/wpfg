#include <AVUICommon.h>
#include <AVUIDataTemplate.h>
#include <ListBox3DItem.h>

REGISTER_ELEMENT_BEGIN(ListBox3DItem)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(DataTemplate)
REGISTER_ELEMENT_END()


using namespace AVUI;


TRefCountedPtr<ModelVisual3D> ListBox3DItem::GenerateModel()
{
    DataTemplate* pTemplate = get_DataTemplate();


    return object_cast<ModelVisual3D>(pTemplate->CreateInstance(this));
}


