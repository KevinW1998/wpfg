#include <AVUICommon.h>
#include <AVUIGeometryHelper.h>
#include <AVUIMatrix.h>

namespace AVUI
{

void GeometryHelper::GetArcAngle(const Point& ptStart, const Point& ptEnd, bool fIsLargeArc, SweepDirection::Enum eSweepDirection, 
                 float* pfCosArcAngle, float* pfSinArcAngle, int* pcPieces)
{
    *pfCosArcAngle = DotProduct(ptStart, ptEnd);
    *pfSinArcAngle = Determinant(ptStart, ptEnd);
    if (*pfCosArcAngle >= 0.0f)
    {
        if (!fIsLargeArc)
        {
            *pcPieces = 1;
            return;
        }
        *pcPieces = 4;
    }
    else if (fIsLargeArc)
    {
        *pcPieces = 3;
    }
    else
    {
        *pcPieces = 2;
    }
    float d = atan2f(*pfSinArcAngle, *pfCosArcAngle);
    if (eSweepDirection == SweepDirection::Clockwise)
    {
        if (d < 0.0f)
        {
            d += 6.2831853071795862f;
        }
    }
    else if (d > 0.0f)
    {
        d -= 6.2831853071795862f;
    }
    d /= (float) *pcPieces;
    *pfCosArcAngle = cosf(d);
    *pfSinArcAngle = sinf(d);
}

float GeometryHelper::GetBezierDistance(float rDot, float rRadius)
{
    float num = rRadius * rRadius;
    float num2 = 0.0f;
    float d = (num + rDot) / 2.0f;
    if (d < 0.0f)
    {
        return num2;
    }
    float num4 = num - d;
    if (num4 <= 0.0f)
    {
        return num2;
    }
    num4 = sqrtf(num4);
    d = sqrtf(d);
    num2 = (4.0f * (rRadius - d)) / 3.0f;
    if (num2 <= (num4 * 1E-06f))
    {
        return 0.0f;
    }
    return (((4.0f * (rRadius - d)) / num4) / 3.0f);
}

float GeometryHelper::DotProduct(const Point& a, const Point& b)
{
    return ((a.get_X() * b.get_X()) + (a.get_Y() * b.get_Y()));
}

float GeometryHelper::Determinant(const Point& a, const Point& b)
{
    return ((a.get_X() * b.get_Y()) - (a.get_Y() * b.get_X()));
}


bool GeometryHelper::AcceptRadius(float rHalfChord2, float rFuzz2, float* pfRadius)
{
    bool flag = ((*pfRadius) * (*pfRadius)) > (rHalfChord2 * rFuzz2);
    if (flag && ((*pfRadius) < 0.0f))
    {
        (*pfRadius) = 0.0f;
    }
    return flag;
}

void GeometryHelper::ArcToBezier(float xStart, float yStart, float xRadius, float yRadius, float rRotation, 
                                 bool fIsLargeArc, SweepDirection::Enum eSweepDirection, float xEnd, float yEnd, 
                                 Point* rgPointsOut, int* pcCurves)
{
    float num;
    float num2;
    float num3;
    float num4;
    Point point2;
    float num7 = 1E-12f;
    bool flag = false;

    *pcCurves = 0;

    float num8 = (xEnd - xStart) / 2.0f;
    float num9 = (yEnd - yStart) / 2.0f;
    float num10 = (num8 * num8) + (num9 * num9);
    if (num10 < num7)
    {
        return;
    }
    if (!AcceptRadius(num10, num7, &xRadius) || !AcceptRadius(num10, num7, &yRadius))
    {
        return;
    }
    rRotation = -rRotation * 0.017453292519943295f;
    float num11 = cosf(rRotation);
    float num12 = sinf(rRotation);
    float num5 = (num8 * num11) - (num9 * num12);
    num9 = (num8 * num12) + (num9 * num11);
    num8 = num5;
    num8 /= xRadius;
    num9 /= yRadius;
    num10 = (num8 * num8) + (num9 * num9);
    if (num10 > 1.0f)
    {
        num5 = sqrtf(num10);
        xRadius *= num5;
        yRadius *= num5;
        num3 = num4 = 0.0f;
        flag = true;
        num8 /= num5;
        num9 /= num5;
    }
    else
    {
        num5 = sqrtf((1.0f - num10) / num10);
        if (fIsLargeArc != (eSweepDirection == SweepDirection::Clockwise))
        {
            num3 = -num5 * num9;
            num4 = num5 * num8;
        }
        else
        {
            num3 = num5 * num9;
            num4 = -num5 * num8;
        }
    }
    Point ptStart(-num8 - num3, -num9 - num4);
    Point ptEnd(num8 - num3, num9 - num4);
    Matrix matrix(num11 * xRadius, -num12 * xRadius, num12 * yRadius, num11 * yRadius, (xEnd + xStart) / 2.0f, (yEnd + yStart) / 2.0f);
    if (!flag)
    {
        matrix.set_OffsetX(matrix.get_OffsetX() + (matrix.GetM11() * num3) + (matrix.GetM21() * num4));
        matrix.set_OffsetY(matrix.get_OffsetY() + (matrix.GetM12() * num3) + (matrix.GetM22() * num4));
    }

    GetArcAngle(ptStart, ptEnd, fIsLargeArc, eSweepDirection, &num, &num2, pcCurves);

    float bezierDistance = GetBezierDistance(num, 1.0f);
    if (eSweepDirection == SweepDirection::Counterclockwise)
    {
        bezierDistance = -bezierDistance;
    }
    Point b(-bezierDistance * ptStart.get_Y(), bezierDistance * ptStart.get_X());

    for (int i = 1; i < (*pcCurves); i++)
    {
        Point a((ptStart.get_X() * num) - (ptStart.get_Y() * num2), (ptStart.get_X() * num2) + (ptStart.get_Y() * num));
        point2 = Point(-bezierDistance * a.get_Y(), bezierDistance * a.get_X());
        
        rgPointsOut[0] = matrix.Transform(Add(ptStart, b));
        rgPointsOut[1] = matrix.Transform(Sub(a, point2));
        rgPointsOut[2] = matrix.Transform(a);

        ptStart = a;
        b = point2;

        rgPointsOut += 3;
    }
    point2 = Point(-bezierDistance * ptEnd.get_Y(), bezierDistance * ptEnd.get_X());

    rgPointsOut[0] = matrix.Transform(Add(ptStart, b));
    rgPointsOut[1] = matrix.Transform(Sub(ptEnd, point2));
    rgPointsOut[2] = Point(xEnd, yEnd);
}

Point GeometryHelper::Add(const Point& a, const Point& b)
{
    return Point(a.get_X() + b.get_X(), a.get_Y() + b.get_Y());
}

Point GeometryHelper::Sub(const Point& a, const Point& b)
{
    return Point(a.get_X() - b.get_X(), a.get_Y() - b.get_Y());
}



};
