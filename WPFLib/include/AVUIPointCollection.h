#pragma once

#include <AVUIValueTypeCollection.h>

namespace AVUI {

class PointCollection : public ValueTypeCollection<Point>
{
public:
    DECLARE_ELEMENT(PointCollection, Object);
};


}; // namespace AVUI
