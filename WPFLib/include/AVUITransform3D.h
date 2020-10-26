#pragma once

#include <AVUIAnimatable.h>
#include <AVUIMatrix3D.h>

namespace AVUI {

class Transform3D : public Animatable
{
public:

    virtual Matrix3D get_Value() = 0;
    DECLARE_ELEMENT(Transform3D, Animatable);


    static const Transform3D* get_Identity();
    bool get_IsIdentity();

protected:
    Transform3D() { };
    virtual ~Transform3D() { }

private:
};


}; // namespace AVUI
