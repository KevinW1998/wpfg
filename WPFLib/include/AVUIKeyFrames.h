#pragma once

#include <AVUITKeyFrame.h>
#include <AVUIKeyTime.h>
#include <AVUIPoint.h>
#include <AVUIColor.h>
#include <AVUIPoint3D.h>
#include <AVUIVector3D.h>
#include <AVUIRotation3D.h>

namespace AVUI {

class SingleKeyFrame : public TValueKeyFrame<float>
{
public:
    DECLARE_ELEMENT(SingleKeyFrame, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Value, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(KeyTime, KeyTime, KeyTime(), PropertyFlag::None);
};

class DoubleKeyFrame : public TValueKeyFrame<float>
{
public:
    DECLARE_ELEMENT(DoubleKeyFrame, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Value, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(KeyTime, KeyTime, KeyTime(), PropertyFlag::None);
};

class PointKeyFrame : public TValueKeyFrame<Point>
{
public:
    DECLARE_ELEMENT(PointKeyFrame, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Value, Point, Point(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(KeyTime, KeyTime, KeyTime(), PropertyFlag::None);
};


class ColorKeyFrame : public TValueKeyFrame<Color>
{
public:
    DECLARE_ELEMENT(ColorKeyFrame, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Value, Color, Color(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(KeyTime, KeyTime, KeyTime(), PropertyFlag::None);
};

class Vector3DKeyFrame : public TValueKeyFrame<Vector3D>
{
public:
    DECLARE_ELEMENT(Vector3DKeyFrame, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Value, Vector3D, Vector3D(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(KeyTime, KeyTime, KeyTime(), PropertyFlag::None);
};

class Point3DKeyFrame : public TValueKeyFrame<Point3D>
{
public:
    DECLARE_ELEMENT(Point3DKeyFrame, Animatable);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Value, Point3D, Point3D(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(KeyTime, KeyTime, KeyTime(), PropertyFlag::None);
};

class Rotation3DKeyFrame : public TReferenceKeyFrame<Rotation3D>
{
public:
    DECLARE_ELEMENT(Rotation3DKeyFrame, Animatable);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Value, Rotation3D, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(KeyTime, KeyTime, KeyTime(), PropertyFlag::None);
};



}; // namespace AVUI
