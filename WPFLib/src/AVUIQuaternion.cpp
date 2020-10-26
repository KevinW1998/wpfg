#include <AVUICommon.h>
#include <AVUIQuaternion.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::Quaternion)
    REGISTER_PROPERTY(X);
    REGISTER_PROPERTY(Y);
    REGISTER_PROPERTY(Z);
    REGISTER_PROPERTY(W);
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(Nullable<AVUI::Quaternion>)
REGISTER_VALUE_TYPE_END()

namespace AVUI
{

Quaternion::Quaternion(const Vector3D& axisOfRotation, float angleInDegrees)
{
    while(angleInDegrees > 360.0f)
    {
        angleInDegrees -= 360.0f;
    }

    float num2 = angleInDegrees * 0.017453292519943295f;
    float length = axisOfRotation.get_Length();
    if (length == 0)
    {
        LibraryCriticalError();
    }
    Vector3D vectord = ((axisOfRotation / length) * sinf(0.5f * num2));
    m_x = vectord.get_X();
    m_y = vectord.get_Y();
    m_z = vectord.get_Z();
    m_w = cosf(0.5f * num2);
}

void Quaternion::Conjugate()
{
    m_x = -m_x;
    m_y = -m_y;
    m_z = -m_z;
    m_w = -m_w;
}

void Quaternion::Invert()
{
    Conjugate();
    float num = (((m_x * m_x) + (m_y * m_y)) + (m_z * m_z)) + (m_w * m_w);
    m_x /= num;
    m_y /= num;
    m_z /= num;
    m_w /= num;
}

Vector3D Quaternion::get_Axis() const
{
    Vector3D vectord(m_x, m_y, m_z);
    vectord.Normalize();
    return vectord;
}

float Quaternion::get_Angle() const
{
    float y = sqrtf(((m_x * m_x) + (m_y * m_y)) + (m_z * m_z));
    float x = m_w;
    if (y > Float::get_MaxValue())
    {
        float num = Float::Max(Float::Abs(m_x), Float::Max(Float::Abs(m_y), Float::Abs(m_z)));
        float num5 = m_x / num;
        float num4 = m_y / num;
        float num3 = m_z / num;
        y = sqrtf(((num5 * num5) + (num4 * num4)) + (num3 * num3));
        x = m_w / num;
    }
    return (atan2f(y, x) * 114.59155902616465f);
}

float Quaternion::Length()
{
    float d = (((m_x * m_x) + (m_y * m_y)) + (m_z * m_z)) + (m_w * m_w);
    if (d > Float::get_MaxValue())
    {
        float num = Float::Max(Float::Max(Float::Abs(m_x), Float::Abs(m_y)), Float::Max(Float::Abs(m_z), Float::Abs(m_w)));
        float num5 = m_x / num;
        float num4 = m_y / num;
        float num3 = m_z / num;
        float num2 = m_w / num;
        return (sqrtf((((num5 * num5) + (num4 * num4)) + (num3 * num3)) + (num2 * num2)) * num);
    }
    return sqrtf(d);
}


Quaternion Quaternion::Slerp(const Quaternion& fromIn, const Quaternion& toIn, float t, bool useShortestPath)
{
    float num3;
    float num2;
    Quaternion to(toIn);
    Quaternion from(fromIn);

    float num4 = from.Length();
    float num9 = to.Length();
    from.Scale(1.0f / num4);
    to.Scale(1.0f / num9);
    float d = (((from.m_x * to.m_x) + (from.m_y * to.m_y)) + (from.m_z * to.m_z)) + (from.m_w * to.m_w);
    if (useShortestPath)
    {
        if (d < 0.0f)
        {
            d = -d;
            to.m_x = -to.m_x;
            to.m_y = -to.m_y;
            to.m_z = -to.m_z;
            to.m_w = -to.m_w;
        }
    }
    else if (d < -1.0f)
    {
        d = -1.0f;
    }
    if (d > 1.0f)
    {
        d = 1.0f;
    }
    if (d > 0.999999f)
    {
        num3 = 1.0f - t;
        num2 = t;
    }
    else if (d < -0.9999999999f)
    {
        to = Quaternion(-from.get_Y(), from.get_X(), -from.get_W(), from.get_Z());
        float num8 = t * 3.1415926535897931f;
        num3 = cosf(num8);
        num2 = sinf(num8);
    }
    else
    {
        float num7 = acosf(d);
        float num6 = sqrtf(1.0f - (d * d));
        num3 = sinf((1.0f - t) * num7) / num6;
        num2 = sinf(t * num7) / num6;
    }
    float num5 = num4 * powf(num9 / num4, t);
    num3 *= num5;
    num2 *= num5;
    return Quaternion((num3 * from.m_x) + (num2 * to.m_x), (num3 * from.m_y) + (num2 * to.m_y), (num3 * from.m_z) + (num2 * to.m_z), (num3 * from.m_w) + (num2 * to.m_w));
}


Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
    float x = (((m_w * rhs.m_x) + (m_x * rhs.m_w)) + (m_y * rhs.m_z)) - (m_z * rhs.m_y);
    float y = (((m_w * rhs.m_y) + (m_y * rhs.m_w)) + (m_z * rhs.m_x)) - (m_x * rhs.m_z);
    float z = (((m_w * rhs.m_z) + (m_z * rhs.m_w)) + (m_x * rhs.m_y)) - (m_y * rhs.m_x);
    return Quaternion(x, y, z, (((m_w * rhs.m_w) - (m_x * rhs.m_x)) - (m_y * rhs.m_y)) - (m_z * rhs.m_z));

}

}
