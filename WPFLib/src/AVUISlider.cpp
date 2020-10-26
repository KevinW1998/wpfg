#include <AVUICommon.h>
#include <AVUISlider.h>
#include <AVUIThumb.h>
#include <AVUITrack.h>
#include <AVUIControlTemplate.h>
#include <AVUICommandManager.h>
#include <AVUICommandBinding.h>

REGISTER_ELEMENT_BEGIN(AVUI::Slider)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Orientation)
    REGISTER_PROPERTY(IsDirectionReversed)
    REGISTER_PROPERTY(IsSnapToTickEnabled)
    REGISTER_PROPERTY(TickFrequency)

    REGISTER_ROUTED_COMMAND(IncreaseLarge);
    REGISTER_ROUTED_COMMAND(DecreaseLarge);
    REGISTER_ROUTED_COMMAND(IncreaseSmall);
    REGISTER_ROUTED_COMMAND(DecreaseSmall);
    REGISTER_ROUTED_COMMAND(MinimizeValue);
    REGISTER_ROUTED_COMMAND(MaximizeValue);

    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
REGISTER_ELEMENT_END()

namespace AVUI
{

void Slider::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(Slider), Thumb::get_DragStartedEvent(), RoutedEventHandler::Create(OnDragStartedThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(Slider), Thumb::get_DragCompletedEvent(), RoutedEventHandler::Create(OnDragCompletedThunk), false);
    EventManager::RegisterClassHandler(GET_TYPE(Slider), Thumb::get_DragDeltaEvent(), DragDeltaEventHandler::Create(OnDragDeltaThunk), false);


    CommandManager::RegisterClassHandler(GET_TYPE(Slider), object_allocate<CommandBinding>(Slider::get_IncreaseLarge(), ExecutedRoutedEventHandler::Create(&Slider::OnIncreaseLargeCommand)));
    CommandManager::RegisterClassHandler(GET_TYPE(Slider), object_allocate<CommandBinding>(Slider::get_DecreaseLarge(), ExecutedRoutedEventHandler::Create(&Slider::OnDecreaseLargeCommand)));
    CommandManager::RegisterClassHandler(GET_TYPE(Slider), object_allocate<CommandBinding>(Slider::get_IncreaseSmall(), ExecutedRoutedEventHandler::Create(&Slider::OnIncreaseSmallCommand)));
    CommandManager::RegisterClassHandler(GET_TYPE(Slider), object_allocate<CommandBinding>(Slider::get_DecreaseSmall(), ExecutedRoutedEventHandler::Create(&Slider::OnDecreaseSmallCommand)));
    CommandManager::RegisterClassHandler(GET_TYPE(Slider), object_allocate<CommandBinding>(Slider::get_MinimizeValue(), ExecutedRoutedEventHandler::Create(&Slider::OnMinimizeValueCommand)));
    CommandManager::RegisterClassHandler(GET_TYPE(Slider), object_allocate<CommandBinding>(Slider::get_MaximizeValue(), ExecutedRoutedEventHandler::Create(&Slider::OnMaximizeValueCommand)));

}


void Slider::OnApplyTemplate()
{
    Control::OnApplyTemplate();

    ControlTemplate* pTemplate = get_Template();
    if(pTemplate != NULL)
    {
        m_pTrack = object_cast<Track>(pTemplate->FindName(String(L"PART_Track"),  this));
    }
}


void Slider::OnDragStarted(RoutedEventArgs* pArgs)
{

}

void Slider::OnDragDelta(DragDeltaEventArgs* pArgs)
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
                  UpdateValue(newValue);
            }            
        }
    }
}

void Slider::OnDragCompleted(RoutedEventArgs* pArgs)
{

}

void Slider::UpdateValue(float newValue)
{
    newValue = SnapToTick(newValue);

    newValue = Float::Min(Float::Max(newValue, get_Minimum()), get_Maximum());

    set_Value(newValue);
}


