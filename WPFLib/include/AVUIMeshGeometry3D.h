#pragma once

#include <AVUIGeometry3D.h>

namespace AVUI {

class Point3DCollection;
class Vector3DCollection;
class PointCollection;
class Int32Collection;


class MeshGeometry3D : public Geometry3D
{
public:
    MeshGeometry3D();
    ~MeshGeometry3D();

    DECLARE_ELEMENT(MeshGeometry3D, Geometry3D);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Positions, Point3DCollection, NULL, PropertyFlag::AutoCreate);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Normals, Vector3DCollection, NULL, PropertyFlag::AutoCreate);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(TextureCoordinates, PointCollection, NULL, PropertyFlag::AutoCreate);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(TriangleIndices, Int32Collection, NULL, PropertyFlag::AutoCreate);

    virtual IClientRenderGeometry* GetRenderGeometry(Renderer* pRenderer);

private:

    void OnRenderDeviceLost(Object* pObject, EventArgs* pEventArgs);

    IClientRenderGeometry* m_pClientGeometry;
};


}; // namespace AVUI
