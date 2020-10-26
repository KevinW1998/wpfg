#include <AVUICommon.h>
#include <AVUIGradientStop.h>

REGISTER_ELEMENT_BEGIN(AVUI::GradientStop)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Color)
    REGISTER_PROPERTY(Offset)
REGISTER_ELEMENT_END()


namespace AVUI {

GradientStop::GradientStop()
{             

}


}; // namespace AVUI
