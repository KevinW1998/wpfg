#include <AVUITransform.h>

namespace AVUI {

class ScaleTransform : public Transform
{
public:
    ScaleTransform() { };
    ScaleTransform(float sx, float sy) { set_ScaleX(sx); set_ScaleY(sy); };
    ~ScaleTransform() { }

    DECLARE_ELEMENT(ScaleTransform, Transform);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ScaleX, float, 1.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ScaleY, float, 1.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterY, float, 0.0f, PropertyFlag::None);

    virtual Matrix get_Value();
};


typedef TRefCountedPtr<ScaleTransform> ScaleTransformPtr;


}; // namespace AVUI
