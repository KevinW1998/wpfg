#include <AVUICommon.h>
#include <AVUIMediaElement.h>
#include <AVUIRenderer.h>
#include <AVUIIClientRendererInterface.h>

REGISTER_ELEMENT_BEGIN(AVUI::MediaElement)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Source)
    REGISTER_PROPERTY(Volume)
    REGISTER_PROPERTY(Stretch)
    REGISTER_PROPERTY(StretchDirection)
REGISTER_ELEMENT_END()

namespace AVUI {

MediaElement::MediaElement() : m_pClientMediaElement(NULL)
{
    set_IsMediaElement(true); 
}

MediaElement::~MediaElement()
{
    if(m_pClientMediaElement != NULL)
    {
        m_pClientMediaElement->Destroy();
        m_pClientMediaElement = NULL;
    }
}

Size MediaElement::MeasureOverride(const Size& constraintSize)
{
    return constraintSize;

}

Size MediaElement::ArrangeOverride(const Size& arrangeSize)
{
    return arrangeSize;
}

void MediaElement::RenderMedia(Renderer* pRenderer)
{
    if(m_pClientMediaElement == NULL)
    {
        TRefCountedPtr<String> pSource = get_Source();

        if(pSource == NULL || pSource->get_Cch() == 0)
        {
            return;
        }

        m_pClientMediaElement = pRenderer->get_ClientRendererInterface()->CreateMediaElement(pSource->get_Ptr());
        m_pClientMediaElement->set_Volume(get_Volume());
    }

    pRenderer->get_ClientRendererInterface()->RenderMediaElement(m_pClientMediaElement, get_RenderSize());
}

void MediaElement::OnRender(DrawingContext& drawingContext)
{
    drawingContext.DrawRoundedRectangle(NULL, NULL, Rect(Point(), get_RenderSize()), 0.0f, 0.0f);
}

void MediaElement::OnVolumeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    MediaElement* pMediaElement = (MediaElement*) pDO;

    if(pMediaElement->m_pClientMediaElement != NULL)
    {
        pMediaElement->m_pClientMediaElement->set_Volume(UnboxValue<float>(e.pNewValue));
    }
}

void MediaElement::OnVisualParentChanged(DependencyObject* pOldParent)
{
    if(m_pClientMediaElement != NULL)
    {
        m_pClientMediaElement->Destroy();
        m_pClientMediaElement = NULL;
    }
}



}; // namespace AVUI

