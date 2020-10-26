#pragma once

#include <AVUIValueTypeCollection.h>

namespace AVUI {

class Vector3DCollection : public ValueTypeCollection<Vector3D>
{
public:
    DECLARE_ELEMENT(Vector3DCollection, Object);
};


}; // namespace AVUI
