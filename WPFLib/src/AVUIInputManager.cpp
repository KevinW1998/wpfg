#include <AVUICommon.h>
#include <AVUIInputManager.h>
#include <AVUIMouse.h>
#include <AVUIKeyboard.h>
#include <AVUIUIElement.h>
#include <AVUIDispatcher.h>
#include <AVUIPopupControlService.h>

namespace AVUI {

InputManager* InputManager::get_Current()
{
    return Dispatcher::get_CurrentDispatcher()->get_InputManager();
}

InputManager::InputManager()
{
    m_pMouseDevice = new Mouse();
    m_pKeyboardDevice = new Keyboard();
    m_pPopupControlService = new PopupControlService();

    m_isInputLocked = false;
    m_hasHitTestInvalidatedAsyncOperation = false;
}

InputManager::~InputManager()
{
    delete m_pKeyboardDevice;
    delete m_pMouseDevice;
    delete m_pPopupControlService;
}

void InputManager::Update()
{

}

bool InputManager::MessageKeyDown(Key::Enum keyDown)
{
    if(m_isInputLocked)
    {
        return true;
    }

    return m_pKeyboardDevice->KeyDown(keyDown);
}

bool InputManager::MessageKeyUp(Key::Enum keyUp)
{
    if(m_isInputLocked)
    {
        return true;
    }

    return m_pKeyboardDevice->KeyUp(keyUp);
}
bool InputManager::MessageKeyChar(Key::Enum keyChar, WCHAR wch)
{
    if(m_isInputLocked)
    {
        return true;
    }

    return m_pKeyboardDevice->KeyChar(keyChar, wch);
}

bool InputManager::MessageMouseMove(int xDelta, int yDelta, int zDelta)
{
    if(m_isInputLocked)
    {
        return false;
    }
    return m_pMouseDevice->MessageMouseMove(xDelta, yDelta, zDelta);
}
bool InputManager::MessageMousePosition(int xPosition, int yPosition, int zPosition)
{
    if(m_isInputLocked)
    {
        return false;
    }
    return m_pMouseDevice->MessageMousePosition(xPosition, yPosition, zPosition);
}

bool InputManager::MessageMouseButtonDown(MouseButton::Enum mouseButton)
{
    if(m_isInputLocked)
    {
        return false;
    }
    return m_pMouseDevice->MessageMouseButtonDown(mouseButton);
}
bool InputManager::MessageMouseButtonUp(MouseButton::Enum mouseButton)
{
    if(m_isInputLocked)
    {
        return false;
    }
    return m_pMouseDevice->MessageMouseButtonUp(mouseButton);
}

Point InputManager::get_MousePosition(UIElement* pElementRelativeTo)
{
    return m_pMouseDevice->get_MousePosition(pElementRelativeTo);
}

bool InputManager::CaptureMouse(UIElement* pUIElementCapture)
{
    return m_pMouseDevice->CaptureMouse(pUIElementCapture);
}

UIElement* InputManager::get_MouseCapture()
{
    return m_pMouseDevice->get_MouseCapture();
}

void InputManager::ReleaseMouseCapture(UIElement* pUIElementCapture)
{
    m_pMouseDevice->ReleaseMouseCapture(pUIElementCapture);
}

void InputManager::set_KeyboardFocus(Control* pControl)
{
    m_pKeyboardDevice->set_KeyboardFocus(pControl);
}

ModifierKeys::Enum InputManager::get_KeyboardModifiers()
{
    return m_pKeyboardDevice->get_KeyboardModifiers();
}


void InputManager::UpdateMouseOver()
{
    m_pMouseDevice->UpdateMouseOver();
}

void InputManager::UpdateCursor()
{
    m_pMouseDevice->UpdateCursor();
}

void InputManager::NotifyHitTestInvalidated()
{
    if(!m_hasHitTestInvalidatedAsyncOperation)
    {
        m_hasHitTestInvalidatedAsyncOperation = true;
        Dispatcher::get_CurrentDispatcher()->BeginInvoke(DispatcherPriority::Render, 
                                                         DispatcherOperationCallback::Create(this, &InputManager::HitTestInvalidatedAsyncCallback), NULL);       
    }
}

void InputManager::HitTestInvalidatedAsyncCallback(Object* pSender, Object* pArg)
{
    UpdateMouseOver();
    UpdateCursor();

    m_hasHitTestInvalidatedAsyncOperation = false;
}



void InputManager::PostProcessInput(MouseEventArgs* pArgs)
{
    m_pPopupControlService->PostProcessInput(pArgs);
}



}; // namespace AVUI
