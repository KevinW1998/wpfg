#include <AVUICommon.h>
#include <AVUISolidColorBrush.h>
#include <AVUIRenderer.h>
#include <AVUIIClientRendererInterface.h>

REGISTER_ELEMENT_BEGIN(AVUI::SolidColorBrush)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Color);
REGISTER_ELEMENT_END()

namespace AVUI {
          
SolidColorBrush::SolidColorBrush(const Color& color) : m_color(Color(1.0f, 1.0f, 1.0f))
{
    set_Color(color);
}

void SolidColorBrush::set_ActiveBrush(Renderer* pRenderer) 
{
    pRenderer->get_ClientRendererInterface()->set_CurrentBrush(this);
}

void SolidColorBrush::OnColorChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    SolidColorBrush* pBrush = (SolidColorBrush*) pDO;

    pBrush->m_color = UnboxValue<Color>(e.pNewValue);
}


}; // namespace AVUI
