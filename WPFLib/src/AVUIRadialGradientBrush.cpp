#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIRenderer.h>
#include <AVUIRadialGradientBrush.h>

REGISTER_ELEMENT_BEGIN(AVUI::RadialGradientBrush)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Center);
    REGISTER_PROPERTY(RadiusX);
    REGISTER_PROPERTY(RadiusY);
    REGISTER_PROPERTY(GradientOrigin);
REGISTER_ELEMENT_END()

namespace AVUI {

RadialGradientBrush::RadialGradientBrush() : m_needsGradientUpdate(false)
{
    m_pClientRadialGradientBrush = Application::get_Current()->get_ClientRendererInterface()->CreateRadialGradientBrush();

    m_pClientRadialGradientBrush->set_Center(get_Center().get_X(), get_Center().get_Y());
    m_pClientRadialGradientBrush->set_RadiusX(get_RadiusX());
    m_pClientRadialGradientBrush->set_RadiusY(get_RadiusY());
    m_pClientRadialGradientBrush->set_GradientOrigin(get_Center().get_X(), get_Center().get_Y());
}

RadialGradientBrush::~RadialGradientBrush()
{
    m_pClientRadialGradientBrush->Destroy();
}

void RadialGradientBrush::CloneCore(Animatable* pSource)
{
    RadialGradientBrush* pBrushSource = (RadialGradientBrush*) pSource;

    GradientStopCollection* pStops = pBrushSource->get_GradientStops();

    for(int idx = 0; idx < pStops->get_Count(); idx++)
    {
        get_GradientStops()->Add((*pStops)[idx]->Clone());
    }

    Animatable::CloneCore(pSource);
}

void RadialGradientBrush::set_ActiveBrush(Renderer* pRenderer)
{
    if(m_needsGradientUpdate)
    {
        GradientStopCollection* pStops = get_GradientStops();

        m_pClientRadialGradientBrush->BeginGradientUpdate(pStops->get_Count());

        for(int idx = 0; idx < pStops->get_Count(); idx++)
        {
            m_pClientRadialGradientBrush->AddGradientStop((*pStops)[idx]->get_Offset(), (*pStops)[idx]->get_Color().get_ARGB());
        }

        m_pClientRadialGradientBrush->EndGradientUpdate();
        m_needsGradientUpdate = false;
    }

    pRenderer->get_ClientRendererInterface()->set_CurrentBrush(this, m_pClientRadialGradientBrush);
}

void RadialGradientBrush::OnGradientStopsChanged()
{
    m_needsGradientUpdate = true;
}

void RadialGradientBrush::OnCenterChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    RadialGradientBrush* pBrush = (RadialGradientBrush*) pDO;
    Point newPoint = UnboxValue<Point>(e.pNewValue);

    pBrush->m_pClientRadialGradientBrush->set_Center(newPoint.get_X(), newPoint.get_Y());
}

void RadialGradientBrush::OnRadiusXChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    RadialGradientBrush* pBrush = (RadialGradientBrush*) pDO;
    float newRadiusX = UnboxValue<float>(e.pNewValue);

    pBrush->m_pClientRadialGradientBrush->set_RadiusX(newRadiusX);    
}

void RadialGradientBrush::OnRadiusYChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    RadialGradientBrush* pBrush = (RadialGradientBrush*) pDO;
    float newRadiusY = UnboxValue<float>(e.pNewValue);

    pBrush->m_pClientRadialGradientBrush->set_RadiusY(newRadiusY);    
}

void RadialGradientBrush::OnGradientOriginChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    RadialGradientBrush* pBrush = (RadialGradientBrush*) pDO;
    Point newPoint = UnboxValue<Point>(e.pNewValue);

    pBrush->m_pClientRadialGradientBrush->set_GradientOrigin(newPoint.get_X(), newPoint.get_Y());
}



}; // namespace AVUI


