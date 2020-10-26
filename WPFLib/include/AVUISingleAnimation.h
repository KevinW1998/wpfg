#include <AVUITAnimation.h>

namespace AVUI {

class SingleAnimation : public TValueAnimation<float>
{
public:

    SingleAnimation();

    DECLARE_ELEMENT(SingleAnimation, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(By, Nullable<float>, Nullable<float>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(From, Nullable<float>, Nullable<float>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(To, Nullable<float>, Nullable<float>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);

};

typedef TRefCountedPtr<SingleAnimation> SingleAnimationPtr;

}; // namespace AVUI
