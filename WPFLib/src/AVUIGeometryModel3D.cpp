#include <AVUICommon.h>
#include <AVUIGeometryModel3D.h>
#include <AVUIGeometry3D.h>
#include <AVUIRenderer.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIEmissiveMaterial.h>
#include <AVUIDiffuseMaterial.h>
#include <AVUIBrush.h>
#include <AVUIMatrix3D.h>
#include <AVUITransform3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::GeometryModel3D)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Geometry);
    REGISTER_PROPERTY(Material);
    REGISTER_PROPERTY(BackMaterial);
REGISTER_ELEMENT_END()

namespace AVUI
{


void GeometryModel3D::Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix)
{
    Geometry3D* pGeometry = get_Geometry();

    if(pGeometry != NULL)
    {
        IClientRenderGeometry* pRenderGeometry = pGeometry->GetRenderGeometry(pRenderer);

        if(pRenderGeometry != NULL)
        {
            Matrix3D localMatrix = parentMatrix;

            Transform3D* pTransform = get_Transform();

            if(pTransform != NULL)
            {
                localMatrix = pTransform->get_Value() * localMatrix;
            }

            Material* pMaterial = get_Material();

            if(pMaterial == NULL)
            {
                return;
            }

            if(pMaterial->IsType(GET_TYPE(EmissiveMaterial)))
            {
                ((EmissiveMaterial*)get_Material())->get_Brush()->set_ActiveBrush(pRenderer);
            }
            else if(pMaterial->IsType(GET_TYPE(DiffuseMaterial)))
            {
                ((DiffuseMaterial*)get_Material())->get_Brush()->set_ActiveBrush(pRenderer);
            }

            pRenderer->get_ClientRendererInterface()->SetCurrent3DTransform(localMatrix);
            pRenderer->get_ClientRendererInterface()->RenderGeometry3D(pRenderGeometry);
        }
    }
}

void GeometryModel3D::RenderDependencies3D(Renderer* pRenderer)
{
    Material* pMaterial = get_Material();

    if(pMaterial == NULL)
    {
        return;
    }

    Brush* pBrush = NULL;

    if(pMaterial->IsType(GET_TYPE(EmissiveMaterial)))
    {
        pBrush = ((EmissiveMaterial*) pMaterial)->get_Brush();
    }
    else if(pMaterial->IsType(GET_TYPE(DiffuseMaterial)))
    {
        pBrush = ((DiffuseMaterial*) pMaterial)->get_Brush();
    }

    if(pBrush != NULL && pBrush->ContainsRenderDependencies())
    {
        pBrush->RenderDependencies(pRenderer);
    }
}


};


