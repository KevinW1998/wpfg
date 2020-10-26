#include <AVUICommon.h>
#include <AVUINavigationWindow.h>
#include <AVUIApplication.h>
#include <AVUIIClientRendererInterface.h>

REGISTER_ELEMENT_BEGIN(AVUI::NavigationWindow)

    REGISTER_DEFAULT_FACTORY()
REGISTER_ELEMENT_END()

REGISTER_ELEMENT(AVUI::CancelEventArgs);
REGISTER_ELEMENT(AVUI::NavigatingCancelEventArgs);

namespace AVUI {


void NavigationWindow::Navigate(const String& navTo)
{
    NavigatingCancelEventArgs args(object_allocate<String>(navTo), NULL);

    invoke_Navigating(this, &args);

    if(args.get_Cancel())
    {
        return;
    }

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

    EventArgs navigatedArgs;

    invoke_Navigated(this, &navigatedArgs);
}

void NavigationWindow::Navigate(Object* pContent)
{
    NavigatingCancelEventArgs args(NULL, pContent);

    invoke_Navigating(this, &args);

    if(args.get_Cancel())
    {
        return;
    }

    set_Content(pContent);

    EventArgs navigatedArgs;

    invoke_Navigated(this, &navigatedArgs);
}


}; // namespace AVUI
