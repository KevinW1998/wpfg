#pragma once

namespace AVUI {

class Matrix;

class Vector;

class Point
{
public:
    Point() : m_x(0.0f), m_y(0.0f) { };
    Point(float x, float y) { m_x = x; m_y = y; };

    float get_X() const { return m_x; }
    void  set_X(float x) { m_x = x; }
    float get_Y() const { return m_y; }
    void set_Y(float y) { m_y = y; }

    Point operator*(const Matrix& rhs) const;

    Point operator+(const Point& rhs) const;
    Vector operator-(const Point& rhs) const;
    Point operator*(float coeff) const;
    Point operator/(float coeff) const;

    Point operator+(const Vector& rhs) const;
    Point operator-(const Vector& rhs) const;

    bool operator==(const Point& rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y; }
    bool operator!=(const Point& rhs) const { return !(*this == rhs); }

    DECLARE_VALUE_ELEMENT(Point);
    IMPLEMENT_VALUE_PROPERTY(X, float, false);
    IMPLEMENT_VALUE_PROPERTY(Y, float, false);

private:

    float m_x; 
    float m_y;
};

};
