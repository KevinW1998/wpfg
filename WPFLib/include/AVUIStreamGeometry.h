#include <AVUIGeometry.h>

namespace AVUI {

class StreamGeometryContext;
class StreamGeometryStream;

class StreamGeometry : public Geometry
{
    friend class StreamGeometryContext;
public:
    DECLARE_ELEMENT(StreamGeometry, Geometry);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(FillRule, FillRule, FillRule::EvenOdd, PropertyFlag::None);


    TRefCountedPtr<StreamGeometryContext> Open();

    virtual Rect GetBounds();
    virtual bool HitTest(const Point& pt, int hitTestFlags, float strokeThickness);

private:
    virtual void BuildGeometry(GeometryBuilder* pGB);
    void BuildGeometryCommon(GeometryBuilder* pGB);

    void set_GeometryStream(StreamGeometryStream* pStream) { m_pGeometryStream = pStream; }

    TRefCountedPtr<StreamGeometryStream> m_pGeometryStream;

};


}; // namespace AVUI
