#pragma once
#include <AVUIFrameworkElement.h>
#include <AVUIIAddChild.h>
#include <AVUIControl.h>

namespace AVUI {

class FormattedText;

class TextBlock : public FrameworkElement, public IAddChild
{
public:

    DECLARE_ELEMENT(TextBlock, FrameworkElement);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Text, String, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure);


    static const AVUI::DependencyProperty& get_FontSizeProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(PropertyFlag::AffectsMeasure);
        const static AVUI::DependencyProperty& property = Control::get_FontSizeProperty().AddOwner(GET_TYPE(TextBlock), BoxValue<float>(11.0f), &metadata);

        return property;
    }
    float get_FontSize() { return UnboxValue< float >(GetValue(get_FontSizeProperty())); }
    void set_FontSize(const float& value) { SetValue(get_FontSizeProperty(), AVUI::BoxValue(value)); }

    static const AVUI::DependencyProperty& get_ForegroundProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(PropertyFlag::AffectsMeasure | PropertyFlag::IgnoreSubPropertyInvalidation);
        const static AVUI::DependencyProperty& property = Control::get_ForegroundProperty().AddOwner(GET_TYPE(TextBlock), object_allocate<SolidColorBrush>(KnownColor::Black), &metadata);

        return property;
    }
    Brush* get_Foreground() { return object_cast<Brush>(GetValue(get_ForegroundProperty())); }
    void set_Foreground(Brush* pBrush) { SetValue(get_ForegroundProperty(), pBrush); }

    static const AVUI::DependencyProperty& get_FontFamilyProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(PropertyFlag::AffectsMeasure);
        const static AVUI::DependencyProperty& property = Control::get_FontFamilyProperty().AddOwner(GET_TYPE(TextBlock), object_allocate<String>(L"Tahoma"), &metadata);

        return property;
    }
    String* get_FontFamily() { return object_cast<String>(GetValue(get_FontFamilyProperty())); }
    void set_FontFamily(String* pFontFamily) { SetValue(get_FontFamilyProperty(), pFontFamily); }

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(TextWrapping, TextWrapping, TextWrapping::NoWrap, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(TextTrimming, TextTrimming, TextTrimming::CharacterEllipsis, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(TextAlignment, TextAlignment, TextAlignment::Left, PropertyFlag::AffectsMeasure);

    TextBlock() { }
    virtual ~TextBlock();

private:

    virtual void AddChild(Object* pValue);
    virtual void AddText(String* pText);


    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);
    void OnRender(DrawingContext& drawingContext);
    void ClearFormattedText();

    ItemStructList<TRefCountedPtr<FormattedText> > m_formattedText;
};


}; // namespace AVUI
