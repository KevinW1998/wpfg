#pragma once

#include <AVUIEventableObject.h>

namespace AVUI {

class RoutedCommand;
class ExecutedRoutedEventArgs;
class CommandBinding : public EventableObject
{
    friend class CommandManager;
public:
    CommandBinding(const RoutedCommand& command, TDelegate<ExecutedRoutedEventArgs>* pExecuted) : m_command(command) 
    { 
        AddEventHandler(&(get_ExecutedEvent()), pExecuted);
    }
                                        
    DECLARE_ELEMENT(CommandBinding, Object);

    IMPLEMENT_EVENT(Executed, ExecutedRoutedEventArgs);
    IMPLEMENT_EVENT(PreviewExecuted, ExecutedRoutedEventArgs);

    const RoutedCommand& get_Command() const  { return m_command; }

private:

    void OnExecuted(Object* pSender, ExecutedRoutedEventArgs* pArgs);

    const RoutedCommand& m_command;
};

};
