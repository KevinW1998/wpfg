#include <AVUICommon.h>
#include <AVUITranslateTransform3D.h>
#include <AVUIMatrix3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::TranslateTransform3D)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(OffsetX);
    REGISTER_PROPERTY(OffsetY);
    REGISTER_PROPERTY(OffsetZ);
REGISTER_ELEMENT_END()

namespace AVUI {

TranslateTransform3D::TranslateTransform3D(const Vector3D& offset)
{
    set_OffsetX(offset.get_X());
    set_OffsetY(offset.get_Y());
    set_OffsetZ(offset.get_Z());
}

TranslateTransform3D::TranslateTransform3D(float offsetX, float offsetY, float offsetZ)
{
    set_OffsetX(offsetX);
    set_OffsetY(offsetY);
    set_OffsetZ(offsetZ);
}


Matrix3D TranslateTransform3D::get_Value()
{
    Matrix3D returnMatrix = Matrix3D::CreateIdentity();

    returnMatrix.SetTranslationMatrix(Vector3D(get_OffsetX(), get_OffsetY(), get_OffsetZ()));

    return returnMatrix;
}


}; // namespace AVUI
