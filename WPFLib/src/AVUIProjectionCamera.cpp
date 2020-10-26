#include <AVUICommon.h>
#include <AVUIProjectionCamera.h>
#include <AVUIProjectionCamera.h>

REGISTER_ELEMENT_BEGIN(AVUI::ProjectionCamera)

    REGISTER_PROPERTY(NearPlaneDistance)
    REGISTER_PROPERTY(FarPlaneDistance)
    REGISTER_PROPERTY(Position)
    REGISTER_PROPERTY(LookDirection)
    REGISTER_PROPERTY(UpDirection)
REGISTER_ELEMENT_END()

namespace AVUI {

Matrix3D ProjectionCamera::GetViewMatrix()
{
    Point3D position = get_Position();
    Vector3D lookDirection = get_LookDirection();
    Vector3D upDirection = get_UpDirection();

    return CreateViewMatrix(NULL, position, lookDirection, upDirection);
}

Matrix3D ProjectionCamera::CreateViewMatrix(Transform3D* pTransform, const Point3D& position, const Vector3D& lookDirection, const Vector3D& upDirection)
{
    Vector3D vectord2 = -lookDirection;
    vectord2.Normalize();
    Vector3D vectord3 = Vector3D::CrossProduct(upDirection, vectord2);
    vectord3.Normalize();

    Vector3D vectord4 = Vector3D::CrossProduct(vectord2, vectord3);
    Vector3D vectord = *(Vector3D *) &position;

    float offsetX = -Vector3D::DotProduct(vectord3, vectord);
    float offsetY = -Vector3D::DotProduct(vectord4, vectord);
    float offsetZ = -Vector3D::DotProduct(vectord2, vectord);

    Matrix3D viewMatrix(vectord3.get_X(), vectord4.get_X(), vectord2.get_X(), 0, 
                        vectord3.get_Y(), vectord4.get_Y(), vectord2.get_Y(), 0, 
                        vectord3.get_Z(), vectord4.get_Z(), vectord2.get_Z(), 0, 
                        offsetX, offsetY, offsetZ, 1);
    // Camera.PrependInverseTransform(transform, ref viewMatrix);
    return viewMatrix;
}

}; // namespace AVUI
