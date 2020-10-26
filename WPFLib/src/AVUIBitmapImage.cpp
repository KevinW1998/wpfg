#include <AVUICommon.h>
#include <AVUIBitmapImage.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIApplication.h>

REGISTER_ELEMENT_BEGIN(AVUI::BitmapImage)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_TYPE_CONVERTER(BitmapImageTypeConverter)
    REGISTER_PROPERTY(CacheOption)
    REGISTER_PROPERTY(CreateOptions)
    REGISTER_PROPERTY(UriSource)
REGISTER_ELEMENT_END()

namespace AVUI
{

BitmapImage::~BitmapImage()
{
    if(m_pClientImageSource != NULL)
    {
        m_pClientImageSource->Destroy();
        m_pClientImageSource = NULL;
    }
}

void BitmapImage::OnUriSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    BitmapImage* pBitmapImage = (BitmapImage*)pDO;
    if(pBitmapImage->m_pClientImageSource != NULL)
    {
        pBitmapImage->m_pClientImageSource->Destroy();
        pBitmapImage->m_pClientImageSource = NULL;
    }

    pBitmapImage->m_pClientImageSource = Application::get_Current()->get_ClientRendererInterface()->CreateImageSource(((String*)e.pNewValue)->get_Ptr());
}

float BitmapImage::get_Width()
{
    if(m_pClientImageSource != NULL)
    {
        return m_pClientImageSource->get_Width();
    }
    return 0.0f;
}

float BitmapImage::get_Height()
{
    if(m_pClientImageSource != NULL)
    {
        return m_pClientImageSource->get_Height();
    }
    return 0.0f;
}


bool BitmapImageTypeConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr BitmapImageTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    if(pObjectFrom == NULL)
    {
        LibraryCriticalError();
        return NULL;
    }

    if(!CanConvertFrom(pObjectFrom->get_Type()))
    {
        LibraryCriticalError();
        return NULL;
    }

    TRefCountedPtr<BitmapImage> pBitmapImage = object_allocate<BitmapImage>(object_allocate<String>(((String*)pObjectFrom)->get_Ptr()));    

    return static_cast<Object*>(pBitmapImage);
}

}
