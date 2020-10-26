#pragma once

#include <AVUIRoutedEvent.h>
#include <AVUIRoutedEventArgs.h>

namespace AVUI {

template <class T>
class RoutedPropertyChangedEventArgs : public RoutedEventArgs
{
private:

    T m_newValue;
    T m_oldValue;

public:
    RoutedPropertyChangedEventArgs(T oldValue, T newValue) : m_oldValue(oldValue), m_newValue(newValue) { }
    RoutedPropertyChangedEventArgs(T oldValue, T newValue, const RoutedEvent* pRoutedEvent) : m_oldValue(oldValue), m_newValue(newValue), RoutedEventArgs(pRoutedEvent) { }

    // Properties
    T get_NewValue() { return m_newValue; }
    T get_OldValue() { return m_oldValue; }

};

class RoutedPropertyChangedBoolEventArgs : public RoutedPropertyChangedEventArgs<bool>
{
public:
    RoutedPropertyChangedBoolEventArgs(bool oldValue, bool newValue) : RoutedPropertyChangedEventArgs<bool>(oldValue, newValue) { }
    RoutedPropertyChangedBoolEventArgs(bool oldValue, bool newValue, const RoutedEvent* pRoutedEvent) : RoutedPropertyChangedEventArgs<bool>(oldValue, newValue, pRoutedEvent) { }

    DECLARE_ELEMENT(RoutedPropertyChangedBoolEventArgs, RoutedEventArgs);
};

class RoutedPropertyChangedObjectEventArgs : public RoutedPropertyChangedEventArgs< TRefCountedPtr<Object> >
{
public:
    RoutedPropertyChangedObjectEventArgs(Object* pOldValue, Object* pNewValue) : RoutedPropertyChangedEventArgs< TRefCountedPtr<Object> >(pOldValue, pNewValue) { }
    RoutedPropertyChangedObjectEventArgs(Object* pOldValue, Object* pNewValue, const RoutedEvent* pRoutedEvent) : RoutedPropertyChangedEventArgs< TRefCountedPtr<Object> >(pOldValue, pNewValue, pRoutedEvent) { }

    DECLARE_ELEMENT(RoutedPropertyChangedObjectEventArgs, RoutedEventArgs);
};

DEFINE_EVENTHANDLER(RoutedPropertyChangedBool)
DEFINE_EVENTHANDLER(RoutedPropertyChangedObject)


}; // namespace AVUI


