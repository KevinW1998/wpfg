#include <AVUICommon.h>
#include <AVUIViewport3D.h>
#include <AVUICamera.h>
#include <AVUIMatrix3D.h>
#include <AVUIRenderer.h>
#include <AVUIModelVisual3D.h>
#include <AVUIIClientRendererInterface.h>

REGISTER_ELEMENT_BEGIN(AVUI::Viewport3D)

    REGISTER_DEFAULT_FACTORY();
    REGISTER_PROPERTY(Camera);
    REGISTER_PROPERTY(Children);
REGISTER_ELEMENT_END()

namespace AVUI {

Viewport3D::Viewport3D() : m_pCamera(NULL)
{ 
    set_IsViewport3D(true); 
    m_pRootVisual = object_allocate<ModelVisual3D>();
    m_pChildren = object_allocate<Visual3DCollection>(this);
}


void Viewport3D::Render3D(Renderer* pRenderer, const Matrix& localMatrix, const Rect& localClipRect)
{
    Camera* pCamera = get_Camera();

    if(pCamera == NULL)
    {
        return;
    }

    Size renderSize = get_RenderSize();
    Matrix3D viewMatrix = pCamera->GetViewMatrix();
    Matrix3D projectionMatrix = pCamera->GetProjectionMatrix(renderSize.get_Width() / renderSize.get_Height());

    pRenderer->get_ClientRendererInterface()->Begin3DRender();

    for(int idx = 0; idx < get_Children()->get_Count(); idx++)
    {
        Matrix3D currentMatrix(localMatrix.GetM11(),    localMatrix.GetM12(),        0.0f, 0.0f,
                               localMatrix.GetM21(),    localMatrix.GetM22(),        0.0f, 0.0f,
                               0.0f,                    0.0f,                        1.0f, 0.0f,
                               localMatrix.get_OffsetX(), localMatrix.get_OffsetY(), 0.0f, 1.0f);

        Matrix3D localViewportTransform = GetHomogeneousToViewportTransform3D(Rect(Point(), renderSize));

        (*get_Children())[idx]->Render3D(pRenderer, viewMatrix * projectionMatrix * localViewportTransform * currentMatrix);
    }

    m_pRootVisual->Render3D(pRenderer, Matrix3D::Multiply(viewMatrix, projectionMatrix));

    pRenderer->get_ClientRendererInterface()->End3DRender();
}

void Viewport3D::RenderDependencies3D(Renderer* pRenderer)
{

    for(int idx = 0; idx < get_Children()->get_Count(); idx++)
    {
        (*get_Children())[idx]->RenderDependencies3D(pRenderer);
    }

    m_pRootVisual->RenderDependencies3D(pRenderer);
}


Matrix3D Viewport3D::GetHomogeneousToViewportTransform3D(const Rect& viewport)
{
    float num2 = viewport.get_Width() / 2;
    float num = viewport.get_Height() / 2;
    float offsetX = viewport.get_X() + num2;
    return Matrix3D(num2, 0.0f, 0.0f, 0.0f, 0.0f, -num, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, offsetX, viewport.get_Y() + num, 0.0f, 1.0f);
}

Size Viewport3D::ArrangeOverride(const Size& finalSize)
{
    return finalSize;
}

void Viewport3D::OnRender(DrawingContext& drawingContext)
{
    drawingContext.DrawRoundedRectangle(NULL, NULL, Rect(Point(), get_RenderSize()), 0.0f, 0.0f);
    set_VisualClip(Rect(Size(get_RenderSize())));
}


}; // namespace AVUI
