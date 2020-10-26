#pragma once

#include <math.h>

namespace AVUI {

class Vector
{
public:
    Vector() : m_x(0.0f), m_y(0.0f) { };
    Vector(float x, float y) : m_x(x), m_y(y) { };

    float get_X() const { return m_x; }
    void set_X(float x) { m_x = x; }

    float get_Y() const { return m_y; }
    void set_Y(float y) { m_y = y; }

    void Normalize() { float length = get_Length(); m_x /= length; m_y /= length; }

    bool operator==(const Vector& rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y; }
    bool operator!=(const Vector& rhs) const { return !(*this == rhs); }
    Vector operator*(float coeff) const;

    float get_Length()
    {
        return sqrtf(m_x * m_x + m_y * m_y);
    }

private:

    float m_x; 
    float m_y;

};

}; // namespace AVUI
