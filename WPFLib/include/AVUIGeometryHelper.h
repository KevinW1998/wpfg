namespace AVUI
{

class GeometryHelper
{
public:

    static void ArcToBezier(float xStart, float yStart, float xRadius, float yRadius, float rRotation, 
                            bool fIsLargeArc, SweepDirection::Enum eSweepDirection, float xEnd, float yEnd, 
                            Point* rgPointsOut, int* pcCurves);

    static bool AcceptRadius(float rHalfChord2, float rFuzz2, float* pfRadius);
    static float GetBezierDistance(float rDot, float rRadius);

    static void GetArcAngle(const Point& ptStart, const Point& ptEnd, bool fIsLargeArc, SweepDirection::Enum eSweepDirection, 
                            float* pfCosArcAngle, float* pfSinArcAngle, int* pcPieces);

    static float DotProduct(const Point& a, const Point& b);
    static float Determinant(const Point& a, const Point& b);
    static Point Add(const Point& a, const Point& b);
    static Point Sub(const Point& a, const Point& b);

};


};




