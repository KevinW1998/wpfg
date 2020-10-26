#include <AVUICommon.h>
#include <AVUIScaleTransform3D.h>
#include <AVUIMatrix3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::ScaleTransform3D)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(CenterX);
    REGISTER_PROPERTY(CenterY);
    REGISTER_PROPERTY(CenterZ);
    REGISTER_PROPERTY(ScaleX);
    REGISTER_PROPERTY(ScaleY);
    REGISTER_PROPERTY(ScaleZ);
REGISTER_ELEMENT_END()

namespace AVUI {

Matrix3D ScaleTransform3D::get_Value()
{
    Point3D location(get_CenterX(), get_CenterY(), get_CenterZ());
    Vector3D scale(get_ScaleX(), get_ScaleY(), get_ScaleZ());

    Matrix3D returnMatrix = Matrix3D::CreateIdentity();

    returnMatrix.ScaleAt(scale, location);

    return returnMatrix;
}


}; // namespace AVUI
