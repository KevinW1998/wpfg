#pragma once

namespace AVUI {

class Matrix3D;

class Point3D
{
public:
    Point3D() : m_x(0.0f), m_y(0.0f), m_z(0.0f) { };
    Point3D(float x, float y, float z) { m_x = x; m_y = y; m_z = z; };

    float get_X() const { return m_x; }
    void  set_X(float x) { m_x = x; }
    float get_Y() const { return m_y; }
    void set_Y(float y) { m_y = y; }

    float get_Z() const { return m_z; }
    void set_Z(float z) { m_z = z; }

    Point3D operator*(const Matrix3D& rhs) const;

    Point3D operator+(const Point3D& rhs) const;
    Point3D operator-(const Point3D& rhs) const;
    Point3D operator*(float coeff) const;
    Point3D operator/(float coeff) const;

    bool operator==(const Point3D& rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z; }
    bool operator!=(const Point3D& rhs) const { return !(*this == rhs); }

    DECLARE_VALUE_ELEMENT(Point3D);
    IMPLEMENT_VALUE_PROPERTY(X, float, false);
    IMPLEMENT_VALUE_PROPERTY(Y, float, false);
    IMPLEMENT_VALUE_PROPERTY(Z, float, false);

    static Point3D Parse(const String* pSource);

private:

    float m_x; 
    float m_y;
    float m_z;
};

};
