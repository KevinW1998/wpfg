#include <AVUICommon.h>
#include <AVUIColorAnimation.h>

REGISTER_ELEMENT_BEGIN(AVUI::ColorAnimation)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(By);
    REGISTER_PROPERTY(From);
    REGISTER_PROPERTY(To);
    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
REGISTER_ELEMENT_END()

namespace AVUI {

ColorAnimation::ColorAnimation()
{

}


}; // namespace AVUI
