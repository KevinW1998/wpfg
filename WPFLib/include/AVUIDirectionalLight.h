#pragma once

#include <AVUILight.h>

namespace AVUI {

class DirectionalLight : public Light
{
public:

    DECLARE_ELEMENT(DirectionalLight, Light);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Direction, Vector3D, Vector3D(0.0f, 0.0f, -1.0f), PropertyFlag::None);
};


}; // namespace AVUI
