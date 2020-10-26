#pragma once

#include <AVUIAnimatable.h>
#include <AVUIMatrix.h>

namespace AVUI {

class Transform : public Animatable
{
public:

    virtual Matrix get_Value() = 0;
    DECLARE_ELEMENT(Transform, Animatable);


    static const Transform* get_Identity();
    bool get_IsIdentity();

protected:
    Transform() { };
    virtual ~Transform() { }

private:
};


}; // namespace AVUI
