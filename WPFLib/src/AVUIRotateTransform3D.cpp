#include <AVUICommon.h>
#include <AVUIRotateTransform3D.h>
#include <AVUIMatrix3D.h>
#include <AVUIRotation3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::RotateTransform3D)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(CenterX);
    REGISTER_PROPERTY(CenterY);
    REGISTER_PROPERTY(CenterZ);
    REGISTER_PROPERTY(Rotation);
REGISTER_ELEMENT_END()

namespace AVUI {

Matrix3D RotateTransform3D::get_Value()
{
    Rotation3D* pRotation = get_Rotation();
    if(!pRotation)
    {
        return Matrix3D::CreateIdentity();
    }

    // todo - quaternion from rotation
    return Matrix3D::CreateRotationMatrix(pRotation->get_InternalQuaternion(), Point3D(get_CenterX(), get_CenterY(), get_CenterZ()));
}


}; // namespace AVUI
