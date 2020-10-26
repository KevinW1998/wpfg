#pragma once

#include <AVUITypeConverter.h>

namespace AVUI {

class Point3DConverter : public StdValueTypeConverter<Point3D>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const Point3D& value) const;
};
class NullablePoint3DConverter : public NullableTypeConverter<Point3D> { };

}
