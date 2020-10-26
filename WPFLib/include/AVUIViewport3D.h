#pragma once

#include <AVUIVisual3DCollection.h>
#include <AVUIFrameworkElement.h>

namespace AVUI {

class Camera;
class Visual3D;

class Viewport3D : public FrameworkElement
{
    friend class Visual;
public:
    DECLARE_ELEMENT(Viewport3D, FrameworkElement);

    Viewport3D();

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Camera, Camera, NULL, DependencyPropertyMetadata(PropertyFlag::None, OnCameraChanged));
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(Children, Visual3DCollection, true);

    Visual3DCollection* get_Children() { return m_pChildren; }
    const Visual3DCollection* get_Children() const { return m_pChildren; }

protected:

    virtual Size ArrangeOverride(const Size& constraintSize);

private:

    Matrix3D GetHomogeneousToViewportTransform3D(const Rect& viewport);
    virtual void OnRender(DrawingContext& drawingContext);

    void Render3D(Renderer* pRenderer, const Matrix& localMatrix, const Rect& localClipRect);
    void RenderDependencies3D(Renderer* pRenderer);

    static void OnCameraChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
    {
        ((Viewport3D*)pDO)->m_pCamera = (Camera*) e.pNewValue;
    }

    Camera* m_pCamera;
    TRefCountedPtr<Visual3DCollection> m_pChildren;
    TRefCountedPtr<Visual3D> m_pRootVisual;
};


}; // namespace AVUI
