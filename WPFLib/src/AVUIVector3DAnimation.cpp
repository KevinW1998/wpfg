#include <AVUICommon.h>
#include <AVUIVector3DAnimation.h>

REGISTER_ELEMENT_BEGIN(AVUI::Vector3DAnimation)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(By);
    REGISTER_PROPERTY(From);
    REGISTER_PROPERTY(To);
    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
REGISTER_ELEMENT_END()

namespace AVUI {

Vector3DAnimation::Vector3DAnimation()
{

}


}; // namespace AVUI
