#pragma once

#include <AVUIDecorator.h>

namespace AVUI {

class Viewbox : public Decorator
{
public:

    virtual ~Viewbox() { }

    DECLARE_ELEMENT(Viewbox, Decorator);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Stretch, Stretch, Stretch::Uniform, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(StretchDirection, StretchDirection, StretchDirection::Both, PropertyFlag::AffectsMeasure);

    static Size ComputeScaleFactor(const Size& availableSize, const Size& contentSize, Stretch::Enum stretch, StretchDirection::Enum stretchDirection);

protected:

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);
};


}; // namespace AVUI
