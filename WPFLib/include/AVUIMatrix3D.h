
#pragma once

#include <AVUIPoint3D.h>
#include <AVUIVector3D.h>

namespace AVUI {

class Quaternion;

class Matrix3D
{
public:
    Matrix3D() { }
    Matrix3D(float m11, float m12, float m13, float m14, 
           float m21, float m22, float m23, float m24, 
           float m31, float m32, float m33, float m34, 
           float offsetX, float offsetY, float offsetZ, float m44) :
                m_m11(m11), m_m12(m12), m_m13(m13), m_m14(m14), 
                m_m21(m21), m_m22(m22), m_m23(m23), m_m24(m24), 
                m_m31(m31), m_m32(m32), m_m33(m33), m_m34(m34), 
                m_offsetX(offsetX), m_offsetY(offsetY), m_offsetZ(offsetZ), m_m44(m44)
                                                         
    {

    }
    ~Matrix3D() { };

    void Append(const Matrix3D& matrix)
    {
        (*this) = (*this) * matrix;
    }

    static Matrix3D CreateIdentity()
    {
        return Matrix3D(1.0f, 0.0f, 0.0f, 0.0f, 
                        0.0f, 1.0f, 0.0f, 0.0f,
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);

    }

    static Matrix3D CreateRotationMatrix(const Quaternion& quaternion, const Point3D& center);

    void Invert();

    static Matrix3D Multiply(const Matrix3D& matrix1, const Matrix3D& matrix2)
    {
        return matrix1*matrix2;
    }

    bool operator==(const Matrix3D& rhs) const;
    Matrix3D operator*(const Matrix3D& rhs) const;

    void Prepend(const Matrix3D& matrix)
    {
        (*this) = matrix * (*this);
    }

    void Rotate(const Quaternion& quaternion)
    {
        *this = (*this) * CreateRotationMatrix(quaternion, Point3D());
    }

    void RotateAt(const Quaternion& quaternion, const Point3D& center)
    {
        *this = (*this) * CreateRotationMatrix(quaternion, center);
    }

    void RotatePrepend(const Quaternion& quaternion)
    {
        *this = CreateRotationMatrix(quaternion, Point3D()) * (*this);
    }

    void RotateAtPrepend(const Quaternion& quaternion, const Point3D& center)
    {
        *this = CreateRotationMatrix(quaternion, center) * (*this);
    }

    void Scale(const Vector3D &scale);
    void ScaleAt(const Vector3D &scale, const Point3D& center);

    void ScaleAtPrepend(const Vector3D &scale, const Point3D& center);
    void ScalePrepend(const Vector3D &scale);
    void SetIdentity() { *this = CreateIdentity(); };

    Point3D Transform(const Point3D& point);
    Vector3D Transform(const Vector3D& offset);

    void Transform(Point3D* rgPoints, int cPoints);
    void Transform(Vector3D* rgVectors, int cVectors);

    void Translate(const Vector3D& offset);
    void TranslatePrepend(const Vector3D& offset);


    float get_M11() const { return m_m11; }
    float get_M12() const { return m_m12; }
    float get_M13() const { return m_m13; }
    float get_M14() const { return m_m14; }
    float get_M21() const { return m_m11; }
    float get_M22() const { return m_m12; }
    float get_M23() const { return m_m13; }
    float get_M24() const { return m_m14; }
    float get_M31() const { return m_m11; }
    float get_M32() const { return m_m12; }
    float get_M33() const { return m_m13; }
    float get_M34() const { return m_m14; }
    float get_OffsetX() const { return m_offsetX; }
    float get_OffsetY() const { return m_offsetY; }
    float get_OffsetZ() const { return m_offsetZ; }
    float get_M44() const { return m_m44; }

    void set_M11(float m11) { m_m11 = m11; }
    void set_M12(float m12) { m_m12 = m12; }
    void set_M13(float m13) { m_m13 = m13; }
    void set_M14(float m14) { m_m14 = m14; }
    void set_M21(float m21) { m_m21 = m21; }
    void set_M22(float m22) { m_m22 = m22; }
    void set_M23(float m23) { m_m23 = m23; }
    void set_M24(float m24) { m_m24 = m24; }
    void set_M31(float m31) { m_m31 = m31; }
    void set_M32(float m32) { m_m32 = m32; }
    void set_M33(float m33) { m_m33 = m33; }
    void set_M34(float m34) { m_m34 = m34; }
    void set_OffsetX(float offsetX) { m_offsetX = offsetX; }
    void set_OffsetY(float offsetY) { m_offsetY = offsetY; }
    void set_OffsetZ(float offsetZ) { m_offsetZ = offsetZ; }
    void set_M44(float m44) { m_m44 = m44; }

    float get_Determinant() const;
    bool HasInverse() const;
    bool IsAffine() const;

    void SetScaleMatrix(const Vector3D& scale);
    void SetScaleMatrix(const Vector3D& scale, const Point3D& center);
    void SetTranslationMatrix(const Vector3D& offset);

private:

    bool InvertCore();
    float GetNormalizedAffineDeterminant();
    void MultiplyPoint(Point3D* pPoint);
    void MultiplyVector(Vector3D* pPoint);
    bool NormalizedAffineInvert();

    float m_m11, m_m12, m_m13, m_m14;
    float m_m21, m_m22, m_m23, m_m24;
    float m_m31, m_m32, m_m33, m_m34;
    float m_offsetX, m_offsetY, m_offsetZ, m_m44;
};


}; // namespace AVUI
