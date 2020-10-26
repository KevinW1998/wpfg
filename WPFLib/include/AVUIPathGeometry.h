#include <AVUIGeometry.h>

namespace AVUI {

class PathFigureCollection;
class PathGeometry : public Geometry
{
public:
    DECLARE_ELEMENT(PathGeometry, Geometry);


    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(FillRule, FillRule, FillRule::EvenOdd, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Figures, PathFigureCollection, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AutoCreate);

    virtual Rect GetBounds();
    virtual bool HitTest(const Point& pt, int hitTestFlags, float strokeThickness);

private:
    virtual void BuildGeometry(GeometryBuilder* pGB);
    void BuildGeometryCommon(GeometryBuilder* pGB);
};


}; // namespace AVUI
