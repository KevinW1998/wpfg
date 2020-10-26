
#pragma once

#include <AVUIAnimatable.h>
#include <AVUIColor.h>

namespace AVUI {

class GradientStop : public Animatable
{
    friend class XamlParser;
public:
    GradientStop();

    DECLARE_ELEMENT(GradientStop, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Color, Color, Color(1.0f, 1.0f, 1.0f), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Offset, float, 0.0f, PropertyFlag::None);

};


}; // namespace AVUI
