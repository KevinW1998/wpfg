#pragma once

#include <AVUIButtonBase.h>

namespace AVUI {

class ControlTemplate;
class DispatcherTimer;
class RepeatButton : public ButtonBase
{
public:
    RepeatButton() { };
    ~RepeatButton() { };

    DECLARE_ELEMENT(RepeatButton, ButtonBase);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Delay, int, GetKeyboardDelay(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Interval, int, GetKeyboardSpeed(), PropertyFlag::None);

    static void StaticConstructor();

protected:

    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs);
    virtual void OnMouseEnter(MouseEventArgs* pArgs);
    virtual void OnMouseLeave(MouseEventArgs* pArgs);
    virtual void OnKeyDown(KeyEventArgs* pArgs);


private:

    static int GetKeyboardDelay();
    static int GetKeyboardSpeed();

 

    void StopTimer();
    void StartTimer();
    void OnTimeout(Object* pSender, EventArgs* pArgs);

    bool HandleIsMouseOverChanged();

    TRefCountedPtr<DispatcherTimer> m_pTimer;
};

}; // namespace AVUI
