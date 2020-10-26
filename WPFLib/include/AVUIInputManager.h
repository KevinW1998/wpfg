#pragma once

namespace AVUI {

class Mouse;
class Keyboard;
class UIElement;
class Control;
class MouseEventArgs;
class PopupControlService;

class InputManager
{
public:
    static InputManager* get_Current();   

    InputManager();
    ~InputManager();

    void Update();

    bool MessageKeyDown(Key::Enum keyDown);
    bool MessageKeyUp(Key::Enum keyUp);
    bool MessageKeyChar(Key::Enum keyChar, WCHAR wch);
    bool MessageMouseMove(int xDelta, int yDelta, int zDelta);
    bool MessageMousePosition(int xPosition, int yPosition, int zPosition);
    bool MessageMouseButtonDown(MouseButton::Enum buttonDown);
    bool MessageMouseButtonUp(MouseButton::Enum buttonUp);

    Point get_MousePosition(UIElement* pElementRelativeTo);
    bool CaptureMouse(UIElement* pUIElementCapture);
    UIElement* get_MouseCapture();
    void ReleaseMouseCapture(UIElement* pUIElementCapture);   
    void set_KeyboardFocus(Control* pControl);
    void NotifyHitTestInvalidated();
    ModifierKeys::Enum get_KeyboardModifiers();


    void LockInputs() { m_isInputLocked = true; }
    void UnlockInputs() { m_isInputLocked = false; UpdateMouseOver(); }
    void UpdateMouseOver();
    void UpdateCursor();

    void PostProcessInput(MouseEventArgs* pArgs);

    Mouse* get_Mouse() { return m_pMouseDevice; }
    Keyboard* get_Keyboard() { return m_pKeyboardDevice; }
    PopupControlService* get_PopupControlService() { return m_pPopupControlService; }

private:

    void HitTestInvalidatedAsyncCallback(Object* pSender, Object* pArg);

    bool m_isInputLocked;
    bool m_hasHitTestInvalidatedAsyncOperation;

    Mouse* m_pMouseDevice;
    Keyboard* m_pKeyboardDevice;
    PopupControlService* m_pPopupControlService;
};

};
