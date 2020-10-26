#include <AVUICommon.h>
#include <AVUIPoint3D.h>
#include <AVUIPoint3DConverter.h>
#include <AVUITokenizerHelper.h>
#include <AVUIStringUtil.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::Point3D)
    REGISTER_TYPE_CONVERTER(Point3DConverter)
    REGISTER_PROPERTY(X);
    REGISTER_PROPERTY(Y);
    REGISTER_PROPERTY(Z);
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(Nullable<AVUI::Point3D>)
    REGISTER_TYPE_CONVERTER(NullablePoint3DConverter)
REGISTER_VALUE_TYPE_END()

namespace AVUI
{

Point3D Point3D::Parse(const String* pString)
{
    TokenizerHelper helper(pString);
    StringPtr pText = helper.NextTokenRequired();

    float x = Float::Parse(pText);
    float y = Float::Parse(helper.NextTokenRequired());
    float z = Float::Parse(helper.NextTokenRequired());

    Point3D pointd(x, y, z);

    helper.LastTokenRequired();

    return pointd;    
}

Point3D Point3D::operator+(const Point3D& rhs) const
{
    return Point3D(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z);
}

Point3D Point3D::operator-(const Point3D& rhs) const
{
    return Point3D(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
}

Point3D Point3D::operator*(float coeff) const
{
    return Point3D(m_x * coeff, m_y * coeff, m_z * coeff);
}

Point3D Point3D::operator/(float coeff) const
{
    return Point3D(m_x / coeff, m_y / coeff, m_z / coeff);
}

ObjectPtr Point3DConverter::ConvertFromString(const String* pString) const
{
    return BoxValue(Point3D::Parse(pString));
}

ObjectPtr Point3DConverter::ConvertToString(const Point3D& value) const
{
    WCHAR convertBuf[240];

    StringUtil::Print(convertBuf, sizeof(convertBuf) / sizeof(WCHAR), L"%f, %f, %f", value.get_X(), value.get_Y(), value.get_Z());

    return (Object*) object_allocate<String>(convertBuf);
}



}
