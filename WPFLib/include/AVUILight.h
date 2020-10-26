#pragma once

#include <AVUIModel3D.h>
#include <AVUIColor.h>

namespace AVUI {

class Light : public Model3D
{
public:

    DECLARE_ELEMENT(Light, Model3D);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Color, Color, Color(1.0f, 1.0f, 1.0f), PropertyFlag::None);

    virtual void Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix) { };
    virtual void RenderDependencies3D(Renderer* pRenderer) { };

};


}; // namespace AVUI
