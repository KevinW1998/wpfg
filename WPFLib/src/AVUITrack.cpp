#include <AVUICommon.h>
#include <AVUITrack.h>
#include <AVUIThumb.h>
#include <AVUIRepeatButton.h>
#include <AVUITemplateBinding.h>

REGISTER_ELEMENT_BEGIN(AVUI::Track)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(IncreaseRepeatButton)
    REGISTER_PROPERTY(DecreaseRepeatButton)
    REGISTER_PROPERTY(Thumb)
    REGISTER_PROPERTY(Minimum)
    REGISTER_PROPERTY(Maximum)
    REGISTER_PROPERTY(Value)
    REGISTER_PROPERTY(ViewportSize)
    REGISTER_PROPERTY(IsDirectionReversed)
    REGISTER_PROPERTY(Orientation)
REGISTER_ELEMENT_END()

namespace AVUI {

Track::Track() : m_cVisualChildren(0)
{

}

Track::~Track()
{
    m_cVisualChildren = 0;

    if(m_pDecreaseButton != NULL)
    {
        RemoveVisualChild(m_pDecreaseButton);
        m_pDecreaseButton = NULL;
    }
    if(m_pIncreaseButton != NULL)
    {
        RemoveVisualChild(m_pIncreaseButton);
        m_pIncreaseButton = NULL;
    }
    if(m_pThumb != NULL)
    {
        RemoveVisualChild(m_pThumb);
        m_pThumb = NULL;
    }

}

Size Track::MeasureOverride(const Size& availableSize)
{
    Size desiredSize;

    if(m_pThumb != NULL)
    {
        m_pThumb->Measure(availableSize);
        desiredSize = m_pThumb->get_DesiredSize();
    }

    if(!Float::IsNaN(get_ViewportSize()))
    {
        if(get_Orientation() == Orientation::Vertical)
        {
            desiredSize.set_Height(0);
            return desiredSize;
        }
        desiredSize.set_Width(0);
    }
    return desiredSize;
}       

Size Track::ArrangeOverride(const Size& arrangeSize)
{
    float decreaseButtonLength;
    float thumbLength;
    float increaseButtonLength;
    bool isOrientationVertical = get_Orientation() == Orientation::Vertical;
        
    if (Float::IsNaN(get_ViewportSize()))
    {
        ComputeSliderLengths(arrangeSize, isOrientationVertical, &decreaseButtonLength, &thumbLength, &increaseButtonLength);
    }
    else
    {
        float viewportSize = Float::Max(0, get_ViewportSize());
        if (!ComputeScrollBarLengths(arrangeSize, viewportSize, isOrientationVertical, &decreaseButtonLength, &thumbLength, &increaseButtonLength))
        {
            return arrangeSize;
        }
    }
        
    Point currentPoint;
    Size currentSize = arrangeSize;

    bool isDirectionReversed = get_IsDirectionReversed();
    if (isOrientationVertical)
    {   
        Track::CoerceLength(&decreaseButtonLength, arrangeSize.get_Height());
        Track::CoerceLength(&increaseButtonLength, arrangeSize.get_Height());
        Track::CoerceLength(&thumbLength, arrangeSize.get_Height());

        if(m_pIncreaseButton != NULL)
        {
            currentPoint.set_Y(isDirectionReversed ? (decreaseButtonLength + thumbLength) : 0.0f);
            currentSize.set_Height(increaseButtonLength);
            m_pIncreaseButton->Arrange(Rect(currentPoint, currentSize));
        }


        if(m_pDecreaseButton != NULL)
        {
            currentPoint.set_Y(isDirectionReversed ? 0.0f : (increaseButtonLength + thumbLength));
            currentSize.set_Height(decreaseButtonLength);
            m_pDecreaseButton->Arrange(Rect(currentPoint, currentSize));
        }


        if(m_pThumb != NULL)
        {
            currentPoint.set_Y(isDirectionReversed ? decreaseButtonLength : increaseButtonLength);
            currentSize.set_Height(thumbLength);
            m_pThumb->Arrange(Rect(currentPoint, currentSize));
        }

        m_thumbCenterOffset = currentPoint.get_Y() + (thumbLength * 0.5f);
    }
    else
    {
        Track::CoerceLength(&decreaseButtonLength, arrangeSize.get_Width());
        Track::CoerceLength(&increaseButtonLength, arrangeSize.get_Width());
        Track::CoerceLength(&thumbLength, arrangeSize.get_Width());

        if(m_pDecreaseButton != NULL)
        {
            currentPoint.set_X(isDirectionReversed ? (increaseButtonLength + thumbLength) : 0);
            currentSize.set_Width(decreaseButtonLength);
            m_pDecreaseButton->Arrange(Rect(currentPoint, currentSize));
        }

        if(m_pIncreaseButton != NULL)
        {
            currentPoint.set_X(isDirectionReversed ? 0 : (decreaseButtonLength + thumbLength));
            currentSize.set_Width(increaseButtonLength);
            m_pIncreaseButton->Arrange(Rect(currentPoint, currentSize));
        }

        if(m_pThumb != NULL)
        {
            currentPoint.set_X(isDirectionReversed ? increaseButtonLength : decreaseButtonLength);
            currentSize.set_Width(thumbLength);
            m_pThumb->Arrange(Rect(currentPoint, currentSize));
        }

        m_thumbCenterOffset = currentPoint.get_X() + (thumbLength * 0.5f);
    }
    return arrangeSize;
}


void Track::ComputeSliderLengths(const Size& arrangeSize, bool isOrientationVertical, float* pDecreaseButtonLength, float* pThumbLength, float* pIncreaseButtonLength)
{
    float trackLength;
    float minimum = get_Minimum();
    float range = Float::Max((float) 0.0f, get_Maximum() - minimum);
    float num3 = Float::Min(range, get_Value() - minimum);
    if (isOrientationVertical)
    {
        trackLength = arrangeSize.get_Height();
        if(m_pThumb != NULL)
        {
            *pThumbLength = m_pThumb->get_DesiredSize().get_Height();
        }
    }
    else
    {
        trackLength = arrangeSize.get_Width();
        if(m_pThumb != NULL)
        {
            *pThumbLength = m_pThumb->get_DesiredSize().get_Width();
        }
    }
    Track::CoerceLength(pThumbLength, trackLength);

    float num5 = trackLength - *pThumbLength;
    *pDecreaseButtonLength = (num5 * num3) / range;
    Track::CoerceLength(pDecreaseButtonLength, num5);
    *pIncreaseButtonLength = num5 - *pDecreaseButtonLength;
    Track::CoerceLength(pIncreaseButtonLength, num5);
    m_density = range / num5;
}

bool Track::ComputeScrollBarLengths(const Size& arrangeSize, float viewportSize, bool isOrientationVertical, float* pDecreaseButtonLength, float* pThumbLength, float* pIncreaseButtonLength)
{
    float num5;
    float num6 = 8.0f;
    float minimum = get_Minimum();
    float num2 = Float::Max(0.0f, (float) (get_Maximum() - minimum));
    float num3 = Float::Min(num2, get_Value() - minimum);
    float num4 = Float::Max(0, num2) + viewportSize;
    if (isOrientationVertical)
    {
        num5 = arrangeSize.get_Height();
    }
    else
    {
        num5 = arrangeSize.get_Width();
    }

    *pThumbLength = (num5 * viewportSize) / num4;
    Track::CoerceLength(pThumbLength, num5);
    *pThumbLength = Float::Max(num6, *pThumbLength);

    bool flag1 = Float::LessThanOrClose(num2, 0);
    bool flag2 = *pThumbLength > num5;
    if (flag1 || flag2)
    {
        if (get_Visibility() != Visibility::Hidden)
        {
            set_Visibility(Visibility::Hidden);
        }
        m_thumbCenterOffset = Float::get_NaN();
        m_density = Float::get_NaN();
        *pDecreaseButtonLength = 0;
        *pIncreaseButtonLength = 0;
        return false;
    }
    if (get_Visibility() != Visibility::Visible)
    {
        set_Visibility(Visibility::Visible);
    }

    float num7 = num5 - *pThumbLength;
    *pDecreaseButtonLength = (num7 * num3) / num2;
    Track::CoerceLength(pDecreaseButtonLength, num7);

    *pIncreaseButtonLength = num7 - *pDecreaseButtonLength;
    Track::CoerceLength(pIncreaseButtonLength, num7);
    m_density = num2 / num7;

    return true;
}

void Track::CoerceLength(float* pComponentLength, float trackLength)
{
    if(*pComponentLength < 0)
    {
        *pComponentLength = 0;
    }
    else if(*pComponentLength > trackLength || Float::IsNaN(*pComponentLength))
    {
        *pComponentLength = trackLength;
    }
}

Visual* Track::get_VisualChild(unsigned int idx)
{
    return m_rgVisualChildren[idx];
}

 

float Track::ValueFromDistance(float horizontal, float vertical)
{
      float multiplier = get_IsDirectionReversed() ? -1.0f : 1.0f;
      if (get_Orientation() == Orientation::Horizontal)
      {
            return ((multiplier * horizontal) * m_density);
      }
      return (((-1.0f * multiplier) * vertical) * m_density);
}


void Track::set_DecreaseRepeatButton(RepeatButton* pDecreaseRepeatButton)
{
    if(m_pDecreaseButton != NULL)
    {
        RemoveVisualChild(m_pDecreaseButton);
    }
    m_pDecreaseButton = pDecreaseRepeatButton;

    if(m_pDecreaseButton != NULL)
    {
        AddVisualChild(m_pDecreaseButton);
    }    

    RebuildVisualChildren();
}

void Track::set_IncreaseRepeatButton(RepeatButton* pIncreaseRepeatButton)
{
    if(m_pIncreaseButton != NULL)
    {
        RemoveVisualChild(m_pIncreaseButton);
    }
    m_pIncreaseButton = pIncreaseRepeatButton;

    if(m_pIncreaseButton != NULL)
    {
        AddVisualChild(m_pIncreaseButton);
    }    

    RebuildVisualChildren();
}

void Track::set_Thumb(Thumb* pThumb)
{
    if(m_pThumb != NULL)
    {
        RemoveVisualChild(m_pThumb);
    }
    m_pThumb = pThumb;

    if(m_pThumb != NULL)
    {
        AddVisualChild(m_pThumb);
    }    

    RebuildVisualChildren();
}

void Track::RebuildVisualChildren()
{
    m_cVisualChildren = 0;

    if(m_pDecreaseButton != NULL)
    {
        m_rgVisualChildren[m_cVisualChildren++] = m_pDecreaseButton;
    }

    if(m_pIncreaseButton != NULL)
    {
        m_rgVisualChildren[m_cVisualChildren++] = m_pIncreaseButton;
    }

    if(m_pThumb != NULL)
    {
        m_rgVisualChildren[m_cVisualChildren++] = m_pThumb;
    }
    InvalidateMeasure();
    InvalidateArrange();
}
 



}; // namespace AVUI
