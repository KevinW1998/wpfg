#pragma once

#include <AVUIFrameworkElement.h>
#include <AVUIBrush.h>
#include <AVUIPen.h>

namespace AVUI {

class Shape : public FrameworkElement
{
public:
    Shape();

    DECLARE_ELEMENT(Shape, FrameworkElement);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Fill, Brush, NULL, PropertyFlag::AffectsRender | PropertyFlag::IgnoreSubPropertyInvalidation);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Stroke, Brush, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsRender | PropertyFlag::IgnoreSubPropertyInvalidation, Shape::OnPenPropertyChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(StrokeThickness, float, 1.0f, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure | PropertyFlag::AffectsRender, Shape::OnPenPropertyChanged));
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Stretch, Stretch, Stretch::None, PropertyFlag::AffectsArrange);

    static void OnPenPropertyChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

protected:
    Pen* GetPen();
    float GetStrokeThickness();

private:

    bool get_IsPenNoOp();

    PenPtr m_pPen;
};

}; // namespace AVUI
