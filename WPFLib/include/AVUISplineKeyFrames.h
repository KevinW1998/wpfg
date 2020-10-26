#pragma once

#include <AVUIKeyFrames.h>

namespace AVUI {

class SplineSingleKeyFrame : public TValueSplineKeyFrame<float, SingleKeyFrame>
{
public:
    DECLARE_ELEMENT(SplineSingleKeyFrame, SingleKeyFrame);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(KeySpline, KeySpline, NULL, PropertyFlag::None);
};

class SplineDoubleKeyFrame : public TValueSplineKeyFrame<float, DoubleKeyFrame>
{
public:
    DECLARE_ELEMENT(SplineDoubleKeyFrame, DoubleKeyFrame);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(KeySpline, KeySpline, NULL, PropertyFlag::None);
};

class SplinePointKeyFrame : public TValueSplineKeyFrame<Point, PointKeyFrame>
{
public:
    DECLARE_ELEMENT(SplinePointKeyFrame, PointKeyFrame);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(KeySpline, KeySpline, NULL, PropertyFlag::None);
};

class SplineColorKeyFrame : public TValueSplineKeyFrame<Color, ColorKeyFrame>
{
public:
    DECLARE_ELEMENT(SplineColorKeyFrame, ColorKeyFrame);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(KeySpline, KeySpline, NULL, PropertyFlag::None);
};

class SplineVector3DKeyFrame : public TValueSplineKeyFrame<Vector3D, Vector3DKeyFrame>
{
public:
    DECLARE_ELEMENT(SplineVector3DKeyFrame, Vector3DKeyFrame);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(KeySpline, KeySpline, NULL, PropertyFlag::None);
};

class SplinePoint3DKeyFrame : public TValueSplineKeyFrame<Point3D, Point3DKeyFrame>
{
public:
    DECLARE_ELEMENT(SplinePoint3DKeyFrame, Point3DKeyFrame);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(KeySpline, KeySpline, NULL, PropertyFlag::None);
};


class SplineRotation3DKeyFrame : public TReferenceSplineKeyFrame<Rotation3D, Rotation3DKeyFrame>
{
public:
    DECLARE_ELEMENT(SplineRotation3DKeyFrame, Rotation3DKeyFrame);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(KeySpline, KeySpline, NULL, PropertyFlag::None);
};




}; // namespace AVUI
