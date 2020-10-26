#include <AVUICommon.h>
#include <AVUIRoutedCommand.h>
#include <AVUICommandManager.h>
#include <AVUIUIElement.h>

REGISTER_ELEMENT(AVUI::ExecutedRoutedEventArgs);
REGISTER_ELEMENT(AVUI::RoutedCommand);

namespace AVUI {

void RoutedCommand::Execute(Object* pParameter, UIElement* pTarget)
{
    if(pTarget == NULL)
    {
        LibraryCriticalError();
    }

    TRefCountedPtr<ExecutedRoutedEventArgs> pArgs = object_allocate<ExecutedRoutedEventArgs>(*this, pParameter);

    pArgs->set_RoutedEvent(&(CommandManager::get_PreviewExecutedEvent()));
    pTarget->RaiseEvent(pArgs);
    if(!pArgs->get_IsHandled())
    {
        pArgs->set_RoutedEvent(&(CommandManager::get_ExecutedEvent()));
        pTarget->RaiseEvent(pArgs);
    }
}


}; // namespace AVUI
