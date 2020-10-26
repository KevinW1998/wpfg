#pragma once

#include <AVUIKeyFrames.h>

namespace AVUI {

class LinearSingleKeyFrame : public TValueLinearKeyFrame<float, SingleKeyFrame>
{
public:
    DECLARE_ELEMENT(LinearSingleKeyFrame, SingleKeyFrame);
};

class LinearDoubleKeyFrame : public TValueLinearKeyFrame<float, DoubleKeyFrame>
{
public:
    DECLARE_ELEMENT(LinearDoubleKeyFrame, DoubleKeyFrame);
};

class LinearPointKeyFrame : public TValueLinearKeyFrame<Point, PointKeyFrame>
{
public:
    DECLARE_ELEMENT(LinearPointKeyFrame, PointKeyFrame);
};

class LinearColorKeyFrame : public TValueLinearKeyFrame<Color, ColorKeyFrame>
{
public:
    DECLARE_ELEMENT(LinearColorKeyFrame, ColorKeyFrame);
};

class LinearVector3DKeyFrame : public TValueLinearKeyFrame<Vector3D, Vector3DKeyFrame>
{
public:
    DECLARE_ELEMENT(LinearVector3DKeyFrame, Vector3DKeyFrame);
};

class LinearPoint3DKeyFrame : public TValueLinearKeyFrame<Point3D, Point3DKeyFrame>
{
public:
    DECLARE_ELEMENT(LinearPoint3DKeyFrame, Point3DKeyFrame);
};

class LinearRotation3DKeyFrame : public TReferenceLinearKeyFrame<Rotation3D, Rotation3DKeyFrame>
{
public:
    DECLARE_ELEMENT(LinearRotation3DKeyFrame, Rotation3DKeyFrame);
};


}; // namespace AVUI
