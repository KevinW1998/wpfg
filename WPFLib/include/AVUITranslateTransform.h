#include <AVUITransform.h>

namespace AVUI {

class TranslateTransform : public Transform
{
public:
    TranslateTransform() { };
    TranslateTransform(float offsetX, float offsetY);
    ~TranslateTransform() { }

    DECLARE_ELEMENT(TranslateTransform, Transform);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(X, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Y, float, 0.0f, PropertyFlag::None);

    virtual Matrix get_Value();

};


typedef TRefCountedPtr<TranslateTransform> TranslateTransformPtr;


}; // namespace AVUI
