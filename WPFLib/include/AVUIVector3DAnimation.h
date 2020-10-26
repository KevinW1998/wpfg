#include <AVUITAnimation.h>

namespace AVUI {

class Vector3DAnimation : public TValueAnimation<Vector3D>
{
public:

    Vector3DAnimation();

    DECLARE_ELEMENT(Vector3DAnimation, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(By, Nullable<Vector3D>, Nullable<Vector3D>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(From, Nullable<Vector3D>, Nullable<Vector3D>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(To, Nullable<Vector3D>, Nullable<Vector3D>(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
};

typedef TRefCountedPtr<Vector3DAnimation> Vector3DAnimationPtr;

}; // namespace AVUI
