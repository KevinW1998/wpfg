
#pragma once

#include <AVUIModel3D.h>
#include <AVUIModel3DCollection.h>

namespace AVUI {

class Geometry3D;
class Material;

class Model3DGroup : public Model3D
{
public:
    Model3DGroup() { };

    DECLARE_ELEMENT(Model3DGroup, Model3D);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Children, Model3DCollection, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AutoCreate);

    virtual void Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix);
    virtual void RenderDependencies3D(Renderer* pRenderer);
};


}; // namespace AVUI
