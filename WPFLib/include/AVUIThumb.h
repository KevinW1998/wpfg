#pragma once

#include <AVUIControl.h>

namespace AVUI {

class DragStartedEventArgs;
class DragDeltaEventArgs;
class DragCompletedEventArgs;

class Thumb : public Control
{
public:
    Thumb() { };
    ~Thumb() { };

    DECLARE_ELEMENT(Thumb, Control);

    IMPLEMENT_ROUTED_EVENT(DragStarted, DragStartedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(DragDelta, DragDeltaEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_ROUTED_EVENT(DragCompleted, DragCompletedEventArgs, RoutingStrategy::Bubble);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsDragging, bool, false, PropertyFlag::None);

protected:

    virtual void OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs);
    virtual void OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs);
    virtual void OnMouseMove(MouseEventArgs* pArgs);

private:
    Point m_originThumbPoint;
    Point m_previousPosition;
};



class DragStartedEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(DragStartedEventArgs, RoutedEventArgs);

    DragStartedEventArgs(float horizontalOffset, float verticalOffset) :  m_horizontalOffset(horizontalOffset), m_verticalOffset(verticalOffset) 
    { 
        set_RoutedEvent(&Thumb::get_DragStartedEvent());
    }

    float get_HorizontalOffset() { return m_horizontalOffset; }
    float get_VerticalOffset() { return m_verticalOffset; }
private:
    float m_horizontalOffset;
    float m_verticalOffset;
};

class DragCompletedEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(DragCompletedEventArgs, RoutedEventArgs);

    DragCompletedEventArgs(float horizontalChange, float verticalChange, bool canceled) :  m_horizontalChange(horizontalChange), m_verticalChange(verticalChange), m_wasCanceled(canceled)
    {
        set_RoutedEvent(&Thumb::get_DragCompletedEvent());
    }

    float get_HorizontalChange() { return m_horizontalChange; }
    float get_VerticalChange() { return m_verticalChange; }
    bool get_Canceled() { return m_wasCanceled; }

private:
    float m_horizontalChange;
    float m_verticalChange;
    bool m_wasCanceled;
};


class DragDeltaEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(DragDeltaEventArgs, RoutedEventArgs);

    DragDeltaEventArgs(float horizontalChange, float verticalChange) :  m_horizontalChange(horizontalChange), m_verticalChange(verticalChange)
    {
        set_RoutedEvent(&Thumb::get_DragDeltaEvent());
    }

    float get_HorizontalChange() { return m_horizontalChange; }
    float get_VerticalChange() { return m_verticalChange; }
private:
    float m_horizontalChange;
    float m_verticalChange;
};




DEFINE_EVENTHANDLER(DragDelta);
DEFINE_EVENTHANDLER(DragStarted);
DEFINE_EVENTHANDLER(DragCompleted);


}; // namespace AVUI
