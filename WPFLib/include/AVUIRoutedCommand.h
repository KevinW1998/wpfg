#pragma once

namespace AVUI {

class RoutedCommand;
class ExecutedRoutedEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(ExecutedRoutedEventArgs, RoutedEventArgs);

    ExecutedRoutedEventArgs(const RoutedCommand& routedCommand, Object* pArgument) : m_routedCommand(routedCommand), m_pArgument(pArgument), RoutedEventArgs(NULL) { }

    const RoutedCommand& get_Command() { return m_routedCommand; }
    Object* get_Argument() { return m_pArgument; }
private:
    const RoutedCommand& m_routedCommand;
    ObjectPtr m_pArgument;
};




class UIElement;
class RoutedCommand : public Object
{
public:
    RoutedCommand(const char* szName, const Type& ownerType) : m_szName(szName), m_ownerType(ownerType) { }

    DECLARE_ELEMENT(RoutedCommand, Object);

    void Execute(Object* pParameter, UIElement* pTarget);

    const char* get_Name() const { return m_szName; }
    const Type& get_OwnerType() const { return m_ownerType; }

    bool operator==(const RoutedCommand& rhs) const
    {
        if(this == &rhs)
        {
            return true;
        }

        return false;
    }

    bool operator!=(const RoutedCommand& rhs) const
    {
        return !((*this) == rhs);
    }

private:

    RoutedCommand(const RoutedCommand&);

    const char* m_szName;
    const Type& m_ownerType;
};

DEFINE_EVENTHANDLER(ExecutedRouted);

};
