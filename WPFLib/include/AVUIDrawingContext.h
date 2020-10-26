#pragma once

#include <AVUIObject.h>

namespace AVUI {

class Brush;
class Pen;
class Point;
class AnimationClock;
class Geometry;
class Transform;
class ImageSource;
class FormattedText;
class Transform;

class DrawingContext : public Object
{
public:
    virtual ~DrawingContext() { };
    virtual void DrawEllipse(Brush* pBrush, Pen* pPen, const Point& center, float radiusX, float radiusY) = 0;
    virtual void DrawLine(Pen* pPen, const Point& point0, const Point& point1) = 0;
    virtual void DrawRoundedRectangle(Brush* pBrush, Pen* pPen, const Rect& rectangle, float radiusX, float radiusY) = 0; 
    virtual void DrawGeometry(Brush* pBrush, Pen* pPen, Geometry* pGeometry, Transform* pTransform) = 0;
    virtual void DrawFormattedText(Brush* pBrush, FormattedText* pFormattedText, const Point& origin) = 0;

    DECLARE_ELEMENT(DrawingContext, Object);
};

 


}; // namespace AVUI
