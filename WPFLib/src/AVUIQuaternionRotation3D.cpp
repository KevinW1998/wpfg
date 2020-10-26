#include <AVUICommon.h>
#include <AVUIQuaternionRotation3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::QuaternionRotation3D)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Quaternion)
REGISTER_ELEMENT_END()

namespace AVUI {

Quaternion QuaternionRotation3D::get_InternalQuaternion()
{
    return get_Quaternion();
}


}; // namespace AVUI
