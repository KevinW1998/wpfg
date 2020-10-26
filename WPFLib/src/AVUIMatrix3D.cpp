#include <AVUICommon.h>
#include <AVUIMatrix3D.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::Matrix3D)
REGISTER_VALUE_TYPE_END()

namespace AVUI
{

Matrix3D Matrix3D::operator*(const Matrix3D& matrix1) const
{
    return Matrix3D((((m_m11 * matrix1.m_m11) + (m_m12 * matrix1.m_m21)) + (m_m13 * matrix1.m_m31)) + (m_m14 * matrix1.m_offsetX), 
                    (((m_m11 * matrix1.m_m12) + (m_m12 * matrix1.m_m22)) + (m_m13 * matrix1.m_m32)) + (m_m14 * matrix1.m_offsetY), 
                    (((m_m11 * matrix1.m_m13) + (m_m12 * matrix1.m_m23)) + (m_m13 * matrix1.m_m33)) + (m_m14 * matrix1.m_offsetZ), 
                    (((m_m11 * matrix1.m_m14) + (m_m12 * matrix1.m_m24)) + (m_m13 * matrix1.m_m34)) + (m_m14 * matrix1.m_m44), 
                    (((m_m21 * matrix1.m_m11) + (m_m22 * matrix1.m_m21)) + (m_m23 * matrix1.m_m31)) + (m_m24 * matrix1.m_offsetX), 
                    (((m_m21 * matrix1.m_m12) + (m_m22 * matrix1.m_m22)) + (m_m23 * matrix1.m_m32)) + (m_m24 * matrix1.m_offsetY), 
                    (((m_m21 * matrix1.m_m13) + (m_m22 * matrix1.m_m23)) + (m_m23 * matrix1.m_m33)) + (m_m24 * matrix1.m_offsetZ), 
                    (((m_m21 * matrix1.m_m14) + (m_m22 * matrix1.m_m24)) + (m_m23 * matrix1.m_m34)) + (m_m24 * matrix1.m_m44), 
                    (((m_m31 * matrix1.m_m11) + (m_m32 * matrix1.m_m21)) + (m_m33 * matrix1.m_m31)) + (m_m34 * matrix1.m_offsetX), 
                    (((m_m31 * matrix1.m_m12) + (m_m32 * matrix1.m_m22)) + (m_m33 * matrix1.m_m32)) + (m_m34 * matrix1.m_offsetY),
                    (((m_m31 * matrix1.m_m13) + (m_m32 * matrix1.m_m23)) + (m_m33 * matrix1.m_m33)) + (m_m34 * matrix1.m_offsetZ), 
                    (((m_m31 * matrix1.m_m14) + (m_m32 * matrix1.m_m24)) + (m_m33 * matrix1.m_m34)) + (m_m34 * matrix1.m_m44), 
                    (((m_offsetX * matrix1.m_m11) + (m_offsetY * matrix1.m_m21)) + (m_offsetZ * matrix1.m_m31)) + (m_m44 * matrix1.m_offsetX), 
                    (((m_offsetX * matrix1.m_m12) + (m_offsetY * matrix1.m_m22)) + (m_offsetZ * matrix1.m_m32)) + (m_m44 * matrix1.m_offsetY), 
                    (((m_offsetX * matrix1.m_m13) + (m_offsetY * matrix1.m_m23)) + (m_offsetZ * matrix1.m_m33)) + (m_m44 * matrix1.m_offsetZ), 
                    (((m_offsetX * matrix1.m_m14) + (m_offsetY * matrix1.m_m24)) + (m_offsetZ * matrix1.m_m34)) + (m_m44 * matrix1.m_m44));
}

bool Matrix3D::operator==(const Matrix3D& rhs) const
{
    return m_m11 == rhs.m_m11 && 
           m_m12 == rhs.m_m12 && 
           m_m13 == rhs.m_m13 && 
           m_m14 == rhs.m_m14 && 
           m_m21 == rhs.m_m21 && 
           m_m22 == rhs.m_m22 && 
           m_m23 == rhs.m_m23 && 
           m_m24 == rhs.m_m24 && 
           m_m31 == rhs.m_m31 && 
           m_m32 == rhs.m_m32 && 
           m_m33 == rhs.m_m33 && 
           m_m34 == rhs.m_m34 && 
           m_offsetX == rhs.m_offsetX && 
           m_offsetY == rhs.m_offsetY &&
           m_offsetZ == rhs.m_offsetZ &&
           m_m44 == rhs.m_m44;
}

void Matrix3D::Invert()
{
    if(!InvertCore())
    {
        LibraryCriticalError();
    }
}

bool Matrix3D::InvertCore()
{
//    if (this.IsAffine)
//    {
//        return this.NormalizedAffineInvert();
//    }
    float num7 = (m_m13 * m_m24) - (m_m23 * m_m14);
    float num6 = (m_m13 * m_m34) - (m_m33 * m_m14);
    float num5 = (m_m13 * m_m44) - (m_offsetZ * m_m14);
    float num4 = (m_m23 * m_m34) - (m_m33 * m_m24);
    float num3 = (m_m23 * m_m44) - (m_offsetZ * m_m24);
    float num2 = (m_m33 * m_m44) - (m_offsetZ * m_m34);
    float num12 = ((m_m22 * num6) - (m_m32 * num7)) - (m_m12 * num4);
    float num11 = ((m_m12 * num3) - (m_m22 * num5)) + (m_offsetY * num7);
    float num10 = ((m_m32 * num5) - (m_offsetY * num6)) - (m_m12 * num2);
    float num9 = ((m_m22 * num2) - (m_m32 * num3)) + (m_offsetY * num4);
    float num8 = (((m_offsetX * num12) + (m_m31 * num11)) + (m_m21 * num10)) + (m_m11 * num9);
    if (Float::IsZero(num8))
    {
        return false;
    }
    float num24 = ((m_m11 * num4) - (m_m21 * num6)) + (m_m31 * num7);
    float num23 = ((m_m21 * num5) - (m_offsetX * num7)) - (m_m11 * num3);
    float num22 = ((m_m11 * num2) - (m_m31 * num5)) + (m_offsetX * num6);
    float num21 = ((m_m31 * num3) - (m_offsetX * num4)) - (m_m21 * num2);
    num7 = (m_m11 * m_m22) - (m_m21 * m_m12);
    num6 = (m_m11 * m_m32) - (m_m31 * m_m12);
    num5 = (m_m11 * m_offsetY) - (m_offsetX * m_m12);
    num4 = (m_m21 * m_m32) - (m_m31 * m_m22);
    num3 = (m_m21 * m_offsetY) - (m_offsetX * m_m22);
    num2 = (m_m31 * m_offsetY) - (m_offsetX * m_m32);
    float num20 = ((m_m13 * num4) - (m_m23 * num6)) + (m_m33 * num7);
    float num19 = ((m_m23 * num5) - (m_offsetZ * num7)) - (m_m13 * num3);
    float num18 = ((m_m13 * num2) - (m_m33 * num5)) + (m_offsetZ * num6);
    float num17 = ((m_m33 * num3) - (m_offsetZ * num4)) - (m_m23 * num2);
    float num16 = ((m_m24 * num6) - (m_m34 * num7)) - (m_m14 * num4);
    float num15 = ((m_m14 * num3) - (m_m24 * num5)) + (m_m44 * num7);
    float num14 = ((m_m34 * num5) - (m_m44 * num6)) - (m_m14 * num2);
    float num13 = ((m_m24 * num2) - (m_m34 * num3)) + (m_m44 * num4);
    float num = 1.0f / num8;
    m_m11 = num9 * num;
    m_m12 = num10 * num;
    m_m13 = num11 * num;
    m_m14 = num12 * num;
    m_m21 = num21 * num;
    m_m22 = num22 * num;
    m_m23 = num23 * num;
    m_m24 = num24 * num;
    m_m31 = num13 * num;
    m_m32 = num14 * num;
    m_m33 = num15 * num;
    m_m34 = num16 * num;
    m_offsetX = num17 * num;
    m_offsetY = num18 * num;
    m_offsetZ = num19 * num;
    m_m44 = num20 * num;

    return true;
}


Matrix3D Matrix3D::CreateRotationMatrix(const Quaternion& quaternion, const Point3D& center)
{
    Matrix3D matrixd = CreateIdentity();

    float num12 = quaternion.get_X() + quaternion.get_X();
    float num2 = quaternion.get_Y() + quaternion.get_Y();
    float num = quaternion.get_Z() + quaternion.get_Z();
    float num11 = quaternion.get_X() * num12;
    float num10 = quaternion.get_X() * num2;
    float num9 = quaternion.get_X() * num;
    float num8 = quaternion.get_Y() * num2;
    float num7 = quaternion.get_Y() * num;
    float num6 = quaternion.get_Z() * num;
    float num5 = quaternion.get_W() * num12;
    float num4 = quaternion.get_W() * num2;
    float num3 = quaternion.get_W() * num;
    matrixd.m_m11 = 1 - (num8 + num6);
    matrixd.m_m12 = num10 + num3;
    matrixd.m_m13 = num9 - num4;
    matrixd.m_m21 = num10 - num3;
    matrixd.m_m22 = 1 - (num11 + num6);
    matrixd.m_m23 = num7 + num5;
    matrixd.m_m31 = num9 + num4;
    matrixd.m_m32 = num7 - num5;
    matrixd.m_m33 = 1 - (num11 + num8);
    if (((center.get_X() != 0) || (center.get_Y() != 0)) || (center.get_Z() != 0))
    {
        matrixd.m_offsetX = (((-center.get_X() * matrixd.m_m11) - (center.get_Y() * matrixd.m_m21)) - (center.get_Z() * matrixd.m_m31)) + center.get_X();
        matrixd.m_offsetY = (((-center.get_X() * matrixd.m_m12) - (center.get_Y() * matrixd.m_m22)) - (center.get_Z() * matrixd.m_m32)) + center.get_Y();
        matrixd.m_offsetZ = (((-center.get_X() * matrixd.m_m13) - (center.get_Y() * matrixd.m_m23)) - (center.get_Z() * matrixd.m_m33)) + center.get_Z();
    }

    return matrixd;
}

void Matrix3D::ScaleAt(const Vector3D &scale, const Point3D& center)
{
    float num = m_m14 * center.get_X();
    m_m11 = num + (scale.get_X() * (m_m11 - num));
    num = m_m14 * center.get_Y();
    m_m12 = num + (scale.get_Y() * (m_m12 - num));
    num = m_m14 * center.get_Z();
    m_m13 = num + (scale.get_Z() * (m_m13 - num));
    num = m_m24 * center.get_X();
    m_m21 = num + (scale.get_X() * (m_m21 - num));
    num = m_m24 * center.get_Y();
    m_m22 = num + (scale.get_Y() * (m_m22 - num));
    num = m_m24 * center.get_Z();
    m_m23 = num + (scale.get_Z() * (m_m23 - num));
    num = m_m34 * center.get_X();
    m_m31 = num + (scale.get_X() * (m_m31 - num));
    num = m_m34 * center.get_Y();
    m_m32 = num + (scale.get_Y() * (m_m32 - num));
    num = m_m34 * center.get_Z();
    m_m33 = num + (scale.get_Z() * (m_m33 - num));
    num = m_m44 * center.get_X();
    m_offsetX = num + (scale.get_X() * (m_offsetX - num));
    num = m_m44 * center.get_Y();
    m_offsetY = num + (scale.get_Y() * (m_offsetY - num));
    num = m_m44 * center.get_Z();
    m_offsetZ = num + (scale.get_Z() * (m_offsetZ - num));
    
}

void Matrix3D::SetTranslationMatrix(const Vector3D& offset)
{
    m_m11 = m_m22 = m_m33 = m_m44 = 1.0;
    m_offsetX = offset.get_X();
    m_offsetY = offset.get_Y();
    m_offsetZ = offset.get_Z();
}

 

 



}

