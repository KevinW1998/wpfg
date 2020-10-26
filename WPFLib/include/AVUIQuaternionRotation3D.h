#pragma once

#include <AVUIRotation3D.h>

namespace AVUI {

class QuaternionRotation3D : public Rotation3D
{
public:
    QuaternionRotation3D() { };
    QuaternionRotation3D(const Quaternion& quaternion) { set_Quaternion(quaternion); };
    virtual ~QuaternionRotation3D() { }

    virtual Quaternion get_InternalQuaternion();
    DECLARE_ELEMENT(QuaternionRotation3D, Rotation3D);
                       
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Quaternion, Quaternion, Quaternion(), PropertyFlag::None);

private:
};


}; // namespace AVUI
