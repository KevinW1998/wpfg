#include <AVUIObject.h>

namespace AVUI {

class StreamGeometryStream : public Object
{
public:
    friend class StreamGeometry;
    friend class StreamGeometryContext;

private:

    struct RecordType
    {
        friend class StreamGeometry;
        friend class StreamGeometryContext;
    
        enum Enum
        {

            ArcTo,
            BeginFigure,
            BezierTo,
            LineTo,
            QuadraticBezierTo
        };
    };

    struct BaseRecord
    {
        RecordType::Enum recordType;
        unsigned int sizeOfRecord;
    };

    struct ArcToRecord : public BaseRecord
    {
        Point point;
        Size size;
        float rotationAngle;
        bool isLargeArc;
        SweepDirection::Enum sweepDirection;
        bool isStroked;
        bool isSmoothJoin;
    };

    struct BeginFigureRecord : public BaseRecord
    {
        Point startPoint;
        bool isFilled;
        bool isClosed;
    };

    struct BezierToRecord : public BaseRecord
    {
        Point point1;
        Point point2;
        Point point3;
        bool isStroked;
        bool isSmoothJoin;
    };

    struct LineToRecord : public BaseRecord
    {
        Point point;
        bool isStroked;
        bool isSmoothJoin;
    };

    struct QuadraticBezierToRecord : public BaseRecord
    {
        Point point1;
        Point point2;
        bool isStroked;
        bool isSmoothJoin;
    };

    void WriteRecord(BaseRecord* pRecord);
    BaseRecord* GetNextRecord(int* pcbCur);

    ItemStructList<unsigned char> m_streamBuffer;
};




}; // namespace AVUI
