#include <AVUITKeyFrameAnimation.h>
#include <AVUIKeyFrameCollections.h>

namespace AVUI
{

class SingleAnimationUsingKeyFrames : public TValueKeyFrameAnimation<float, SingleKeyFrame, SingleKeyFrameCollection>
{
public:
    DECLARE_ELEMENT(SingleAnimationUsingKeyFrames, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(KeyFrames, SingleKeyFrameCollection, true);
};

class DoubleAnimationUsingKeyFrames : public TValueKeyFrameAnimation<float, DoubleKeyFrame, DoubleKeyFrameCollection>
{
public:
    DECLARE_ELEMENT(DoubleAnimationUsingKeyFrames, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(KeyFrames, DoubleKeyFrameCollection, true);
};

class PointAnimationUsingKeyFrames : public TValueKeyFrameAnimation<Point, PointKeyFrame, PointKeyFrameCollection>
{
public:
    DECLARE_ELEMENT(PointAnimationUsingKeyFrames, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(KeyFrames, PointKeyFrameCollection, true);
};

class ColorAnimationUsingKeyFrames : public TValueKeyFrameAnimation<Color, ColorKeyFrame, ColorKeyFrameCollection>
{
public:
    DECLARE_ELEMENT(ColorAnimationUsingKeyFrames, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(KeyFrames, ColorKeyFrameCollection, true);
};

class Vector3DAnimationUsingKeyFrames : public TValueKeyFrameAnimation<Vector3D, Vector3DKeyFrame, Vector3DKeyFrameCollection>
{
public:
    DECLARE_ELEMENT(Vector3DAnimationUsingKeyFrames, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(KeyFrames, Vector3DKeyFrameCollection, true);
};

class Point3DAnimationUsingKeyFrames : public TValueKeyFrameAnimation<Point3D, Point3DKeyFrame, Point3DKeyFrameCollection>
{
public:
    DECLARE_ELEMENT(Point3DAnimationUsingKeyFrames, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(KeyFrames, Point3DKeyFrameCollection, true);
};

class Rotation3DAnimationUsingKeyFrames : public TReferenceKeyFrameAnimation<Rotation3D, Rotation3DKeyFrame, Rotation3DKeyFrameCollection>
{
public:
    DECLARE_ELEMENT(Rotation3DAnimationUsingKeyFrames, AnimationTimeline);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsAdditive, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsCumulative, bool, false, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(KeyFrames, Rotation3DKeyFrameCollection, true);
};




}; // end namespace AVUI

