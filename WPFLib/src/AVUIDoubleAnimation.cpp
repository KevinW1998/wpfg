#include <AVUICommon.h>
#include <AVUIDoubleAnimation.h>

REGISTER_ELEMENT_BEGIN(AVUI::DoubleAnimation)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(By);
    REGISTER_PROPERTY(From);
    REGISTER_PROPERTY(To);
    REGISTER_PROPERTY(IsAdditive);
    REGISTER_PROPERTY(IsCumulative);
REGISTER_ELEMENT_END()

namespace AVUI {

DoubleAnimation::DoubleAnimation()
{

}

DoubleAnimation::DoubleAnimation(float fromValue, float toValue, const Duration& duration, FillBehavior::Enum fillBehavior)
{
    set_From(fromValue);
    set_To(toValue);
    set_Duration(duration);
    set_FillBehavior(fillBehavior);
}


}; // namespace AVUI
