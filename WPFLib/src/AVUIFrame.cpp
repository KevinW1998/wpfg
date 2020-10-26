#include <AVUICommon.h>
#include <AVUIFrame.h>
#include <AVUIApplication.h>
#include <AVUIIClientRendererInterface.h>

REGISTER_ELEMENT_BEGIN(AVUI::Frame)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI {


void Frame::Navigate(const String& navTo)
{
    IClientNavResult* pResult = Application::get_Current()->get_ClientRendererInterface()->NavigateTo(navTo.get_Ptr());

    while(pResult != NULL && pResult->GetNavStatus() == NavigationStatus::Navigating)
    {
        // Empty;
    }

    if(pResult != NULL)
    {
        set_Content(pResult->GetNavResult());
        pResult->Destroy();
        pResult = NULL;
    }
}


}; // namespace AVUI
