#include <AVUICommon.h>
#include <AVUIRoutedCommand.h>
#include <AVUICommandBinding.h>
#include <AVUICommandManager.h>

REGISTER_ELEMENT(AVUI::CommandBinding);

namespace AVUI {

void CommandBinding::OnExecuted(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    if(!pArgs->get_IsHandled())
    {
        if(pArgs->get_RoutedEvent() == &(CommandManager::get_ExecutedEvent()))
        {
            invoke_Executed(pSender, pArgs);
            pArgs->set_IsHandled(true);
        }
        else if(pArgs->get_RoutedEvent() == &(CommandManager::get_PreviewExecutedEvent()))
        {
            invoke_PreviewExecuted(pSender, pArgs);
            pArgs->set_IsHandled(true);
        }
    }
}

}; // namespace AVUI
