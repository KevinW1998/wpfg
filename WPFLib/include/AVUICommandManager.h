#include <AVUIEventableObject.h>

namespace AVUI {

class ExecutedRoutedEventArgs;
class RoutedCommand;
class CommandBinding;

class CommandManager : public EventableObject
{
friend class UIElement;

public:
    CommandManager() { }

    DECLARE_ELEMENT(CommandManager, Object);
    IMPLEMENT_ROUTED_EVENT(PreviewExecuted,  ExecutedRoutedEventArgs, RoutingStrategy::Tunnel);
    IMPLEMENT_ROUTED_EVENT(Executed,  ExecutedRoutedEventArgs, RoutingStrategy::Bubble);

    static void RegisterClassHandler(const Type& type, CommandBinding* pCommandBinding);
    static void CleanupStaticAllocations();

private:

    static void TranslateInput(DependencyObject* pTargetElement, RoutedEventArgs* pArgs);

    static ItemStructMap<ItemStructList<TRefCountedPtr<CommandBinding> > >* get_ClassCommandHandlers()
    {
        static ItemStructMap<ItemStructList<TRefCountedPtr<CommandBinding> > > s_classCommandHandlers;

        return &s_classCommandHandlers;
    }

    static void OnExecuted(Object* pSender, ExecutedRoutedEventArgs* pArgs);
    static void FindCommandBinding(Object* pSender, ExecutedRoutedEventArgs* pArgs, const RoutedCommand& command, bool executeCommand);
    static bool ExecuteCommandBinding(Object* pSender, ExecutedRoutedEventArgs* pArgs, CommandBinding* pCommandBinding);
};

};
