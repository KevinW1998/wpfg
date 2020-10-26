#pragma once

#include <AVUITypeConverter.h>

namespace AVUI {

class Vector3DConverter : public StdValueTypeConverter<Vector3D>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const Vector3D& value) const;
};
class NullableVector3DConverter : public NullableTypeConverter<Vector3D> { };

}
