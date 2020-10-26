#include <AVUICommon.h>
#include <AVUIVector3D.h>
#include <AVUIVector3DConverter.h>
#include <AVUITokenizerHelper.h>
#include <AVUIStringUtil.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::Vector3D)
    REGISTER_TYPE_CONVERTER(Vector3DConverter)
    REGISTER_PROPERTY(X);
    REGISTER_PROPERTY(Y);
    REGISTER_PROPERTY(Z);
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(Nullable<AVUI::Vector3D>)
    REGISTER_TYPE_CONVERTER(NullableVector3DConverter)
REGISTER_VALUE_TYPE_END()


namespace AVUI
{

Vector3D Vector3D::Parse(const String* pString)
{
    TokenizerHelper helper(pString);
    StringPtr pText = helper.NextTokenRequired();

    float x = Float::Parse(pText);
    float y = Float::Parse(helper.NextTokenRequired());
    float z = Float::Parse(helper.NextTokenRequired());

    Vector3D vectord(x, y, z);

    helper.LastTokenRequired();

    return vectord;    
}

Vector3D Vector3D::operator*(float coeff) const
{
    return Vector3D(m_x * coeff, m_y * coeff, m_z * coeff);
}

Vector3D Vector3D::operator/(float coeff) const
{
    return Vector3D(m_x / coeff, m_y / coeff, m_z / coeff);
}

Vector3D Vector3D::operator+(const Vector3D& rhs) const
{
    return Vector3D(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z);
}

Vector3D Vector3D::operator-(const Vector3D& rhs) const
{
    return Vector3D(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
}

ObjectPtr Vector3DConverter::ConvertFromString(const String* pString) const
{
    return BoxValue(Vector3D::Parse(pString));
}

ObjectPtr Vector3DConverter::ConvertToString(const Vector3D& value) const
{
    WCHAR convertBuf[240];

    StringUtil::Print(convertBuf, sizeof(convertBuf) / sizeof(WCHAR), L"%f, %f, %f", value.get_X(), value.get_Y(), value.get_Z());

    return (Object*) object_allocate<String>(convertBuf);
}

}