float Slider::SnapToTick(float value)
{
    if(get_IsSnapToTickEnabled())
    {
        float tickValue = get_Minimum() + (Float::Round(((value - get_Minimum()) / get_TickFrequency())) * get_TickFrequency());
        float nextTickValue = Float::Min(get_Maximum(), tickValue + get_TickFrequency());
        value = Float::GreaterThanOrClose(value, (tickValue + nextTickValue) * 0.5f) ? nextTickValue : tickValue;
    }

    return value;
}

void Slider::OnIncreaseLargeCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    Slider* pSlider = object_cast<Slider>(pSender);
    if (pSlider != NULL)
    {
        pSlider->OnIncreaseLarge();
    }
}

void Slider::OnDecreaseLargeCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    Slider* pSlider = object_cast<Slider>(pSender);
    if (pSlider != NULL)
    {
        pSlider->OnDecreaseLarge();
    }
}

void Slider::OnIncreaseSmallCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    Slider* pSlider = object_cast<Slider>(pSender);
    if (pSlider != NULL)
    {
        pSlider->OnIncreaseSmall();
    }
}

void Slider::OnDecreaseSmallCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    Slider* pSlider = object_cast<Slider>(pSender);
    if (pSlider != NULL)
    {
        pSlider->OnDecreaseSmall();
    }
}

void Slider::OnMinimizeValueCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    Slider* pSlider = object_cast<Slider>(pSender);
    if (pSlider != NULL)
    {
        pSlider->OnMinimizeValue();
    }
}

void Slider::OnMaximizeValueCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs)
{
    Slider* pSlider = object_cast<Slider>(pSender);
    if (pSlider != NULL)
    {
        pSlider->OnMaximizeValue();
    }
}

void Slider::OnIncreaseLarge()
{
    MoveToNextTick(get_LargeChange());
}

void Slider::OnDecreaseLarge()
{
    MoveToNextTick(-get_LargeChange());
}
void Slider::OnIncreaseSmall()
{
    MoveToNextTick(get_SmallChange());
}
void Slider::OnDecreaseSmall()
{
    MoveToNextTick(-get_SmallChange());
}
void Slider::OnMinimizeValue()
{
    set_Value(get_Minimum());
}
void Slider::OnMaximizeValue()
{
    set_Value(get_Maximum());
}

void Slider::MoveToNextTick(float direction)
{
    if (direction != 0.0f)
    {
        float num = get_Value();
        float num2 = SnapToTick(Float::Max(get_Minimum(), Float::Min(get_Maximum(), num + direction)));
        bool flag = direction > 0.0f;
        if (((num2 == num) && (!flag || (num != get_Maximum()))) && (flag || (num != get_Minimum())))
        {
            /*
            bool flag2;
            DoubleCollection ticks = null;
            if ((base.GetValueSource(TicksProperty, null, out flag2) != BaseValueSourceInternal.Default) || flag2)
            {
                ticks = this.Ticks;
            }
            if ((ticks != null) && (ticks.Count > 0))
            {
                for (int i = 0; i < ticks.Count; i++)
                {
                    double num4 = ticks[i];
                    if (((flag && DoubleUtil.GreaterThan(num4, num)) && (DoubleUtil.LessThan(num4, num2) || (num2 == num))) || ((!flag && DoubleUtil.LessThan(num4, num)) && (DoubleUtil.GreaterThan(num4, num2) || (num2 == num))))
                    {
                        num2 = num4;
                    }
                }
            }
            else 
            */

            if (Float::GreaterThan(get_TickFrequency(), 0.0f))
            {
                float num5 = Float::Round((float) ((num - get_Minimum()) / get_TickFrequency()));
                if (flag)
                {
                    num5++;
                }
                else
                {
                    num5--;
                }
                num2 = get_Minimum() + (num5 * get_TickFrequency());
            }
        }
        if (num2 != num)
        {
            set_Value(num2);
        }
    }
}


} // namespace AVUI
