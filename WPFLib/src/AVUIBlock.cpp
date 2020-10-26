#include <AVUICommon.h>
#include <AVUIBlock.h>
#include <AVUILength.h>

REGISTER_ELEMENT_BEGIN(AVUI::Block)

    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
    REGISTER_PROPERTY(LineHeight)
REGISTER_ELEMENT_END()

namespace AVUI {

void Block::StaticConstructor()
{
    static LengthTypeConverter lengthConverter;

    get_LineHeightProperty().RegisterPropertyConverter(&lengthConverter);
}


}; // namespace AVUI
