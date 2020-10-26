#include <AVUITransform.h>

namespace AVUI {

class RotateTransform : public Transform
{
public:
    RotateTransform() { };
    ~RotateTransform() { }

    DECLARE_ELEMENT(RotateTransform, Transform);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterY, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Angle, float, 0.0f, PropertyFlag::None);

    virtual Matrix get_Value();
};


typedef TRefCountedPtr<RotateTransform> RotateTransformPtr;


}; // namespace AVUI
