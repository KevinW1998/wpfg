#include <AVUITransform.h>

namespace AVUI {

class SkewTransform : public Transform
{
public:
    SkewTransform() { };
    SkewTransform(float angleX, float angleY);
    SkewTransform(float angleX, float angleY, float centerX, float centerY);
    ~SkewTransform() { }

    DECLARE_ELEMENT(SkewTransform, Transform);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(AngleX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(AngleY, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CenterY, float, 0.0f, PropertyFlag::None);

    virtual Matrix get_Value();

};


typedef TRefCountedPtr<SkewTransform> SkewTransformPtr;


}; // namespace AVUI
