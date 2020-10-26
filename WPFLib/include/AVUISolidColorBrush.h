#pragma once

#include <AVUIBrush.h>
#include <AVUIColor.h>

namespace AVUI {

class IClientSolidColorBrush;
class SolidColorBrush : public Brush
{
public:

    SolidColorBrush() : m_color(Color(1.0f, 1.0f, 1.0f)) { };
    SolidColorBrush(const Color& color);

    DECLARE_ELEMENT(SolidColorBrush, Brush);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(Color, Color, Color(1.0f, 1.0f, 1.0f), DependencyPropertyMetadata(PropertyFlag::None, SolidColorBrush::OnColorChanged));
    const Color& get_Color() const { return m_color; }
    void set_Color(const Color& value) { SetValue(get_ColorProperty(), AVUI::BoxValue(value)); }
    static void OnColorChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    void set_ActiveBrush(Renderer* pRenderer);

private:

    Color m_color;
    IClientSolidColorBrush* m_pClientSolidColorBrush;
};

typedef TRefCountedPtr<SolidColorBrush> SolidColorBrushPtr;

}; // namespace AVUI
