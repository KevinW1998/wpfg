#include <AVUICommon.h>
#include <AVUINavButton.h>
#include <AVUIFrame.h>
#include <AVUINavigationWindow.h>

REGISTER_ELEMENT_BEGIN(AVUI::NavButton)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(NavigateUri)
    REGISTER_PROPERTY(TargetName)
    REGISTER_ROUTED_EVENT(RequestNavigate);
REGISTER_ELEMENT_END()


REGISTER_ELEMENT(AVUI::RequestNavigateEventArgs)


namespace AVUI
{


void NavButton::OnClick()
{
    if(get_NavigateUri() != NULL)
    {
        RequestNavigateEventArgs eventArgs(&get_RequestNavigateEvent(), get_NavigateUri(), get_TargetName());

        eventArgs.set_Source(this);
        RaiseEvent(&eventArgs);

        if(!eventArgs.get_IsHandled() && get_TargetName() != NULL)
        {
            Object* pTarget = FindName(*get_TargetName());

            if(pTarget != NULL)
            {
                if(pTarget->IsType(GET_TYPE(Frame)))
                {
                    ((Frame*)pTarget)->Navigate(*(get_NavigateUri()));
                }
                else if(pTarget->IsType(GET_TYPE(NavigationWindow)))
                {
                    ((NavigationWindow*)pTarget)->Navigate(*(get_NavigateUri()));
                }
            }
        }
    }

    Button::OnClick();
}



};
