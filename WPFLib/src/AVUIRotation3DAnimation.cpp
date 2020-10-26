#include <AVUICommon.h>
#include <AVUIRotation3DAnimation.h>

REGISTER_ELEMENT_BEGIN(AVUI::Rotation3DAnimation)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(By);
    REGISTER_PROPERTY(From);
    REGISTER_PROPERTY(To);
    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
REGISTER_ELEMENT_END()

namespace AVUI {

Rotation3DAnimation::Rotation3DAnimation()
{

}


}; // namespace AVUI
