#include <AVUICommon.h>
#include <AVUIDiffuseMaterial.h>

REGISTER_ELEMENT_BEGIN(AVUI::DiffuseMaterial)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Brush)
    REGISTER_PROPERTY(Color)
    REGISTER_PROPERTY(AmbientColor)
REGISTER_ELEMENT_END()

namespace AVUI {


}; // namespace AVUI
