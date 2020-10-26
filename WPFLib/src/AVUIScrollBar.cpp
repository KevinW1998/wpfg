#include <AVUICommon.h>
#include <AVUIScrollBar.h>
#include <AVUIThumb.h>
#include <AVUITrack.h>
#include <AVUIControlTemplate.h>
#include <AVUIScrollViewer.h>
#include <AVUIScrollContentPresenter.h>

REGISTER_ELEMENT_BEGIN(AVUI::ScrollBar)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Orientation)
    REGISTER_PROPERTY(ViewportSize)

    REGISTER_ROUTED_COMMAND(LineUpCommand)
    REGISTER_ROUTED_COMMAND(LineDownCommand)
    REGISTER_ROUTED_COMMAND(LineLeftCommand)
    REGISTER_ROUTED_COMMAND(LineRightCommand)
    REGISTER_ROUTED_COMMAND(PageUpCommand)
    REGISTER_ROUTED_COMMAND(PageDownCommand)
    REGISTER_ROUTED_COMMAND(PageLeftCommand)
    REGISTER_ROUTED_COMMAND(PageRightCommand)
    REGISTER_ROUTED_COMMAND(ScrollToEndCommand)
    REGISTER_ROUTED_COMMAND(ScrollToHomeCommand)
    REGISTER_ROUTED_COMMAND(ScrollToRightEndCommand)
    REGISTER_ROUTED_COMMAND(ScrollToLeftEndCommand)
    REGISTER_ROUTED_COMMAND(ScrollToTopCommand)
    REGISTER_ROUTED_COMMAND(ScrollToBottomCommand)
    REGISTER_ROUTED_COMMAND(ScrollToHorizontalOffsetCommand)
    REGISTER_ROUTED_COMMAND(ScrollToVerticalOffsetCommand)
    REGISTER_ROUTED_COMMAND(ScrollHereCommand)
    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
REGISTER_ELEMENT_END()

namespace AVUI {

void ScrollBar::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(ScrollBar), Thumb::get_DragStartedEvent(), DragStartedEventHandler::Create(OnDragStartedThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(ScrollBar), Thumb::get_DragCompletedEvent(), DragCompletedEventHandler::Create(OnDragCompletedThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(ScrollBar), Thumb::get_DragDeltaEvent(), DragDeltaEventHandler::Create(OnDragDeltaThunk), false);
}

void ScrollBar::OnDragStarted(DragStartedEventArgs* pArgs)
{

}
void ScrollBar::OnDragDelta(DragDeltaEventArgs* pArgs)
{
    if(m_pTrack != NULL)
    {
        float horizontalDragDelta = pArgs->get_HorizontalChange();
        float verticalDragDelta = pArgs->get_VerticalChange();

        float dragValue = m_pTrack->ValueFromDistance(horizontalDragDelta, verticalDragDelta);

        if(!Float::IsInfinity(dragValue) && !Float::IsNaN(dragValue) && !Float::IsZero(dragValue))
        {
            float currentValue = get_Value();
            float newValue = currentValue + dragValue;

            if (!Float::AreClose(currentValue, newValue))
            {
                  ChangeValue(newValue);
            }            
        }
    }
}

void ScrollBar::OnDragCompleted(DragCompletedEventArgs* pArgs)
{

}

void ScrollBar::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    ControlTemplate* pTemplate = get_Template();
    if(pTemplate != NULL)
    {
        m_pTrack = object_cast<Track>(pTemplate->FindName(String(L"PART_Track"),  this));
    }
}


void ScrollBar::ChangeValue(float newValue)
{
      newValue = Float::Min(Float::Max(newValue, get_Minimum()), get_Maximum());
      if (m_isStandalone)
      {
            set_Value(newValue);
      }
      else
      {
          ScrollViewer* pScrollViewer = object_cast<ScrollViewer>(get_TemplatedParent());
          if(pScrollViewer != NULL)
          {

              // This is BAD! Fix this!
              if(get_Orientation() == Orientation::Horizontal)
              {
                  pScrollViewer->m_pScrollInfo->set_HorizontalOffset(newValue);
              }
              else
              {
                  pScrollViewer->m_pScrollInfo->set_VerticalOffset(newValue);
              }
          }
          /*
            IInputElement element1 = base.TemplatedParent as IInputElement;
            if (element1 == null)
            {
                  element1 = this;
            }
            RoutedCommand command1 = (this.Orientation == Orientation.Horizontal) ? ScrollBar.ScrollToHorizontalOffsetCommand : ScrollBar.ScrollToVerticalOffsetCommand;
            if (command1.CanExecute(newValue, element1))
            {
                  command1.Execute(newValue, element1);
            }
            */
      }
}

 



}; // namespace AVUI
