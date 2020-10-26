#include <AVUICommon.h>
#include <AVUIEllipse.h>

REGISTER_ELEMENT_BEGIN(AVUI::Ellipse)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI {

Ellipse::Ellipse()
{
}

Ellipse::~Ellipse()
{
}

Size Ellipse::MeasureOverride(const Size& constraintSize)
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

Size Ellipse::ArrangeOverride(const Size& finalSize)
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

 
void Ellipse::OnRender(DrawingContext& drawingContext)
{
    drawingContext.DrawEllipse(get_Fill(), GetPen(), Point(m_rect.get_Left() + m_rect.get_Width() / 2.0f, m_rect.get_Top() + m_rect.get_Height() / 2.0f), 
                               m_rect.get_Width() / 2.0f, m_rect.get_Height() / 2.0f);
}


}; // namespace AVUI
