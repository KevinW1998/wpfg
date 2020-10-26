#pragma once

#include <math.h>

namespace AVUI {

class Vector3D
{
public:
    Vector3D() : m_x(0.0f), m_y(0.0f), m_z(0.0f) { };
    Vector3D(float x, float y, float z) : m_x(x), m_y(y), m_z(z) { };

    float get_X() const { return m_x; }
    void set_X(float x) { m_x = x; }

    float get_Y() const { return m_y; }
    void set_Y(float y) { m_y = y; }

    float get_Z() const { return m_z; }
    void set_Z(float z) { m_z = z; }

    void Normalize() { float length = get_Length(); m_x /= length; m_y /= length; m_z /= length; }

    bool operator==(const Vector3D& rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z; }
    bool operator!=(const Vector3D& rhs) const { return !(*this == rhs); }

    Vector3D operator-() const { return Vector3D(-m_x, -m_y, -m_z); }
    Vector3D operator*(float coeff) const;
    Vector3D operator/(float coeff) const;
    Vector3D operator+(const Vector3D& rhs) const;
    Vector3D operator-(const Vector3D& rhs) const;



    float get_Length() const { return sqrtf(m_x * m_x + m_y * m_y + m_z * m_z); }

    static Vector3D CrossProduct(const Vector3D& vector1, const Vector3D& vector2)
    {
        return Vector3D((vector1.m_y * vector2.m_z) - (vector1.m_z * vector2.m_y), (vector1.m_z * vector2.m_x) - (vector1.m_x * vector2.m_z), (vector1.m_x * vector2.m_y) - (vector1.m_y * vector2.m_x));
    }

    static float DotProduct(const Vector3D& vector1, const Vector3D& vector2)
    {
        return vector1.m_x * vector2.m_x + vector1.m_y * vector2.m_y + vector1.m_z * vector2.m_z;
    }
 
    static Vector3D Parse(const String* pString);

    DECLARE_VALUE_ELEMENT(Vector3D);
    IMPLEMENT_VALUE_PROPERTY(X, float, false);
    IMPLEMENT_VALUE_PROPERTY(Y, float, false);
    IMPLEMENT_VALUE_PROPERTY(Z, float, false);

private:

    float m_x; 
    float m_y;
    float m_z;
};

}; // namespace AVUI
