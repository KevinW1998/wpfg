#pragma once

#include <AVUIProjectionCamera.h>

namespace AVUI {
    
class PerspectiveCamera : public ProjectionCamera
{
public:
    PerspectiveCamera() { };

    DECLARE_ELEMENT(PerspectiveCamera, ProjectionCamera);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(FieldOfView, float, 45.0f, PropertyFlag::None);

    virtual Matrix3D GetProjectionMatrix(float aspectRatio);

private:

    Matrix3D GetProjectionMatrix(float aspectRatio, float zn, float zf);

};


}; // namespace AVUI

