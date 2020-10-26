#include <AVUITAnimation.h>

namespace AVUI {

class PointAnimation : public TValueAnimation<Point>
{
public:

    PointAnimation();

    DECLARE_ELEMENT(PointAnimation, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(By, Nullable<Point>, Nullable<Point>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(From, Nullable<Point>, Nullable<Point>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(To, Nullable<Point>, Nullable<Point>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
};

typedef TRefCountedPtr<PointAnimation> PointAnimationPtr;

}; // namespace AVUI
