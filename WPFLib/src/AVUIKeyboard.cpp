#include <AVUICommon.h>
#include <AVUIKeyboard.h>
#include <AVUIKeyboardNavigation.h>
#include <AVUIControl.h>
#include <AVUIInputManager.h>

REGISTER_ELEMENT_BEGIN(AVUI::Keyboard)
    REGISTER_ROUTED_EVENT(KeyDown);
    REGISTER_ROUTED_EVENT(KeyUp);
    REGISTER_ROUTED_EVENT(KeyChar);
REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::KeyEventArgs)

REGISTER_ELEMENT_END()

REGISTER_ELEMENT_BEGIN(AVUI::KeyCharEventArgs)

REGISTER_ELEMENT_END()

namespace AVUI {

bool Keyboard::KeyDown(Key::Enum keyDown)
{
    switch(keyDown)
    {
    case Key::LeftShift:
        m_isLeftShiftDown = true;
        break;

    case Key::RightShift:
        m_isRightShiftDown = true;
        break;

    case Key::LeftCtrl:
        m_isLeftCtrlDown = true;
        break;

    case Key::RightCtrl:
        m_isRightCtrlDown = true;
        break;

    case Key::LeftAlt:
        m_isLeftAltDown = true;
        break;

    case Key::RightAlt:
        m_isRightAltDown = true;
        break;
    }


    KeyEventArgs eventArgs(&get_KeyDownEvent(), this, keyDown);

    if(m_pKeyboardFocus != NULL)
    {
        m_pKeyboardFocus->RaiseEvent(&eventArgs);
    }

    if(!eventArgs.get_IsHandled())
    {
        UIElement* pFocusable = NULL;

        if(keyDown == Key::Left)
        {
            pFocusable = m_navigation.PredictFocusedElement(m_pKeyboardFocus, FocusNavigationDirection::Left);            
        }
        else if(keyDown == Key::Right)
        {
            pFocusable = m_navigation.PredictFocusedElement(m_pKeyboardFocus, FocusNavigationDirection::Right);            
        }
        else if(keyDown == Key::Up)
        {
            pFocusable = m_navigation.PredictFocusedElement(m_pKeyboardFocus, FocusNavigationDirection::Up);            
        }
        else if(keyDown == Key::Down)
        {
            pFocusable = m_navigation.PredictFocusedElement(m_pKeyboardFocus, FocusNavigationDirection::Down);            
        }

        if(pFocusable != NULL && pFocusable->IsType(GET_TYPE(Control)))
        {
            ((Control*)pFocusable)->Focus();
            eventArgs.set_IsHandled(true);
        }
    }

    return eventArgs.get_IsHandled();
}

bool Keyboard::KeyUp(Key::Enum keyUp)
{
    switch(keyUp)
    {
    case Key::LeftShift:
        m_isLeftShiftDown = false;
        break;

    case Key::RightShift:
        m_isRightShiftDown = false;
        break;

    case Key::LeftCtrl:
        m_isLeftCtrlDown = false;
        break;

    case Key::RightCtrl:
        m_isRightCtrlDown = false;
        break;

    case Key::LeftAlt:
        m_isLeftAltDown = false;
        break;

    case Key::RightAlt:
        m_isRightAltDown = false;
        break;
    }


    if(m_pKeyboardFocus == NULL)
    {
        return false;
    }

    KeyEventArgs eventArgs(&get_KeyUpEvent(), this, keyUp);
    m_pKeyboardFocus->RaiseEvent(&eventArgs);

    return eventArgs.get_IsHandled();
}

bool Keyboard::KeyChar(Key::Enum key, WCHAR wch)
{
    if(m_pKeyboardFocus == NULL)
    {
        return false;
    }

    KeyCharEventArgs eventArgs(&get_KeyCharEvent(), key, wch);
    m_pKeyboardFocus->RaiseEvent(&eventArgs);

    return eventArgs.get_IsHandled();
}

void Keyboard::set_KeyboardFocus(Control* pControl)
{
    if(pControl == m_pKeyboardFocus)
    {
        return;
    }

    if(m_pKeyboardFocus != NULL)
    {
        m_pKeyboardFocus->PrivateSetFocused(false);
    }

    m_pKeyboardFocus = pControl;

    if(m_pKeyboardFocus != NULL)
    {
        m_pKeyboardFocus->PrivateSetFocused(true);
    }

}

ModifierKeys::Enum Keyboard::get_KeyboardModifiers()
{
    int modifierKeys = 0;

    if(m_isLeftShiftDown || m_isRightShiftDown)
    {
        modifierKeys |= ModifierKeys::Shift;
    }

    if(m_isLeftCtrlDown || m_isRightCtrlDown)
    {
        modifierKeys |= ModifierKeys::Control;
    }

    if(m_isLeftAltDown || m_isRightAltDown)
    {
        modifierKeys |= ModifierKeys::Alt;
    }

    return (ModifierKeys::Enum) modifierKeys;
}

    // significant deviation from original.
bool Keyboard::IsFocusable(DependencyObject* pElement)
{
    if (pElement == NULL)
    {
        return false;
    }
    UIElement* pUIElement = object_cast<UIElement>(pElement);

    if(pUIElement == NULL)
    {
        return false;
    }
    if (!pUIElement->get_IsVisible())
    {
        return false;
    }
    if (!pUIElement->get_IsEnabled())
    {
        return false;
    }

    if(!pUIElement->IsType(GET_TYPE(Control)))
    {
        return false;
    }

    Control* pControl = (Control*) pUIElement;

    if(!pControl->get_Focusable())
    {
        return false;
    }

    return true;
}

Keyboard* Keyboard::get_Current()
{
    return InputManager::get_Current()->get_Keyboard();
}


}; // namespace AVUI
