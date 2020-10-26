
#include <AVUIGeometry.h>

namespace AVUI {
    
class RectangleGeometry : public Geometry
{
public:
    DECLARE_ELEMENT(RectangleGeometry, Geometry);

    RectangleGeometry() { };
    ~RectangleGeometry() { };

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RadiusX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RadiusY, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Rect, Rect, Rect(), PropertyFlag::None);

    static void BuildGeometryStatic(GeometryBuilder* pGeometryBuilder, float radiusX, float radiusY, const Rect& rect);

    virtual Rect GetBounds();
    virtual bool HitTest(const Point& pt, int hitTestFlags, float strokeThickness);

private:
    virtual void BuildGeometry(GeometryBuilder* pGB);

};



}; // namespace AVUI
