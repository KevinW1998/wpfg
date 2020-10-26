#include <AVUITAnimation.h>

namespace AVUI {

class QuaternionAnimation : public TValueAnimation<Quaternion>
{
public:

    QuaternionAnimation();

    DECLARE_ELEMENT(QuaternionAnimation, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(By, Nullable<Quaternion>, Nullable<Quaternion>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(From, Nullable<Quaternion>, Nullable<Quaternion>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(To, Nullable<Quaternion>, Nullable<Quaternion>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);

};

typedef TRefCountedPtr<QuaternionAnimation> QuaternionAnimationPtr;

}; // namespace AVUI
