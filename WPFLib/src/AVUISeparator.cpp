
#include <AVUICommon.h>
#include <AVUISeparator.h>

REGISTER_ELEMENT_BEGIN(AVUI::Separator)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()


namespace AVUI
{

void Separator::PrepareContainer(Control* pContainer)
{
    pContainer->set_IsEnabled(false);
    pContainer->set_HorizontalContentAlignment(HorizontalAlignment::Stretch);
}


};



