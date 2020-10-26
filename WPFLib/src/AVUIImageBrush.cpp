#include <AVUICommon.h>
#include <AVUIImageBrush.h>
#include <AVUIRenderer.h>
#include <AVUIApplication.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIBitmapImage.h>

REGISTER_ELEMENT_BEGIN(AVUI::ImageBrush)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(ImageSource);
REGISTER_ELEMENT_END()

namespace AVUI {

void ImageBrush::set_ActiveBrush(Renderer* pRenderer)
{
    IClientImageSource* pImageSource = NULL;

    if(m_pBitmapImage != NULL)
    {
        pImageSource = m_pBitmapImage->get_ClientImageSource();
    }

    pRenderer->get_ClientRendererInterface()->set_CurrentBrush(this, pImageSource);
}

float ImageBrush::get_NaturalWidth()
{
    if(m_pBitmapImage != NULL)
    {
        return m_pBitmapImage->get_Width();
    }

    return 0.0f;
}
float ImageBrush::get_NaturalHeight()
{
    if(m_pBitmapImage != NULL)
    {
        return m_pBitmapImage->get_Height();
    }

    return 0.0f;
}

void ImageBrush::OnImageSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    ((ImageBrush*)pDO)->m_pBitmapImage = (BitmapImage*)e.pNewValue;
}


}; // namespace AVUI
