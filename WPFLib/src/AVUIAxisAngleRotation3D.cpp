#include <AVUICommon.h>
#include <AVUIAxisAngleRotation3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::AxisAngleRotation3D)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Axis)
    REGISTER_PROPERTY(Angle)
REGISTER_ELEMENT_END()

namespace AVUI {


Quaternion AxisAngleRotation3D::get_InternalQuaternion()
{
    return Quaternion(get_Axis(), get_Angle());
}


}; // namespace AVUI
