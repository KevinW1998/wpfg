#include <AVUICommon.h>
#include <AVUICommandManager.h>
#include <AVUICommandBinding.h>
#include <AVUIUIElement.h>
#include <AVUIRoutedCommand.h>

REGISTER_ELEMENT(AVUI::CommandManager);

namespace AVUI {

void CommandManager::OnExecuted(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    if(pSender != NULL && pArgs != NULL)
    {
        CommandManager::FindCommandBinding(pSender, pArgs, pArgs->get_Command(), true);
    }
}

void CommandManager::FindCommandBinding(Object* pSender, ExecutedRoutedEventArgs* pArgs, const RoutedCommand& command, bool executeCommand)
{
    bool isHandled = false;

    if(pSender->IsType(GET_TYPE(UIElement)))
    {
        ItemStructList<TRefCountedPtr<CommandBinding> >* pBindings = ((UIElement*) pSender)->get_CommandBindings();
        if(pBindings != NULL)
        {
            for(int i = 0; i < pBindings->get_Count(); i++)
            {
                CommandBinding& commandBinding = *((*pBindings)[i]);
                if(commandBinding.get_Command() == command)
                {
                    if(CommandManager::ExecuteCommandBinding(pSender, pArgs, &commandBinding))
                    {
                        isHandled = true;
                        break;
                    }
                }
            }
        }
    }

    const Type* pTypeCur = &(pSender->get_Type());

    ItemStructMap<ItemStructList<TRefCountedPtr<CommandBinding> > >& classCommandHandlers = *(get_ClassCommandHandlers());

    while(pTypeCur != NULL && !isHandled)
    {
        int entryIndex = classCommandHandlers.Search(pTypeCur->get_TypeID());
        if(entryIndex >= 0)
        {
            ItemStructList<TRefCountedPtr<CommandBinding> >& commandList = classCommandHandlers[entryIndex];

            for(int i = 0; i < commandList.get_Count(); i++)
            {
                if(commandList[i]->get_Command() == command)
                {
                    if(CommandManager::ExecuteCommandBinding(pSender, pArgs, commandList[i]))
                    {
                        isHandled = true;
                        break;
                    }
                }
            }
        }
        pTypeCur = pTypeCur->get_BaseType();
    }



}

bool CommandManager::ExecuteCommandBinding(Object* pSender, ExecutedRoutedEventArgs* pArgs, CommandBinding* pCommandBinding)
{
    pCommandBinding->OnExecuted(pSender, pArgs);

    return pArgs->get_IsHandled();
}

void CommandManager::RegisterClassHandler(const Type& type, CommandBinding* pCommandBinding)
{
    ItemStructMap<ItemStructList<TRefCountedPtr<CommandBinding> > >& classCommandHandlers = *(get_ClassCommandHandlers());
    int entryIndex = classCommandHandlers.EnsureEntry(type.get_TypeID());

    classCommandHandlers[entryIndex].AddItem(pCommandBinding);
}

void CommandManager::TranslateInput(DependencyObject* pTargetElement, RoutedEventArgs* pArgs)
{

}


void CommandManager::CleanupStaticAllocations()
{
    ItemStructMap<ItemStructList<TRefCountedPtr<CommandBinding> > >& classCommandHandlers = *(get_ClassCommandHandlers());

    classCommandHandlers.EraseData();
}


}; // namespace AVUI
