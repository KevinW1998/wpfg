#include <AVUICommon.h>
#include <AVUISingleAnimation.h>

REGISTER_ELEMENT_BEGIN(AVUI::SingleAnimation)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(By);
    REGISTER_PROPERTY(From);
    REGISTER_PROPERTY(To);
    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
REGISTER_ELEMENT_END()

namespace AVUI {

SingleAnimation::SingleAnimation()
{

}


}; // namespace AVUI
