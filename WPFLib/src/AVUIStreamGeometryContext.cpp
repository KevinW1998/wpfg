#include <AVUICommon.h>
#include <AVUIStreamGeometryContext.h>
#include <AVUIStreamGeometryStream.h>
#include <AVUIStreamGeometry.h>

namespace AVUI {

StreamGeometryContext::StreamGeometryContext(StreamGeometry* pGeometry)
{
    if(pGeometry == NULL)
    {
        LibraryCriticalError();
    }

    m_pGeometry = pGeometry;
    m_pGeometryStream = object_allocate<StreamGeometryStream>();
    m_isClosed = false;
}

StreamGeometryContext::~StreamGeometryContext()
{
    if(!m_isClosed)
    {
        Close();
    }
}

void StreamGeometryContext::ArcTo(const Point& point, const Size& size, float rotationAngle, bool isLargeArc, SweepDirection::Enum sweepDirection, bool isStroked, bool isSmoothJoin)
{
    StreamGeometryStream::ArcToRecord arcRecord;

    arcRecord.recordType = StreamGeometryStream::RecordType::ArcTo;
    arcRecord.sizeOfRecord = sizeof(arcRecord);
    arcRecord.point = point;
    arcRecord.size = size;
    arcRecord.rotationAngle = rotationAngle;
    arcRecord.isLargeArc = isLargeArc;
    arcRecord.sweepDirection = sweepDirection;
    arcRecord.isStroked = isStroked;
    arcRecord.isSmoothJoin = isSmoothJoin;

    m_pGeometryStream->WriteRecord(&arcRecord);
}

void StreamGeometryContext::BeginFigure(const Point& startPoint, bool isFilled, bool isClosed)
{
    StreamGeometryStream::BeginFigureRecord beginFigureRecord;

    beginFigureRecord.recordType = StreamGeometryStream::RecordType::BeginFigure;
    beginFigureRecord.sizeOfRecord = sizeof(beginFigureRecord);
    beginFigureRecord.startPoint = startPoint;
    beginFigureRecord.isFilled = isFilled;
    beginFigureRecord.isClosed = isClosed;

    m_pGeometryStream->WriteRecord(&beginFigureRecord);
}

void StreamGeometryContext::BezierTo(const Point& point1, const Point& point2, const Point& point3, bool isStroked, bool isSmoothJoin)
{
    StreamGeometryStream::BezierToRecord bezierToRecord;

    bezierToRecord.recordType = StreamGeometryStream::RecordType::BezierTo;
    bezierToRecord.sizeOfRecord = sizeof(bezierToRecord);
    bezierToRecord.point1 = point1;
    bezierToRecord.point2 = point2;
    bezierToRecord.point3 = point3;
    bezierToRecord.isStroked = isStroked;
    bezierToRecord.isSmoothJoin = isSmoothJoin;

    m_pGeometryStream->WriteRecord(&bezierToRecord);
}

void StreamGeometryContext::LineTo(const Point& point, bool isStroked, bool isSmoothJoin)
{
    StreamGeometryStream::LineToRecord lineToRecord;

    lineToRecord.recordType = StreamGeometryStream::RecordType::LineTo;
    lineToRecord.sizeOfRecord = sizeof(lineToRecord);
    lineToRecord.point = point;
    lineToRecord.isStroked = isStroked;
    lineToRecord.isSmoothJoin = isSmoothJoin;

    m_pGeometryStream->WriteRecord(&lineToRecord);
}

void StreamGeometryContext::QuadraticBezierTo(const Point& point1, const Point& point2, bool isStroked, bool isSmoothJoin)
{
    StreamGeometryStream::QuadraticBezierToRecord quadraticBezierToRecord;

    quadraticBezierToRecord.recordType = StreamGeometryStream::RecordType::QuadraticBezierTo;
    quadraticBezierToRecord.sizeOfRecord = sizeof(quadraticBezierToRecord);
    quadraticBezierToRecord.point1 = point1;
    quadraticBezierToRecord.point2 = point2;
    quadraticBezierToRecord.isStroked = isStroked;
    quadraticBezierToRecord.isSmoothJoin = isSmoothJoin;

    m_pGeometryStream->WriteRecord(&quadraticBezierToRecord);
}

void StreamGeometryContext::Close()
{
    if(!m_isClosed)
    {
        m_pGeometry->set_GeometryStream(m_pGeometryStream);
        m_pGeometryStream = NULL;
        m_pGeometry = NULL;

        m_isClosed = true;
    }
}

}; // namespace AVUI
