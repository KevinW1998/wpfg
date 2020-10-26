#include <AVUICommon.h>
#include <AVUIVisualBrush.h>
#include <AVUIRenderer.h>
#include <AVUIApplication.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIVisual.h>
#include <AVUIMediaElement.h>
#include <AVUIMatrix.h>

REGISTER_ELEMENT_BEGIN(AVUI::VisualBrush)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Visual);
REGISTER_ELEMENT_END()

namespace AVUI {

VisualBrush::VisualBrush() : m_pClientSubgraphCapture(NULL), m_isSimpleMediaBrush(false)
{
    Application::get_Current()->AddVisualBrush(this);
}

VisualBrush::~VisualBrush()
{
    if(m_pClientSubgraphCapture != NULL)
    {
        m_pClientSubgraphCapture->Destroy();
        m_pClientSubgraphCapture = NULL;
    }

    UpdateVisualBrushRootProperties(get_Visual(), NULL);

    Application::get_Current()->RemoveVisualBrush(this);
}

void VisualBrush::OnRenderFinished()
{
    if(m_pClientSubgraphCapture != NULL)
    {
        m_pClientSubgraphCapture->Destroy();
        m_pClientSubgraphCapture = NULL;
    }
}

void VisualBrush::set_ActiveBrush(Renderer* pRenderer)
{
    pRenderer->get_ClientRendererInterface()->set_CurrentBrush(this, m_pClientSubgraphCapture);
}

void VisualBrush::UpdateVisualBrushRootProperties(Visual* pOld, Visual* pNew)
{
    if(pOld != NULL)
    {
        pOld->set_IsVisualBrushRoot(Application::get_Current()->IsVisualBrushRoot(pOld, this));
    }

    if(pNew != NULL)
    {
        pNew->set_IsVisualBrushRoot(true);
    }
}

void VisualBrush::OnVisualChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((VisualBrush*)pDO)->m_isSimpleMediaBrush = false;

    ((VisualBrush*)pDO)->UpdateVisualBrushRootProperties((Visual*)e.pOldValue, (Visual*)e.pNewValue);

    if(e.pNewValue != NULL && e.pNewValue->IsType(GET_TYPE(UIElement)))
    {
        UIElement* pElement = (UIElement*) e.pNewValue;

        if(pElement->IsType(GET_TYPE(MediaElement)))
        {
            ((VisualBrush*)pDO)->m_isSimpleMediaBrush = true;
        }

        if(pElement->get_VisualParent() == NULL)
        {
            pElement->Measure(Size(Float::get_PositiveInfinity(), Float::get_PositiveInfinity()));
            pElement->Arrange(Rect(pElement->get_DesiredSize()));

            pElement->BuildGeometry(Application::get_Current()->get_GeometryBuilder());
        }
    }
}

void VisualBrush::RenderDependencies(Renderer* pRenderer)
{
    Visual* pVisual = (Visual*) get_Visual();

    if(pVisual != NULL && !m_pClientSubgraphCapture)
    {

        Rect rectSubgraph = pVisual->get_SubgraphBounds();;

        Rect localClipRect = Rect(Point(0.0f, 0.0f), Size(rectSubgraph.get_Width(), rectSubgraph.get_Height()));

        Matrix mtx = Matrix::CreateTranslation(-rectSubgraph.get_Left(), -rectSubgraph.get_Top());

        pVisual->RenderDependencies(pRenderer, mtx, localClipRect);

        float oldOpacity = pRenderer->get_CurrentOpacity();

        if(pVisual->get_HasVisualOpacity())
        {
            pRenderer->set_CurrentOpacity(pVisual->get_VisualOpacity());
        }

        pRenderer->get_ClientRendererInterface()->BeginSubgraphCapture(rectSubgraph);

        pVisual->RenderCore(pRenderer, mtx, localClipRect);

        m_pClientSubgraphCapture = pRenderer->get_ClientRendererInterface()->EndSubgraphCapture();

        pRenderer->set_CurrentOpacity(oldOpacity);
    }
}



}; // namespace AVUI
