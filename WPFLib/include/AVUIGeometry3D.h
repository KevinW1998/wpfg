#pragma once

#include <AVUIAnimatable.h>

namespace AVUI {

class IClientRenderGeometry;
class Renderer;

class Geometry3D : public Animatable
{
public:
    Geometry3D() { };

    DECLARE_ELEMENT(Geometry3D, Animatable);

    virtual IClientRenderGeometry* GetRenderGeometry(Renderer* pRenderer) = 0;
};


}; // namespace AVUI
