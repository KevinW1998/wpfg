#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {

class IClientImageSource;

class BitmapImage : public Animatable
{
    friend class ImageBrush;
public:

    DECLARE_ELEMENT(BitmapImage, Animatable);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(CacheOption, BitmapCacheOption, BitmapCacheOption::Default, PropertyFlag::None);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(CreateOptions, BitmapCreateOptions, BitmapCreateOptions::None, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(UriSource, String, NULL, DependencyPropertyMetadata(PropertyFlag::None, BitmapImage::OnUriSourceChanged));

    BitmapImage() : m_pClientImageSource(NULL) { };
    BitmapImage(String* pURI) : m_pClientImageSource(NULL) { set_UriSource(pURI); }
    ~BitmapImage();

    float get_Width();
    float get_Height();

private:
    IClientImageSource* get_ClientImageSource() { return m_pClientImageSource; }

    static void OnUriSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    IClientImageSource* m_pClientImageSource;
};

class BitmapImageTypeConverter : public TypeConverter
{
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const { return false; };
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { LibraryCriticalError(); return NULL; }
};


}; // namespace AVUI

