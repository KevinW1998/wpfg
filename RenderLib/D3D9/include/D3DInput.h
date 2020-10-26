#pragma once

class GamepadInput
{
public:
    enum Enum
    {
        LeftThumbstickUp,
        LeftThumbstickDown,
        LeftThumbstickLeft,
        LeftThumbstickRight,
        RightThumbstickUp,
        RightThumbstickDown,
        RightThumbstickLeft,
        RightThumbstickRight,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        A,
        B,
        X,
        Y,
        LeftShoulder,
        RightShoulder,
        LeftTrigger,
        RightTrigger,
        Start,
        Back,

        Count   
    };
};


class ControllerXInputState
{
public:

    ControllerXInputState() : m_isCurrentStateValid(false) { ClearState(); }
    ~ControllerXInputState() { }

    void UpdateState(const XINPUT_STATE* pState, DWORD dwTimeStamp);
    void ClearState();

private:   

    struct TrackedButtonState
    {
        bool IsButtonDown;
        DWORD ButtonDownTime;
    };

    struct CookedGamepadState
    {
        TrackedButtonState TrackedButtons[GamepadInput::Count];
    };

    void ControllerXInputState::UpdateButtonState(GamepadInput::Enum buttonID, bool wasButtonDown, DWORD buttonDownTime, bool isButtonDown, DWORD currentTime);
    bool GetButtonState(const XINPUT_STATE* pState, GamepadInput::Enum buttonID);

    bool m_isCurrentStateValid;
    CookedGamepadState m_currentGamepadState;
    
};

class ControllerXInputManager
{
public:

    ControllerXInputManager() { }
    ~ControllerXInputManager() { }

    void Update();

private:

    static const int MAX_CONTROLLER = 4;

    ControllerXInputState m_rgState[MAX_CONTROLLER];

};

