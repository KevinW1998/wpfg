#include <AVUICommon.h>
#include <AVUIPage.h>

REGISTER_ELEMENT_BEGIN(AVUI::Page)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Background)
    REGISTER_PROPERTY(WindowTitle)
    REGISTER_PROPERTY(WindowWidth)
    REGISTER_PROPERTY(WindowHeight)
REGISTER_ELEMENT_END()

namespace AVUI {

}; // namespace AVUI
