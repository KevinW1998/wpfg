#include <AVUIShape.h>

namespace AVUI {
    
class Rectangle : public Shape
{
public:
    DECLARE_ELEMENT(Rectangle, Shape);

    Rectangle();
    ~Rectangle();

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);


    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RadiusX, float, 0.0f, PropertyFlag::AffectsRender);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RadiusY, float, 0.0f, PropertyFlag::AffectsRender);


protected:
    virtual void OnRender(DrawingContext& drawingContext);

private:
    Rect m_rect;
};



}; // namespace AVUI
