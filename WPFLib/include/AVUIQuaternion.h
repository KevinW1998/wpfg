#pragma once

#include <AVUIVector3D.h>

namespace AVUI {

class Quaternion
{
public:
    Quaternion() : m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(0.0f) { };
    Quaternion(float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w) { };
    Quaternion(const Vector3D& axisOfRotation, float angleInDegrees);

    void Conjugate();
    void Invert();
    void Normalize();

    static Quaternion Slerp(const Quaternion& from, const Quaternion& to, float t, bool useShortestPath = true);

    float get_X() const { return m_x; }
    void set_X(float x) { m_x = x; }

    float get_Y() const { return m_y; }
    void set_Y(float y) { m_y = y; }

    float get_Z() const { return m_z; }
    void set_Z(float z) { m_z = z; }

    float get_W() const { return m_w; }
    void set_W(float w) { m_w = w; }

    float get_Angle() const;
    Vector3D get_Axis() const;

    static Quaternion get_Identity() { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }
    bool IsIdentity();
    bool IsNormalized();

    DECLARE_VALUE_ELEMENT(Quaternion);
    IMPLEMENT_VALUE_PROPERTY(X, float, false);
    IMPLEMENT_VALUE_PROPERTY(Y, float, false);
    IMPLEMENT_VALUE_PROPERTY(Z, float, false);
    IMPLEMENT_VALUE_PROPERTY(W, float, false);

    bool operator==(const Quaternion& rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z && m_w == rhs.m_w; }
    Quaternion operator*(float coeff) const { return Quaternion(m_x * coeff, m_y * coeff, m_z * coeff, m_w * coeff); }
    Quaternion operator+(const Quaternion& rhs) const { return Quaternion(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z, m_w + rhs.m_w); }
    Quaternion operator-(const Quaternion& rhs) const { return Quaternion(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z, m_w - rhs.m_w); }
    Quaternion operator*(const Quaternion& rhs) const;

private:

    void Scale(float coeff) { m_x *= coeff; m_y *= coeff; m_z *= coeff; m_w *= coeff; }
    float Length();

    float m_x; 
    float m_y;
    float m_z;
    float m_w;
};

}; // namespace AVUI
