#include <AVUICommon.h>
#include <AVUIPointAnimation.h>

REGISTER_ELEMENT_BEGIN(AVUI::PointAnimation)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(By);
    REGISTER_PROPERTY(From);
    REGISTER_PROPERTY(To);
    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
REGISTER_ELEMENT_END()

namespace AVUI {

PointAnimation::PointAnimation()
{

}


}; // namespace AVUI
