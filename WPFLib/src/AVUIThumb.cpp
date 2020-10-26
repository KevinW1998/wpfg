#include <AVUICommon.h>
#include <AVUIThumb.h>
#include <AVUIMouse.h>
#include <AVUIInputManager.h>

REGISTER_ELEMENT_BEGIN(AVUI::Thumb)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IsDragging)
REGISTER_ELEMENT_END()

REGISTER_ELEMENT(AVUI::DragStartedEventArgs);
REGISTER_ELEMENT(AVUI::DragDeltaEventArgs);
REGISTER_ELEMENT(AVUI::DragCompletedEventArgs);

namespace AVUI {

void Thumb::OnMouseLeftButtonDown(MouseButtonEventArgs* pArgs)
{
    Control::OnMouseLeftButtonDown(pArgs);

    if(!get_IsDragging())
    {
        pArgs->set_IsHandled(true);
        Focus();
        CaptureMouse();
        set_IsDragging(true);


        m_originThumbPoint = get_InputManager()->get_MousePosition(this);
        m_previousPosition = get_InputManager()->get_MousePosition(NULL);

        DragStartedEventArgs dragStartedEventArgs(m_originThumbPoint.get_X(), m_originThumbPoint.get_Y());
        RaiseEvent(&dragStartedEventArgs);
    }
}

void Thumb::OnMouseLeftButtonUp(MouseButtonEventArgs* pArgs)
{
    Control::OnMouseLeftButtonUp(pArgs);
    if(get_IsMouseCaptured() && get_IsDragging())
    {
        pArgs->set_IsHandled(true);
        set_IsDragging(false);
        ReleaseMouseCapture();

        Point currentPosition = get_InputManager()->get_MousePosition(this);

        DragCompletedEventArgs dragCompletedEventArgs(currentPosition.get_X() - m_originThumbPoint.get_X(),
                                                      currentPosition.get_Y() - m_originThumbPoint.get_Y(), false);
        RaiseEvent(&dragCompletedEventArgs);
    }
}

void Thumb::OnMouseMove(MouseEventArgs* pArgs)
{
    Control::OnMouseMove(pArgs);
    if(get_IsMouseCaptured() && get_IsDragging())
    {
        pArgs->set_IsHandled(true);

        Point currentPosition = get_InputManager()->get_MousePosition(this);

        if(!Float::AreClose(currentPosition, m_previousPosition))
        {

            DragDeltaEventArgs dragDeltaEventArgs(currentPosition.get_X() - m_originThumbPoint.get_X(),
                                                  currentPosition.get_Y() - m_originThumbPoint.get_Y());
            RaiseEvent(&dragDeltaEventArgs);

            m_previousPosition = currentPosition;
        }

    }
}


}; // namespace AVUI
