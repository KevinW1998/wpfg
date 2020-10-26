#pragma once

#include <AVUICamera.h>
#include <AVUIMatrix3D.h>

namespace AVUI {

class Transform3D;

class ProjectionCamera : public Camera
{
public:
    ProjectionCamera() { };

    DECLARE_ELEMENT(ProjectionCamera, Camera);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(NearPlaneDistance, float, 0.125f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(FarPlaneDistance, float, Float::get_PositiveInfinity(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Position, Point3D, Point3D(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(LookDirection, Vector3D, Vector3D(0.0f, 0.0f, -1.0f), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(UpDirection, Vector3D, Vector3D(0.0f, 1.0f, 0.0f), PropertyFlag::None);

    virtual Matrix3D GetViewMatrix();

private:
    Matrix3D CreateViewMatrix(Transform3D* pTransform, const Point3D& position, const Vector3D& lookDirection, const Vector3D& upDirection);

};


}; // namespace AVUI
