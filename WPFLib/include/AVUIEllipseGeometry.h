
#include <AVUIGeometry.h>

namespace AVUI {
    
class EllipseGeometry : public Geometry
{
public:
    DECLARE_ELEMENT(EllipseGeometry, Geometry);

    EllipseGeometry() { };
    ~EllipseGeometry() { };

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RadiusX, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RadiusY, float, 0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Center, Point, Point(), PropertyFlag::None);

    static void BuildGeometryStatic(GeometryBuilder* pGeometryBuilder, float radiusX, float radiusY, const Point& center);

    virtual Rect GetBounds();
    virtual bool HitTest(const Point& pt, int hitTestFlags, float strokeThickness);

private:
    virtual void BuildGeometry(GeometryBuilder* pGB);
};



}; // namespace AVUI
