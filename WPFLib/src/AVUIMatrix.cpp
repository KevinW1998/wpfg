#include <AVUICommon.h>
#include <AVUIMatrix.h>

namespace AVUI {

Matrix Matrix::operator*(const Matrix& rhs) const
{
    return MultiplyMatrix(*this, rhs);
}

bool Matrix::operator==(const Matrix& rhs) const
{
    return m_m11 == rhs.m_m11 && 
           m_m12 == rhs.m_m12 && 
           m_m21 == rhs.m_m21 && 
           m_m22 == rhs.m_m22 && 
           m_offsetX == rhs.m_offsetX && 
           m_offsetY == rhs.m_offsetY;
}


Matrix Matrix::MultiplyMatrix(const Matrix& lhs, const Matrix& rhs)
{
    return Matrix( (lhs.m_m11 * rhs.m_m11) + (lhs.m_m12 * rhs.m_m21), 
                   (lhs.m_m11 * rhs.m_m12) + (lhs.m_m12 * rhs.m_m22), 
                   (lhs.m_m21 * rhs.m_m11) + (lhs.m_m22 * rhs.m_m21), 
                   (lhs.m_m21 * rhs.m_m12) + (lhs.m_m22 * rhs.m_m22), 
                   ((lhs.m_offsetX * rhs.m_m11) + (lhs.m_offsetY * rhs.m_m21)) + rhs.m_offsetX, 
                   ((lhs.m_offsetX * rhs.m_m12) + (lhs.m_offsetY * rhs.m_m22)) + rhs.m_offsetY
                 );
}


Matrix Matrix::get_Identity()
{
    return Matrix(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
}

bool Matrix::get_IsIdentity() const
{
    if (m_m11 == 1.0f && m_m12 == 0.0f && m_m21 == 0.0f && m_m22 == 1.0f && m_offsetX == 0.0f)
    {
          return m_offsetY == 0.0f;
    }

    return false;
}

float Matrix::get_Determinant() const
{
    return m_m11 * m_m22 - m_m12 * m_m21;
}                                              

bool Matrix::HasInverse() const
{
    return !Float::IsZero(get_Determinant());
}

void Matrix::Invert()
{
    float determinant = get_Determinant();
    if(Float::IsZero(determinant))
    {
        LibraryCriticalError();
    }

    float invDet = 1.0f / determinant;

    *this = Matrix(m_m22 * invDet, -m_m12 * invDet, -m_m21 * invDet, m_m11 * invDet,
                   (m_m21 * m_offsetY - m_offsetX * m_m22) * invDet,
                   (m_m12 * m_offsetX - m_m11 * m_offsetY) * invDet);
}

Point Matrix::Transform(const Point& pt) const
{
    float x = pt.get_X();
    float y = pt.get_Y();

    float num1 = y * m_m21 + m_offsetX;
    float num2 = x * m_m12 + m_offsetY;

    x *= m_m11;
    x += num1;

    y *= m_m22;
    y += num2;

    return Point(x, y);
}

Vector Matrix::Transform(const Vector& vec) const
{
    float x = vec.get_X();
    float y = vec.get_Y();

    float num1 = y * m_m21 + m_offsetX;
    float num2 = x * m_m12 + m_offsetY;

    x *= m_m11;
    x += num1;

    y *= m_m22;
    y += num2;

    return Vector(x, y);
}


Rect Matrix::TransformBounds(const Rect& rect) const
{
    Point ptTL = Transform(Point(rect.get_Left(), rect.get_Top()));
    Point ptBL = Transform(Point(rect.get_Left(), rect.get_Bottom()));
    Point ptBR = Transform(Point(rect.get_Right(), rect.get_Bottom()));
    Point ptTR = Transform(Point(rect.get_Right(), rect.get_Top()));

    float minX = Float::Min(Float::Min(Float::Min(ptTL.get_X(), ptBL.get_X()), ptBR.get_X()), ptTR.get_X());
    float maxX = Float::Max(Float::Max(Float::Max(ptTL.get_X(), ptBL.get_X()), ptBR.get_X()), ptTR.get_X());

    float minY = Float::Min(Float::Min(Float::Min(ptTL.get_Y(), ptBL.get_Y()), ptBR.get_Y()), ptTR.get_Y());
    float maxY = Float::Max(Float::Max(Float::Max(ptTL.get_Y(), ptBL.get_Y()), ptBR.get_Y()), ptTR.get_Y());

    return Rect(minX, minY, maxX - minX, maxY - minY);
}


void Matrix::Translate(float offsetX, float offsetY)
{
    m_offsetX += offsetX;
    m_offsetY += offsetY;
}

void Matrix::Skew(float skewX, float skewY)
{
    skewX = fmodf(skewX, 360.0f);
    skewY = fmodf(skewY, 360.0f);

    (*this) = MultiplyMatrix(*this, CreateSkewRadians(skewX * 0.017453292519943295f, skewY * 0.017453292519943295f));
}

Matrix Matrix::CreateTranslation(float translateX, float translateY) { return Matrix(1.0f, 0.0f, 0.0f, 1.0f, translateX, translateY); }

void Matrix::ScaleAt(float scaleX, float scaleY, float centerX, float centerY)
{
   (*this) = MultiplyMatrix(*this, CreateScaling(scaleX, scaleY, centerX, centerY));
}

void Matrix::Scale(float scaleX, float scaleY)
{
   (*this) = MultiplyMatrix(*this, CreateScaling(scaleX, scaleY));
}

Matrix Matrix::CreateSkewRadians(float skewX, float skewY)
{
    return Matrix(1.0f, tanf(skewY), tanf(skewX), 1.0f, 0.0f, 0.0f);
}

Matrix Matrix::CreateScaling(float scaleX, float scaleY, float centerX, float centerY)
{
    return Matrix(scaleX, 0.0f, 0.0f, scaleY, centerX - (scaleX * centerX), centerY - (scaleY * centerY));
}

Matrix Matrix::CreateScaling(float scaleX, float scaleY)
{
    return Matrix(scaleX, 0.0f, 0.0f, scaleY, 0.0f, 0.0f);
}


}; // namespace AVUI
