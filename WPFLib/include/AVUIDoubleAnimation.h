#include <AVUITAnimation.h>

namespace AVUI {

class DoubleAnimation : public TValueAnimation<float>
{
public:

    DoubleAnimation();
    DoubleAnimation(float fromValue, float toValue, const Duration& duration, FillBehavior::Enum fillBehavior);

    DECLARE_ELEMENT(DoubleAnimation, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(By, Nullable<float>, Nullable<float>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(From, Nullable<float>, Nullable<float>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(To, Nullable<float>, Nullable<float>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);

};

typedef TRefCountedPtr<DoubleAnimation> DoubleAnimationPtr;

}; // namespace AVUI
