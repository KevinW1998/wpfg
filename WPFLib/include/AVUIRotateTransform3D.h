#include <AVUITransform3D.h>
#include <AVUIRotation3D.h>

namespace AVUI {

class RotateTransform3D : public Transform3D
{
public:
    RotateTransform3D() { };
    RotateTransform3D(Rotation3D* pRotation) { set_Rotation(pRotation); };
    RotateTransform3D(Rotation3D* pRotation, const Point3D& center);
    ~RotateTransform3D() { }

    DECLARE_ELEMENT(RotateTransform3D, Transform3D);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterY, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterZ, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Rotation, Rotation3D, Rotation3D::get_Identity(), PropertyFlag::None);

    virtual Matrix3D get_Value();
};


typedef TRefCountedPtr<RotateTransform3D> RotateTransform3DPtr;


}; // namespace AVUI
