#pragma once

#include <AVUIRotation3D.h>
#include <AVUIVector3D.h>

namespace AVUI {

class AxisAngleRotation3D : public Rotation3D
{
public:
    AxisAngleRotation3D() { };
    AxisAngleRotation3D(const Vector3D& axis, float angle) { set_Angle(angle); set_Axis(axis); };
    virtual ~AxisAngleRotation3D() { }

    virtual Quaternion get_InternalQuaternion();
    DECLARE_ELEMENT(AxisAngleRotation3D, Rotation3D);
                       


    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Angle, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Axis, Vector3D, Vector3D(0.0f, 1.0f, 0.0f), PropertyFlag::None);

private:
};


}; // namespace AVUI
