#include <AVUIShape.h>

namespace AVUI {
    
class Ellipse : public Shape
{
public:
    DECLARE_ELEMENT(Ellipse, Shape);

    Ellipse();
    ~Ellipse();

private:
    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);
    virtual void OnRender(DrawingContext& drawingContext);

    Rect m_rect;
};



}; // namespace AVUI
