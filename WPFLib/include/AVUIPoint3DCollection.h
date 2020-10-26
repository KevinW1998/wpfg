#pragma once

#include <AVUIValueTypeCollection.h>
#include <AVUIPoint3D.h>

namespace AVUI {

class Point3DCollection : public ValueTypeCollection<Point3D>
{
public:
    DECLARE_ELEMENT(Point3DCollection, Object);
};


}; // namespace AVUI
