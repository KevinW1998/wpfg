#pragma once

#include <AVUIDecorator.h>
#include <AVUICornerRadius.h>
#include <AVUIPanel.h>

namespace AVUI {

class StreamGeometry;
class StreamGeometryContext;

class Border : public Decorator
{
public:

    DECLARE_ELEMENT(Border, Decorator);

    virtual ~Border() { }

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(BorderThickness,   Thickness, Thickness(0), PropertyFlag::AffectsMeasure | PropertyFlag::AffectsRender);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Padding,   Thickness, Thickness(0), PropertyFlag::AffectsMeasure | PropertyFlag::AffectsRender);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CornerRadius,   CornerRadius, CornerRadius(0), PropertyFlag::AffectsMeasure | PropertyFlag::AffectsRender);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(BorderBrush, Brush, NULL, PropertyFlag::AffectsRender | PropertyFlag::IgnoreSubPropertyInvalidation);

    static const AVUI::DependencyProperty& get_BackgroundProperty() 
    { 
        static AVUI::DependencyPropertyMetadata metadata(PropertyFlag::AffectsRender | PropertyFlag::IgnoreSubPropertyInvalidation);
        const static AVUI::DependencyProperty& property = Panel::get_BackgroundProperty().AddOwner(GET_TYPE(Border), NULL, &metadata);

        return property;
    }
    Brush* get_Background() { return (Brush*) (Object*) GetValue(get_BackgroundProperty()); }
    void set_Background(Brush* pBackground) { SetValue(get_BackgroundProperty(), (Object*) pBackground); }

protected:


    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);
    virtual void OnRender(DrawingContext& drawingContext);

private:
    Size HelperCollapseThickness(const Thickness& th);
    Rect HelperDeflateRect(const Rect& rt, const Thickness& thick);
    bool AreUniformCorners(const CornerRadius& borderRadii);

    struct Radii
    {
        float LeftTop;
        float TopLeft;
        float TopRight;
        float RightTop;
        float RightBottom;
        float BottomRight;
        float BottomLeft;
        float LeftBottom;
        Radii(const CornerRadius& radii, const Thickness& borders, bool outer);
    };

    void GenerateGeometry(StreamGeometryContext* pCtx, const Rect& rect, const Radii& radii);

    bool m_useComplexRenderCodePath;
    TRefCountedPtr<StreamGeometry> m_pBorderGeometry;
    TRefCountedPtr<StreamGeometry> m_pBackgroundGeometry;

};


}; // namespace AVUI
