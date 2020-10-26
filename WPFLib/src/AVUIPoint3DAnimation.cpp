#include <AVUICommon.h>
#include <AVUIPoint3DAnimation.h>

REGISTER_ELEMENT_BEGIN(AVUI::Point3DAnimation)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(By);
    REGISTER_PROPERTY(From);
    REGISTER_PROPERTY(To);
    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
REGISTER_ELEMENT_END()

namespace AVUI {

Point3DAnimation::Point3DAnimation()
{

}


}; // namespace AVUI
