#include <Windows.h>
#include <AVUICommon.h>
#include <XInput.h>
#include <D3DInput.h>
#include <AVUIDispatcher.h>
#include <AVUIInputManager.h>

static AVUI::Key::Enum InputToKey(int buttonID)
{
    using namespace AVUI;

    switch(buttonID)
    {
    case GamepadInput::LeftThumbstickUp:
        return Key::Up;

    case GamepadInput::LeftThumbstickDown:
        return Key::Down;

    case GamepadInput::LeftThumbstickLeft:
        return Key::Left;

    case GamepadInput::LeftThumbstickRight:
        return Key::Right;

    case GamepadInput::RightThumbstickUp:
    case GamepadInput::RightThumbstickDown:
    case GamepadInput::RightThumbstickLeft:
    case GamepadInput::RightThumbstickRight:
        return Key::None;

    case GamepadInput::DPadUp:
        return Key::Up;
    case GamepadInput::DPadDown:
        return Key::Down;
    case GamepadInput::DPadLeft:
        return Key::Left;
    case GamepadInput::DPadRight:
        return Key::Right;
    case GamepadInput::A:
        return Key::Enter;
    case GamepadInput::B:
        return Key::BrowserBack;
    case GamepadInput::X:
    case GamepadInput::Y:
    case GamepadInput::LeftShoulder:
    case GamepadInput::RightShoulder:
    case GamepadInput::LeftTrigger:
    case GamepadInput::RightTrigger:
    case GamepadInput::Start:
        return Key::None;

    case GamepadInput::Back:
        return Key::BrowserBack;

    default:
        return Key::None;
    }
}

static DWORD GetRepeatTime(int buttonID)
{
    switch(buttonID)
    {
    case GamepadInput::LeftThumbstickUp:
    case GamepadInput::LeftThumbstickDown:
    case GamepadInput::LeftThumbstickLeft:
    case GamepadInput::LeftThumbstickRight:
        return 600;
    case GamepadInput::DPadUp:
    case GamepadInput::DPadDown:
    case GamepadInput::DPadLeft:
    case GamepadInput::DPadRight:
    case GamepadInput::A:
    case GamepadInput::B:
    case GamepadInput::X:
    case GamepadInput::Y:
    case GamepadInput::LeftShoulder:
    case GamepadInput::RightShoulder:
    case GamepadInput::LeftTrigger:
    case GamepadInput::RightTrigger:
    case GamepadInput::Start:
    case GamepadInput::Back:
        return 250;
    }

    return 0;
}

void ControllerXInputManager::Update()
{
    DWORD dwTime = timeGetTime();
    XINPUT_STATE state;

    for(int idx = 0; idx < MAX_CONTROLLER; idx++)
    {
        HRESULT hr = XInputGetState(idx, &state);

        if(hr == S_OK)
        {
            m_rgState[idx].UpdateState(&state, dwTime);
        }
        else
        {
            m_rgState[idx].ClearState();
        }
    }
}

void ControllerXInputState::UpdateButtonState(GamepadInput::Enum buttonID, 
                                              bool wasButtonDown, DWORD buttonDownTime,
                                              bool isButtonDown, DWORD currentTime)
{
    if(wasButtonDown == isButtonDown)
    {
        if(isButtonDown && (currentTime - buttonDownTime) > GetRepeatTime(buttonID))
        {
            m_currentGamepadState.TrackedButtons[buttonID].ButtonDownTime = currentTime;
            AVUI::Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageKeyDown(InputToKey(buttonID));
        }
    }
    else
    {
        m_currentGamepadState.TrackedButtons[buttonID].IsButtonDown = isButtonDown;

        if(isButtonDown)
        {
            m_currentGamepadState.TrackedButtons[buttonID].ButtonDownTime = currentTime;

            AVUI::Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageKeyDown(InputToKey(buttonID));
        }
        else
        {
            AVUI::Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageKeyUp(InputToKey(buttonID));
        }
    }
}

bool ControllerXInputState::GetButtonState(const XINPUT_STATE* pState, GamepadInput::Enum buttonID)
{
    switch(buttonID)
    {
    case GamepadInput::LeftThumbstickUp:
    {
        int ly = pState->Gamepad.sThumbLY;

        return ly > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && ly > abs(pState->Gamepad.sThumbLX); 
    }
        
    case GamepadInput::LeftThumbstickDown:
    {
        int ly = -(pState->Gamepad.sThumbLY);

        return ly > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && ly > abs(pState->Gamepad.sThumbLX); 
    }

    case GamepadInput::LeftThumbstickLeft:
    {
        int lx = -(pState->Gamepad.sThumbLX);

        return lx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && lx > abs(pState->Gamepad.sThumbLY); 
    }

    case GamepadInput::LeftThumbstickRight:
    {
        int lx = pState->Gamepad.sThumbLX;

        return lx > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && lx > abs(pState->Gamepad.sThumbLY); 
    }

    case GamepadInput::RightThumbstickUp:
    {
        int ly = pState->Gamepad.sThumbRY;

        return ly > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && ly > abs(pState->Gamepad.sThumbRX); 
    }

    case GamepadInput::RightThumbstickDown:
    {
        int ly = -(pState->Gamepad.sThumbRY);

        return ly > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && ly > abs(pState->Gamepad.sThumbRX); 
    }

    case GamepadInput::RightThumbstickLeft:
    {
        int lx = -(pState->Gamepad.sThumbRX);

        return lx > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && lx > abs(pState->Gamepad.sThumbRY); 
    }

    case GamepadInput::RightThumbstickRight:
    {
        int lx = pState->Gamepad.sThumbRX;

        return lx > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE && lx > abs(pState->Gamepad.sThumbRY); 
    }

    case GamepadInput::DPadUp:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
    case GamepadInput::DPadDown:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
    case GamepadInput::DPadLeft:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
    case GamepadInput::DPadRight:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
    case GamepadInput::A:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
    case GamepadInput::B:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
    case GamepadInput::X:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
    case GamepadInput::Y:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
    case GamepadInput::LeftShoulder:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
    case GamepadInput::RightShoulder:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
    case GamepadInput::LeftTrigger:
        return (pState->Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) != 0;
    case GamepadInput::RightTrigger:
        return (pState->Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) != 0;
    case GamepadInput::Start:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
    case GamepadInput::Back:
        return (pState->Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;

    default:
        return false;
    }
}


void ControllerXInputState::UpdateState(const XINPUT_STATE* pState, DWORD dwTimeStamp)
{
    for(int idx = 0; idx < GamepadInput::Count; idx++)
    {
        UpdateButtonState((GamepadInput::Enum)idx, m_currentGamepadState.TrackedButtons[idx].IsButtonDown, m_currentGamepadState.TrackedButtons[idx].ButtonDownTime, 
                          GetButtonState(pState, (GamepadInput::Enum) idx), dwTimeStamp);
    }
}

void ControllerXInputState::ClearState()
{
    m_isCurrentStateValid = false;
    ZeroMemory(&m_currentGamepadState, sizeof(m_currentGamepadState));
}


