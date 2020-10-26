#pragma once

#include <AVUIVisual3D.h>
#include <AVUIVisual3DCollection.h>

namespace AVUI {

class Model3D;
class Transform3D;
    
class ModelVisual3D : public Visual3D
{
public:
    ModelVisual3D() : m_children(this) { };
    ~ModelVisual3D() 
    { 
        m_children.InvalidateWeakReferences();
    }

    DECLARE_ELEMENT(ModelVisual3D, Visual3D);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Content, Model3D, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Transform, Transform3D, NULL, PropertyFlag::None);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Children, Visual3DCollection, true);

    virtual void Render3D(Renderer* pRenderer, const Matrix3D& parentMatrix);
    virtual void RenderDependencies3D(Renderer* pRenderer);

    Visual3DCollection* get_Children() { return &m_children; }

private:
    Visual3DCollection m_children;
};


}; // namespace AVUI
