#pragma once

#include <AVUIPoint.h>

namespace AVUI {

class Matrix
{
public:
    Matrix() { *this = get_Identity(); }
    Matrix(float m11, float m12, float m21, float m22, float offsetX, float offsetY) : m_m11(m11), m_m12(m12), m_m21(m21), m_m22(m22), 
                                                                                       m_offsetX(offsetX), m_offsetY(offsetY)
    {

    }
    ~Matrix() { };

    Matrix operator*(const Matrix& rhs) const;
    bool operator==(const Matrix& rhs) const;

    static Matrix MultiplyMatrix(const Matrix& lhs, const Matrix& rhs);
    static Matrix CreateTranslation(float translateX, float translateY);
    static Matrix CreateScaling(float scaleX, float scaleY, float centerX, float centerY);
    static Matrix CreateScaling(float scaleX, float scaleY);
    static Matrix CreateSkewRadians(float skewX, float skewY);

    float GetM11() const { return m_m11; }
    float GetM12() const { return m_m12; }
    float GetM21() const { return m_m21; }
    float GetM22() const { return m_m22; }
    float get_OffsetX() const { return m_offsetX; }
    float get_OffsetY() const { return m_offsetY; }

    void SetM11(float m11) { m_m11 = m11; }
    void SetM12(float m12) { m_m12 = m12; }
    void SetM21(float m21) { m_m21 = m21; }
    void SetM22(float m22) { m_m22 = m22; }

    float get_Determinant() const;
    bool HasInverse() const;
    void Invert();

    void set_OffsetX(float offsetX) { m_offsetX = offsetX; }
    void set_OffsetY(float offsetY) { m_offsetY = offsetY; }
    void Translate(float offsetX, float offsetY);
    void Skew(float skewX, float skewY);

    void ScaleAt(float scaleX, float scaleY, float centerX, float centerY);
    void Scale(float scaleX, float scaleY);


    static Matrix get_Identity();
    bool get_IsIdentity() const;

    Point Transform(const Point& point) const;
    Vector Transform(const Vector& vector) const;
    Rect TransformBounds(const Rect& rect) const;

private:
    

    float m_m11, m_m12;
    float m_m21, m_m22;
    float m_offsetX, m_offsetY;

};


}; // namespace AVUI
