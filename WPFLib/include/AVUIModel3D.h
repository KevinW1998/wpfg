#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {
class Renderer;
class Transform3D;

class Model3D : public Animatable
{
public:
    Model3D() { };

    DECLARE_ELEMENT(Model3D, Animatable);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Transform, Transform3D, NULL, PropertyFlag::None);

    // Transform 3D property here

    virtual void Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix) = 0;
    virtual void RenderDependencies3D(Renderer* pRenderer) = 0;
};


}; // namespace AVUI
