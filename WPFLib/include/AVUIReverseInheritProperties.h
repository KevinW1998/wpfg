#pragma once

#include <AVUIReverseInheritProperty.h>

namespace AVUI {

class FocusWithinProperty : public ReverseInheritProperty
{
public:
    FocusWithinProperty();

    virtual void FireNotifications(UIElement* pUIE, bool oldValue);
};

class MouseCaptureWithinProperty : public ReverseInheritProperty
{
public:
    MouseCaptureWithinProperty();

    virtual void FireNotifications(UIElement* pUIE, bool oldValue);
};

class MouseOverProperty : public ReverseInheritProperty
{
public:
    MouseOverProperty();

    virtual void FireNotifications(UIElement* pUIE, bool oldValue);
};

}; // namespace AVUI

