#include <AVUICommon.h>
#include <AVUISoundPlayerAction.h>
#include <AVUIApplication.h>
#include <AVUIIClientRendererInterface.h>

REGISTER_ELEMENT_BEGIN(AVUI::SoundPlayerAction)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Source)
REGISTER_ELEMENT_END()

namespace AVUI {

void SoundPlayerAction::Invoke(DependencyObject* pTarget, ControlTemplate* pTemplate)
{
    Application::get_Current()->get_ClientRendererInterface()->PlaySound(get_Source()->get_Ptr());
}

void SoundPlayerAction::Seal()
{
    DependencyObject::Seal();
}


}; // namespace AVUI
