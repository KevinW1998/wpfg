#include <AVUICommon.h>
#include <AVUIRectangle.h>

REGISTER_ELEMENT_BEGIN(AVUI::Rectangle)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(RadiusX);
    REGISTER_PROPERTY(RadiusY);
REGISTER_ELEMENT_END()

namespace AVUI {

Rectangle::Rectangle()
{
}

Rectangle::~Rectangle()
{
}

Size Rectangle::MeasureOverride(const Size& constraintSize)
{

    if(get_Stretch() != Stretch::UniformToFill)
    {
        float strokeThickness = GetStrokeThickness();
        return Size(strokeThickness, strokeThickness);
    }
    float width = constraintSize.get_Width();
    float height = constraintSize.get_Height();
    float size;

    if (Float::IsInfinity(width) || Float::IsInfinity(height))
    {
        size = Float::Min(width, height);
    }
    else
    {
        size = Float::Max(width, height);
    }
    return Size(size, size);
}

Size Rectangle::ArrangeOverride(const Size& finalSize)
{
    float strokeThickness = GetStrokeThickness();
    float halfStrokeThickness = strokeThickness / 2.0f;
    
    m_rect = Rect(halfStrokeThickness, halfStrokeThickness, 
                Float::Max(0.0f, finalSize.get_Width() - strokeThickness), 
                Float::Max(0.0f, finalSize.get_Height() - strokeThickness));

    switch (get_Stretch())
    {
    case Stretch::Uniform:
        {
            if (m_rect.get_Width() <= m_rect.get_Height())
            {
                m_rect.set_Height(m_rect.get_Width());
            }
            else
            {
                m_rect.set_Width(m_rect.get_Height());
            }
            break;
        }
    case Stretch::UniformToFill:
        {
              if (m_rect.get_Width() >= m_rect.get_Height())
              {
                    m_rect.set_Height(m_rect.get_Width());
              }
              else
              {
                  m_rect.set_Width(m_rect.get_Height());
              }
              break;
        }
    default:
        break;
    }
    
	    return finalSize;
}

 
void Rectangle::OnRender(DrawingContext& drawingContext)
{
    drawingContext.DrawRoundedRectangle(get_Fill(), GetPen(), m_rect, get_RadiusX(), get_RadiusY());
}


}; // namespace AVUI
