#pragma once

#include <AVUIContentControl.h>

namespace AVUI {
class RoutedCommand;

class ButtonBase : public ContentControl
{
public:
    ButtonBase() : m_isSpaceKeyDown(false) { };
    ~ButtonBase() { };

    DECLARE_ELEMENT(ButtonBase, ContentControl);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsPressed, bool, false, DependencyPropertyMetadata(PropertyFlag::None, ButtonBase::OnIsPressedChanged));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Command, RoutedCommand, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(CommandParameter, Object, NULL, PropertyFlag::None);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(ClickMode, ClickMode, ClickMode::Release, PropertyFlag::None);

    IMPLEMENT_ROUTED_EVENT(Click, RoutedEventArgs, RoutingStrategy::Bubble);


protected:

    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs);
    virtual void OnMouseEnter(MouseEventArgs* pArgs);
    virtual void OnMouseLeave(MouseEventArgs* pArgs);
    virtual void OnMouseMove(MouseEventArgs* pArgs);
    virtual void OnLostMouseCapture(MouseEventArgs* pArgs);

    virtual void OnKeyUp(KeyEventArgs* pArgs);
    virtual void OnKeyDown(KeyEventArgs* pArgs);

    virtual void OnClick();
    virtual void OnIsPressedChanged(const DependencyPropertyChangedEventArgs* pArgs) { }

private:

    static void OnIsPressedChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e) { ((ButtonBase*)pDO)->OnIsPressedChanged(&e); }
    bool GetMouseLeftButtonReleased();
    bool HandleIsMouseOverChanged();   
    bool get_IsSpaceKeyDown() const { return m_isSpaceKeyDown; }
    void set_IsSpaceKeyDown(bool isSpaceKeyDown) { m_isSpaceKeyDown = isSpaceKeyDown; }

    void SetIsPressed(bool pressed);
    void UpdateIsPressed();

    bool m_isSpaceKeyDown;

};


}; // namespace AVUI
