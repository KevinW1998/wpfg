#include <AVUICommon.h>
#include <AVUIModel3DGroup.h>

REGISTER_ELEMENT_BEGIN(AVUI::Model3DGroup)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Children)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::Model3DCollection)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_INTERFACE(ICollection)
REGISTER_ELEMENT_END()


namespace AVUI {


void Model3DGroup::Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix)
{
    Model3DCollection* pChildren = get_Children();

    if(pChildren == NULL || pChildren->get_Count() == 0)
    {
        return;
    }


    for(int idx = 0; idx < pChildren->get_Count(); idx++)
    {
        (*pChildren)[idx]->Render3D(pRenderer, parentMatrix);
    }
}


void Model3DGroup::RenderDependencies3D(Renderer* pRenderer)
{
    Model3DCollection* pChildren = get_Children();

    if(pChildren == NULL || pChildren->get_Count() == 0)
    {
        return;
    }


    for(int idx = 0; idx < pChildren->get_Count(); idx++)
    {
        (*pChildren)[idx]->RenderDependencies3D(pRenderer);
    }
}


}; // namespace AVUI
 
