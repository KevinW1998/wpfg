#pragma once

#include <AVUIMaterial.h>
#include <AVUIColor.h>

namespace AVUI {

class Brush;


class EmissiveMaterial : public Material
{
public:
    EmissiveMaterial() { };

    DECLARE_ELEMENT(EmissiveMaterial, Material);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Brush, Brush, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Color, Color, Color(), PropertyFlag::None);

};


}; // namespace AVUI
