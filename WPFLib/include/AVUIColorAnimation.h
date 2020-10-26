#include <AVUITAnimation.h>
#include <AVUIColor.h>

namespace AVUI {


class ColorAnimation : public TValueAnimation<Color>
{
public:

    ColorAnimation();

    DECLARE_ELEMENT(ColorAnimation, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(By, Nullable<Color>, Nullable<Color>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(From, Nullable<Color>, Nullable<Color>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(To, Nullable<Color>, Nullable<Color>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
};

}; // namespace AVUI
