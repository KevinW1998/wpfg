#include <AVUICommon.h>
#include <AVUIRect.h>
#include <AVUIMatrix.h>
#include <AVUIBuiltinTypeConverters.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::Rect)
    REGISTER_TYPE_CONVERTER(RectTypeConverter);
    REGISTER_PROPERTY(X);
    REGISTER_PROPERTY(Y);
    REGISTER_PROPERTY(Width);
    REGISTER_PROPERTY(Height);
REGISTER_VALUE_TYPE_END()

namespace AVUI {


Rect::Rect(const Point& point1, const Point& point2)
{
    m_x = Float::Min(point1.get_X(), point2.get_X());
    m_y = Float::Min(point1.get_Y(), point2.get_Y());
    m_width = Float::Max((Float::Max(point1.get_X(), point2.get_X()) - m_x), 0.0f);
    m_height = Float::Max((Float::Max(point1.get_Y(), point2.get_Y()) - m_y), 0.0f);
}


Rect Rect::Union(const Rect& a, const Rect& b)
{  
    float minX = Float::Min(a.get_Left(), b.get_Left());
    float minY = Float::Min(a.get_Top(), b.get_Top());
    float maxX = Float::Max(a.get_Right(), b.get_Right());
    float maxY = Float::Max(a.get_Bottom(), b.get_Bottom());

    return Rect(minX, minY, maxX - minX, maxY - minY);
}

bool Rect::Intersect(const Rect& a, const Rect& b, Rect* pRectIntersection)
{
    float minX = Float::Max(a.get_Left(), b.get_Left());
    float minY = Float::Max(a.get_Top(), b.get_Top());
    float maxX = Float::Min(a.get_Right(), b.get_Right());
    float maxY = Float::Min(a.get_Bottom(), b.get_Bottom());

    if(maxX <= minX || maxY <= minY)
    {
        return false;
    }

    *pRectIntersection = Rect(minX, minY, maxX - minX, maxY - minY);

    return true;
}

bool Rect::Intersects(const Rect& other) const
{
    if(get_Right() < other.get_Left())
    {
        return false;
    }

    if(get_Left() > other.get_Right())
    {
        return false;
    }

    if(get_Bottom() < other.get_Top())
    {
        return false;
    }

    if(get_Top() > other.get_Bottom())
    {
        return false;
    }

    return true;
}

bool Rect::Contains(const Rect& other) const
{
    if(get_Right() < other.get_Right())
    {
        return false;
    }

    if(get_Left() > other.get_Left())
    {
        return false;
    }

    if(get_Bottom() < other.get_Bottom())
    {
        return false;
    }

    if(get_Top() > other.get_Top())
    {
        return false;
    }

    return true;
}

Rect Rect::Inflate(float inflate) const
{
    return Rect(m_x - inflate, m_y - inflate, m_width + 2.0f * inflate, m_height + 2.0f * inflate);
}

Rect Rect::Transform(const Rect& rect, const Matrix& matrix)
{
    return matrix.TransformBounds(rect);
}

Rect Rect::Offset(Rect rect, const Vector& offsetVector)
{
    rect.Offset(offsetVector.get_X(), offsetVector.get_Y());
    return rect;
}

void Rect::Offset(float offsetX, float offsetY)
{
    m_x += offsetX;
    m_y += offsetY;
}

 

 



}; // namespace AVUI
