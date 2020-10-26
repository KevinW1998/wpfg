#pragma once


#include <AVUIRoutedEventArgs.h>
#include <AVUIKeyboardNavigation.h>

namespace AVUI {

class UIElement;
class Keyboard;
class Control;

class KeyEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(KeyEventArgs, RoutedEventArgs);

    KeyEventArgs(const RoutedEvent* pRoutedEvent, Keyboard* pDevice, Key::Enum key) : RoutedEventArgs(pRoutedEvent), m_key(key), m_pDevice(pDevice) { }

    Key::Enum get_Key() { return m_key; }
    Key::Enum get_SystemKey() { return m_key; } // TODO -- This is more complicated, lets get back to it if necessary
    Keyboard* get_Device() { return m_pDevice; }

private:

    Keyboard* m_pDevice;
    Key::Enum m_key;
};

class KeyCharEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(KeyCharEventArgs, RoutedEventArgs);

    KeyCharEventArgs(const RoutedEvent* pRoutedEvent, Key::Enum key, WCHAR wch) : RoutedEventArgs(pRoutedEvent), m_key(key), m_wch(wch) { }

    WCHAR get_Char() { return m_wch; }
    Key::Enum get_Key() { return m_key; }

private:
    Key::Enum m_key;
    WCHAR m_wch;
};



class Keyboard : public Object
{
public:
    Keyboard() : m_isLeftShiftDown(false), m_isRightShiftDown(false), m_isLeftCtrlDown(false), m_isRightCtrlDown(false),
                       m_isLeftAltDown(false), m_isRightAltDown(false) { }

    DECLARE_ELEMENT(Keyboard, Object);
    IMPLEMENT_ROUTED_EVENT(KeyDown, KeyEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(KeyUp, KeyEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(KeyChar, KeyCharEventArgs, RoutingStrategy::Bubble);

    bool KeyDown(Key::Enum keyDown);
    bool KeyUp(Key::Enum keyUp);
    bool KeyChar(Key::Enum keyChar, WCHAR wch);

    void set_KeyboardFocus(Control* pControl);

    ModifierKeys::Enum get_KeyboardModifiers();

    static bool IsFocusable(DependencyObject* pElement);

    KeyboardNavigation* get_KeyboardNavigation() { return &m_navigation; }

    static Keyboard* get_Current();
    Control* get_FocusedElement() { return m_pKeyboardFocus; }

private:

    void AddEventHandler(const void* pvKey, Delegate* pEventHandler);
    void RemoveEventHandler(const void* pvKey, Delegate* pEventHandler);
    void InvokeEventHandler(const void* pvKey, Object* pSender, Object* pArgs);


    TWeakPtr<Control> m_pKeyboardFocus;
    KeyboardNavigation m_navigation;

    bool m_isLeftShiftDown : 1;
    bool m_isRightShiftDown : 1;
    bool m_isLeftCtrlDown : 1;
    bool m_isRightCtrlDown : 1;
    bool m_isLeftAltDown : 1;
    bool m_isRightAltDown : 1;


};

DEFINE_EVENTHANDLER(Key);
DEFINE_EVENTHANDLER(KeyChar);


}; // namespace AVUI
