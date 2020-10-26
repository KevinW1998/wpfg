
#pragma once
#include <AVUIFrameworkElement.h>

namespace AVUI {

class ImageBrush;
class BitmapImage;

class Image : public FrameworkElement
{
public:

    DECLARE_ELEMENT(Image, FrameworkElement);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Source, BitmapImage, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Stretch, Stretch, Stretch::Uniform, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(StretchDirection, StretchDirection, StretchDirection::Both, PropertyFlag::AffectsMeasure);

    Image() { }
    virtual ~Image();


private:
    Size MeasureArrangeHelper(const Size& inputSize);
    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);
    void OnRender(DrawingContext& drawingContext);

    TRefCountedPtr<ImageBrush> m_pBrush;
};


}; // namespace AVUI
