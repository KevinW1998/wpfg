#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIRenderer.h>
#include <AVUILinearGradientBrush.h>

REGISTER_ELEMENT_BEGIN(AVUI::LinearGradientBrush)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(StartPoint);
    REGISTER_PROPERTY(EndPoint);
REGISTER_ELEMENT_END()

namespace AVUI {

LinearGradientBrush::LinearGradientBrush() : m_startPoint(0.0f, 0.0f), m_endPoint(1.0f, 1.0f), m_needsGradientUpdate(false)
{
    m_pClientLinearGradientBrush = Application::get_Current()->get_ClientRendererInterface()->CreateLinearGradientBrush();
}

LinearGradientBrush::~LinearGradientBrush()
{
    m_pClientLinearGradientBrush->Destroy();
}

void LinearGradientBrush::CloneCore(Animatable* pSource)
{
    LinearGradientBrush* pBrushSource = (LinearGradientBrush*) pSource;

    GradientStopCollection* pStops = pBrushSource->get_GradientStops();

    for(int idx = 0; idx < pStops->get_Count(); idx++)
    {
        get_GradientStops()->Add((*pStops)[idx]->Clone());
    }

    Animatable::CloneCore(pSource);
}

void LinearGradientBrush::set_ActiveBrush(Renderer* pRenderer)
{
    if(m_needsGradientUpdate)
    {
        GradientStopCollection* pStops = get_GradientStops();

        m_pClientLinearGradientBrush->BeginGradientUpdate(pStops->get_Count());

        for(int idx = 0; idx < pStops->get_Count(); idx++)
        {
            m_pClientLinearGradientBrush->AddGradientStop((*pStops)[idx]->get_Offset(), (*pStops)[idx]->get_Color().get_ARGB());
        }

        m_pClientLinearGradientBrush->EndGradientUpdate();
        m_needsGradientUpdate = false;
    }

    pRenderer->get_ClientRendererInterface()->set_CurrentBrush(this, m_pClientLinearGradientBrush);
}

void LinearGradientBrush::OnGradientStopsChanged()
{
    m_needsGradientUpdate = true;
}

void LinearGradientBrush::OnStartPointChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    LinearGradientBrush* pBrush = (LinearGradientBrush*) pDO;
    pBrush->m_startPoint = UnboxValue<Point>(e.pNewValue);
}

void LinearGradientBrush::OnEndPointChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    LinearGradientBrush* pBrush = (LinearGradientBrush*) pDO;
    pBrush->m_endPoint = UnboxValue<Point>(e.pNewValue);
}


}; // namespace AVUI


