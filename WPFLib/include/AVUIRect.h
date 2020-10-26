#pragma once

#include <AVUIPoint.h>
#include <AVUISize.h>

namespace AVUI {

class Matrix;

class Rect
{
public:
    Rect() : m_x(0), m_y(0), m_width(0), m_height(0) { };
    Rect(const Size& size) : m_x(0), m_y(0), m_width(size.get_Width()), m_height(size.get_Height()) { };
    Rect(const Point& point, const Size& size) : m_x(point.get_X()), m_y(point.get_Y()), m_width(size.get_Width()), m_height(size.get_Height()) { };

    Rect(const Point& point1, const Point& point2);
    Rect(float x, float y, float width, float height) : m_x(x), m_y(y), m_width(width), m_height(height) { };

    bool Contains(const Rect& other) const;
    bool Contains(const Point& pt) const { return Contains(pt.get_X(), pt.get_Y()); }
    bool Contains(float x, float y) const { return x >= m_x && x < (m_x + m_width) && y >= m_y && y < (m_y + m_height); }

    void Offset(float offsetX, float offsetY);
    void Offset(Vector offset) { Offset(offset.get_X(), offset.get_Y()); }

    float get_Left() const { return m_x; };
    float get_Top() const { return m_y; };
    float get_Right() const { return m_x + m_width; };
    float get_Bottom() const { return m_y + m_height; };

    Point get_TopLeft() const { return Point(m_x, m_y); }
    Point get_TopRight() const { return Point(m_x + m_width, m_y); }
    Point get_BottomLeft() const { return Point(m_x, m_y + m_height); }
    Point get_BottomRight() const { return Point(m_x + m_width, m_y + m_height); }

    Point get_Location() const { return Point(m_x, m_y); }
    void set_Location(const Point& point) { m_x = point.get_X(); m_y = point.get_Y(); }
    Size get_Size() const { return Size(m_width, m_height); };
    void set_Size(const Size& size) { m_width = size.get_Width(); m_height = size.get_Height(); };

    float get_Width() const { return m_width; };
    void  set_Width(float width) { m_width = width; };
    float get_Height() const { return m_height; };
    void set_Height(float height) { m_height = height; };

    float get_X() const { return m_x; };
    void  set_X(float x) { m_x = x; };
    float get_Y() const { return m_y; };
    void  set_Y(float y) { m_y = y; };

    bool get_IsEmpty() { return m_x == 0.0f && m_y == 0.0f && m_width == 0.0f && m_height == 0.0f; }
    static Rect get_Empty() { return Rect(); }

    bool Intersects(const Rect& other) const;
    bool IsEqual(const Rect& other) { return other.m_x == m_x && other.m_y == m_y && other.m_height == m_height && other.m_width == m_width; }

    Rect Inflate(float inflate) const;

    static Rect Transform(const Rect&, const Matrix&);
    static Rect Union(const Rect&, const Rect&);
    static bool Intersect(const Rect&, const Rect&, Rect* pRectIntersection);
    static Rect Offset(Rect rect, const Vector& offsetVector);

    bool operator==(const Rect& rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y && m_height == rhs.m_height && m_width == rhs.m_width; }
    bool operator!=(const Rect& rhs) const { return !(*this == rhs); }

    DECLARE_VALUE_ELEMENT(Rect);
    IMPLEMENT_VALUE_PROPERTY(X, float, false);
    IMPLEMENT_VALUE_PROPERTY(Y, float, false);
    IMPLEMENT_VALUE_PROPERTY(Width, float, false);
    IMPLEMENT_VALUE_PROPERTY(Height, float, false);

private:
    float m_x, m_y, m_height, m_width;

};


}; // namespace AVUI
