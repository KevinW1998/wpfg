#pragma once

#include <AVUIContentControl.h>
#include <AVUIPanel.h>

namespace AVUI {

class Page : public ContentControl
{
public:
    Page() { };
    ~Page() { };

    DECLARE_ELEMENT(Page, ContentControl);

    static const AVUI::DependencyProperty& get_BackgroundProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(DependencyPropertyMetadata(PropertyFlag::AffectsRender | PropertyFlag::IgnoreSubPropertyInvalidation));
        const static AVUI::DependencyProperty& property = Panel::get_BackgroundProperty().AddOwner(GET_TYPE(_thisType), NULL, &metadata);

        return property;
    }
    Brush* get_Background() { return object_cast<Brush>(GetValue(get_BackgroundProperty())); }
    void set_Background(Brush* pBrush) { SetValue(get_BackgroundProperty(), pBrush); }

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(WindowTitle, String, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(WindowWidth, float, Float::get_NaN(), PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(WindowHeight, float, Float::get_NaN(), PropertyFlag::AffectsMeasure);
};


}; // namespace AVUI
