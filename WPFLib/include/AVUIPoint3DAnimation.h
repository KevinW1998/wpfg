#include <AVUITAnimation.h>

namespace AVUI {

class Point3DAnimation : public TValueAnimation<Point3D>
{
public:

    Point3DAnimation();

    DECLARE_ELEMENT(Point3DAnimation, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(By, Nullable<Point3D>, Nullable<Point3D>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(From, Nullable<Point3D>, Nullable<Point3D>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(To, Nullable<Point3D>, Nullable<Point3D>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
};

typedef TRefCountedPtr<Point3DAnimation> Point3DAnimationPtr;

}; // namespace AVUI
