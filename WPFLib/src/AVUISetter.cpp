#include <AVUICommon.h>
#include <AVUISetter.h>

REGISTER_ELEMENT_BEGIN(AVUI::Setter)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Property)
    REGISTER_PROPERTY(Value)
    REGISTER_PROPERTY(TargetName)
REGISTER_ELEMENT_END()


namespace AVUI {

Setter::Setter() : m_pProperty(NULL)
{             

}


}; // namespace AVUI
