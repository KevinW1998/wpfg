#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {
    
class Camera : public Animatable
{
public:
    Camera() { };

    DECLARE_ELEMENT(Camera, Animatable);

    virtual Matrix3D GetProjectionMatrix(float aspectRatio) = 0;
    virtual Matrix3D GetViewMatrix() = 0;

};


}; // namespace AVUI
