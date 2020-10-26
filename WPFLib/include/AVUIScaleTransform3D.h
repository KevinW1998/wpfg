#include <AVUITransform3D.h>

namespace AVUI {

class ScaleTransform3D : public Transform3D
{
public:
    ScaleTransform3D() { };
    ~ScaleTransform3D() { }

    DECLARE_ELEMENT(ScaleTransform3D, Transform3D);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterY, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterZ, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ScaleX, float, 1.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ScaleY, float, 1.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ScaleZ, float, 1.0f, PropertyFlag::None);

    virtual Matrix3D get_Value();
};


typedef TRefCountedPtr<ScaleTransform3D> ScaleTransform3DPtr;


}; // namespace AVUI
