#include <AVUICommon.h>
#include <AVUITransitioningPage.h>
#include <AVUINavigationWindow.h>
#include <AVUIDispatcherTimer.h>

REGISTER_ELEMENT_BEGIN(AVUI::TransitioningPage)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(TransitionInDelay)
    REGISTER_PROPERTY(TransitionOutDelay)
    REGISTER_ROUTED_EVENT(TransitionIn)
    REGISTER_ROUTED_EVENT(TransitionOut)
REGISTER_ELEMENT_END()

namespace AVUI {


}; // namespace AVUI
