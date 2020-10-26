#include <AVUICommon.h>
#include <math.h>
#include <AVUIBuiltinTypes.h>
#include <AVUIBuiltinTypeConverters.h>
#include <AVUIMatrix.h>
#include <AVUITransform.h>
#include <AVUIVisual.h>
#include <AVUIStringUtil.h>

#pragma warning(disable:4723)


REGISTER_ELEMENT(AVUI::BoxedValueType)
REGISTER_ELEMENT(AVUI::Array)

REGISTER_VALUE_TYPE_BEGIN(AVUI::Point)
    REGISTER_TYPE_CONVERTER(PointTypeConverter);
    REGISTER_PROPERTY(X);
    REGISTER_PROPERTY(Y);
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(AVUI::Size)
    REGISTER_PROPERTY(Width);
    REGISTER_PROPERTY(Height);
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(AVUI::Nullable<Point>)
    REGISTER_TYPE_CONVERTER(NullablePointTypeConverter);
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(bool)
    REGISTER_TYPE_CONVERTER(BoolTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(unsigned int)
    REGISTER_TYPE_CONVERTER(UnsignedIntTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(int)
    REGISTER_TYPE_CONVERTER(IntTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(unsigned short)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(short)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(Nullable<int>)
    REGISTER_TYPE_CONVERTER(NullableIntTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(float)
    REGISTER_TYPE_CONVERTER(FloatTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(Nullable<float>)
    REGISTER_TYPE_CONVERTER(NullableFloatTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(Nullable<bool>)
    REGISTER_TYPE_CONVERTER(NullableBoolTypeConverter)
REGISTER_VALUE_TYPE_END()


REGISTER_VALUE_TYPE_BEGIN(double)
    REGISTER_TYPE_CONVERTER(DoubleTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(const char*)
    REGISTER_TYPE_CONVERTER(AnsiStringTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(const WCHAR*)
    REGISTER_TYPE_CONVERTER(UnicodeStringTypeConverter)
REGISTER_VALUE_TYPE_END()


namespace AVUI {


Point Point::operator*(const Matrix& rhs) const
{
    return rhs.Transform(*this);
}

Point Point::operator+(const Point& rhs) const
{
    return Point(m_x + rhs.m_x, m_y + rhs.m_y);
}

Point Point::operator+(const Vector& rhs) const
{
    return Point(m_x + rhs.get_X(), m_y + rhs.get_Y());
}

Point Point::operator-(const Vector& rhs) const
{
    return Point(m_x - rhs.get_X(), m_y - rhs.get_Y());
}

Vector Point::operator-(const Point& rhs) const
{
    return Vector(m_x - rhs.m_x, m_y - rhs.m_y);
}

Point Point::operator*(float coeff) const
{
    return Point(m_x * coeff, m_y * coeff);
}


Point Point::operator/(float coeff) const
{
    return Point(m_x / coeff, m_y / coeff);
}


Vector Vector::operator*(float coeff) const
{
    return Vector(m_x * coeff, m_y * coeff);
}



static const float Zero = 0.0f;
float Float::get_PositiveInfinity() { return 1.0f / Zero; }
float Float::get_NegativeInfinity() { return -1.0f / Zero; }
float Float::get_NaN() { return 0.0f / Zero; }
float Float::get_Epsilon() { return (float) 1.0E-03; }
float Float::get_PI() { return (float) 3.1415926535897931f; }
float Float::get_2PI() { return 2.0f * get_PI(); }
float Float::get_MaxValue() { return (float) 1.0E+38; }

float Float::Round(float fl) { return (float)(int)fl; }

bool Float::AreClose(const Size& size1, const Size& size2)
{
    if(AreClose(size1.get_Width(), size2.get_Width()))
    {
        return AreClose(size1.get_Height(), size2.get_Height());
    }

    return false;
}

bool Float::AreClose(const Point& point1, const Point& point2)
{
    if(AreClose(point1.get_X(), point2.get_X()))
    {
        return AreClose(point1.get_Y(), point2.get_Y());
    }

    return false;
}

bool Float::AreClose(const Vector& vector1, const Vector& vector2)
{
    if(AreClose(vector1.get_X(), vector2.get_X()))
    {
        return AreClose(vector1.get_Y(), vector2.get_Y());
    }

    return false;
}


bool Float::AreClose(const Rect& rect1, const Rect& rect2)
{
    if(AreClose(rect1.get_X(), rect2.get_X()) &&
       AreClose(rect1.get_Y(), rect2.get_Y()) &&
       AreClose(rect1.get_Width(), rect2.get_Width()))
    {
        return AreClose(rect1.get_Height(), rect2.get_Height());
    }

    return false;
}

bool Float::AreClose(float one, float two)
{
    if (one == two)
    {
        return true;
    }
    float num = one - two;
    if(num < get_Epsilon())
    {
        return num > -get_Epsilon();
    }
    return false;
}

bool Float::LessThan(float one, float two)
{
    if(one < two)
    {
        return !AreClose(one, two);
    }

    return false;
}

bool Float::LessThanOrClose(float one, float two)
{
    return one < two || AreClose(one, two);
}

bool Float::GreaterThanOrClose(float one, float two)
{
    return one > two || AreClose(one, two);
}

bool Float::GreaterThan(float one, float two)
{
    return one > two && !AreClose(one, two);
}

float Float::Parse(String* pSource)
{
    float value;

    if(StringUtil::Scan(pSource->get_Ptr(), L"%f", &value) != 1)
    {
        LibraryCriticalError();
    }

    return value;
}

int Int::Parse(String* pSource)
{
    int value;

    if(StringUtil::Scan(pSource->get_Ptr(), L"%d", &value) != 1)
    {
        LibraryCriticalError();
    }

    return value;
}

Matrix PointUtil::GetVisualTransform(Visual* pVisual)
{
    if (pVisual == NULL)
    {
        return Matrix();
    }
    Matrix identity;
    Transform* pTransform = pVisual->get_VisualTransform();
    if (pTransform != NULL)
    {
        Matrix matrix2 = pTransform->get_Value();
        identity = Matrix::MultiplyMatrix(identity, matrix2);
    }
    Vector offset = pVisual->get_VisualOffset();
    identity.Translate(offset.get_X(), offset.get_Y());

    return identity;
}



}; // namespace AVUI
