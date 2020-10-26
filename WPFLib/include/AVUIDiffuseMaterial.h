#pragma once

#include <AVUIMaterial.h>
#include <AVUIColor.h>
#include <AVUIBrush.h>

namespace AVUI {

class Brush;


class DiffuseMaterial : public Material
{
public:
    DiffuseMaterial() { };

    DECLARE_ELEMENT(DiffuseMaterial, Material);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Brush, Brush, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Color, Color, Color(), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(AmbientColor, Color, Color(), PropertyFlag::None);

};


}; // namespace AVUI
