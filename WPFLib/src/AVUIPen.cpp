#include <AVUICommon.h>
#include <AVUIPen.h>

REGISTER_ELEMENT_BEGIN(AVUI::Pen)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Brush);
    REGISTER_PROPERTY(Thickness);
REGISTER_ELEMENT_END()

