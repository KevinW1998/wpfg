#include <AVUICommon.h>
#include <AVUIBeginStoryboard.h>
#include <AVUIUIElement.h>
#include <AVUIStoryboard.h>
#include <AVUIClockGroup.h>

REGISTER_ELEMENT_BEGIN(AVUI::BeginStoryboard)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Storyboard);
    REGISTER_PROPERTY(HandoffBehavior);
REGISTER_ELEMENT_END()

namespace AVUI {

void BeginStoryboard::Invoke(DependencyObject* pTarget, ControlTemplate* pTemplate)
{
    Storyboard* pStoryboard = get_Storyboard();

    if(pStoryboard == NULL)
    {
        LibraryCriticalError();
    }

    pStoryboard->BeginCommon(pTarget, pTemplate, get_HandoffBehavior(), false, 1);
}

void BeginStoryboard::Seal()
{
    DependencyObject::Seal();
}


}; // namespace AVUI
