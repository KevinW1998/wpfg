#include <AVUITransform3D.h>

namespace AVUI {

class TranslateTransform3D : public Transform3D
{
public:
    TranslateTransform3D() { };
    TranslateTransform3D(const Vector3D& offset);
    TranslateTransform3D(float offsetX, float offsetY, float offsetZ);
    ~TranslateTransform3D() { }

    DECLARE_ELEMENT(TranslateTransform3D, Transform3D);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(OffsetX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(OffsetY, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(OffsetZ, float, 0.0f, PropertyFlag::None);

    virtual Matrix3D get_Value();
};


typedef TRefCountedPtr<TranslateTransform3D> TranslateTransform3DPtr;


}; // namespace AVUI
