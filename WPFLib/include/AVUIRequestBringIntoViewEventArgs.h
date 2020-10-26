
#pragma once

namespace AVUI {

class RoutedEvent;
class Visual;

class RequestBringIntoViewEventArgs : public RoutedEventArgs
{
public:
    DECLARE_ELEMENT(RequestBringIntoViewEventArgs, RoutedEventArgs);

    RequestBringIntoViewEventArgs(Visual* pVisual) : RoutedEventArgs(NULL), m_pVisual(pVisual), m_hasTargetRect(false) { }
    RequestBringIntoViewEventArgs(Visual* pVisual, const Rect& rect) : RoutedEventArgs(NULL), m_pVisual(pVisual), m_rect(rect), m_hasTargetRect(true) { }

    Visual* get_Target() { return m_pVisual; }
    Rect get_TargetRect() { return m_rect; }
    bool get_HasTargetRect() { return m_hasTargetRect; }

private:

    Visual* m_pVisual;
    Rect m_rect;
    bool m_hasTargetRect;

};

DEFINE_EVENTHANDLER(RequestBringIntoView);

}; 


