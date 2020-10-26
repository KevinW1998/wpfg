#include <AVUITAnimation.h>
#include <AVUIRotation3D.h>

namespace AVUI {

class Rotation3DAnimation : public TReferenceAnimation<Rotation3D>
{
public:

    Rotation3DAnimation();

    DECLARE_ELEMENT(Rotation3DAnimation, AnimationTimeline);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(By, Rotation3D, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(From, Rotation3D, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(To, Rotation3D, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);

};

typedef TRefCountedPtr<Rotation3DAnimation> Rotation3DAnimationPtr;

}; // namespace AVUI
