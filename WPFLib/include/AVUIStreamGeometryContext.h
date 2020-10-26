#include <AVUIObject.h>

namespace AVUI {

class StreamGeometry;
class StreamGeometryStream;

class StreamGeometryContext : public Object
{
public:

    StreamGeometryContext(StreamGeometry* pGeometry);
    ~StreamGeometryContext();

    void ArcTo(const Point& point, const Size& size, float rotationAngle, bool isLargeArc, SweepDirection::Enum sweepDirection, bool isStroked, bool isSmoothJoin);
    void BeginFigure(const Point& startPoint, bool isFilled, bool isClosed);
    void BezierTo(const Point& point1, const Point& point2, const Point& point3, bool isStroked, bool isSmoothJoin);
    void Close();
    void LineTo(const Point& point, bool isStroked, bool isSmoothJoin);
    void QuadraticBezierTo(const Point& point1, const Point& point2, bool isStroked, bool isSmoothJoin);

private:
    TRefCountedPtr<StreamGeometry> m_pGeometry;
    TRefCountedPtr<StreamGeometryStream> m_pGeometryStream;
    bool m_isClosed;
};

 


}; // namespace AVUI
