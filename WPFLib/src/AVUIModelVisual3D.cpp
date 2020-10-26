#include <AVUICommon.h>
#include <AVUIModelVisual3D.h>
#include <AVUIModel3D.h>
#include <AVUIMatrix3D.h>
#include <AVUITransform3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::ModelVisual3D)

    REGISTER_DEFAULT_FACTORY();
    REGISTER_PROPERTY(Children)
    REGISTER_PROPERTY(Content)
    REGISTER_PROPERTY(Transform)
REGISTER_ELEMENT_END()

namespace AVUI {


void ModelVisual3D::Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix)
{
    Model3D* pModel = get_Content();

    Matrix3D localMatrix = parentMatrix;

    Transform3D* pTransform = get_Transform();
    
    if(pTransform != NULL)
    {
        localMatrix = pTransform->get_Value() * localMatrix;
    }

    if(pModel != NULL)
    {
        pModel->Render3D(pRenderer, localMatrix);
    }    

    for(int idx = 0; idx < m_children.get_Count(); idx++)
    {
        m_children[idx]->Render3D(pRenderer, localMatrix);
    }

}

void ModelVisual3D::RenderDependencies3D(Renderer* pRenderer)
{
    Model3D* pModel = get_Content();

    if(pModel != NULL)
    {
        pModel->RenderDependencies3D(pRenderer);
    }    

    for(int idx = 0; idx < m_children.get_Count(); idx++)
    {
        m_children[idx]->RenderDependencies3D(pRenderer);
    }
}


}; // namespace AVUI
