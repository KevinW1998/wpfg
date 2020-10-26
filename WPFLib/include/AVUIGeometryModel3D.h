
#pragma once

#include <AVUIModel3D.h>

namespace AVUI {

class Geometry3D;
class Material;

class GeometryModel3D : public Model3D
{
public:
    GeometryModel3D() { };

    DECLARE_ELEMENT(GeometryModel3D, Model3D);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Geometry, Geometry3D, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Material, Material, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(BackMaterial, Material, NULL, PropertyFlag::None);

    // Transform 3D property here

    virtual void Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix);
    virtual void RenderDependencies3D(Renderer* pRenderer);
};


}; // namespace AVUI
