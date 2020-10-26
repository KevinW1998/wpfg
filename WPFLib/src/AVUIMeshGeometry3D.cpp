#include <AVUICommon.h>
#include <AVUIMeshGeometry3D.h>
#include <AVUIRenderer.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIPoint3DCollection.h>
#include <AVUIInt32Collection.h>
#include <AVUIPointCollection.h>
#include <AVUIApplication.h>

REGISTER_ELEMENT_BEGIN(AVUI::MeshGeometry3D)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Positions);
    REGISTER_PROPERTY(Normals);
    REGISTER_PROPERTY(TextureCoordinates);
    REGISTER_PROPERTY(TriangleIndices);
REGISTER_ELEMENT_END()


namespace AVUI
{

MeshGeometry3D::MeshGeometry3D() : m_pClientGeometry(NULL)
{
    Application::get_Current()->add_RenderDeviceLost(EventHandler::Create(this, &MeshGeometry3D::OnRenderDeviceLost));
}

MeshGeometry3D::~MeshGeometry3D()
{
    Application::get_Current()->remove_RenderDeviceLost(EventHandler::Create(this, &MeshGeometry3D::OnRenderDeviceLost));

    if(m_pClientGeometry != NULL)
    {
        m_pClientGeometry->Destroy();
        m_pClientGeometry = NULL;
    }
}

void MeshGeometry3D::OnRenderDeviceLost(Object* pObject, EventArgs* pEventArgs)
{
    if(m_pClientGeometry != NULL)
    {
        m_pClientGeometry->Destroy();
        m_pClientGeometry = NULL;
    }
}


IClientRenderGeometry* MeshGeometry3D::GetRenderGeometry(Renderer* pRenderer)
{
    if(m_pClientGeometry == NULL)
    {
        m_pClientGeometry = pRenderer->get_ClientRendererInterface()->CreateGeometry();

        Point3DCollection* pPositions = get_Positions();
        PointCollection* pTextureCoordinates = get_TextureCoordinates();
        Int32Collection* pIndices = get_TriangleIndices();

        for(int idx = 0; idx < pPositions->get_Count(); idx++)
        {
            const Point3D& pt = (*pPositions)[idx];
            Point textureCoordinate;

            if(pTextureCoordinates != NULL && pTextureCoordinates->get_Count() > 0)
            {
                textureCoordinate = (*pTextureCoordinates)[idx];
            }

            m_pClientGeometry->AddVertex3D(pt.get_X(), pt.get_Y(), pt.get_Z(), textureCoordinate.get_X(), textureCoordinate.get_Y());
        }

        for(int idx = 0; idx < pIndices->get_Count(); idx+=3)
        {
            m_pClientGeometry->AddIndexTriplet((*pIndices)[idx], 
                                               (*pIndices)[idx + 1], 
                                               (*pIndices)[idx + 2]);
        }

        m_pClientGeometry->Seal();
    }

    return m_pClientGeometry;
}


}

