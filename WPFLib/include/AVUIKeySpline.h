#pragma once

#include <AVUIKeySpline.h>
#include <AVUIPoint.h>
#include <AVUIAnimatable.h>

namespace AVUI {

class KeySplineTypeConverter : public TypeConverter
{
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const { return false; };
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { LibraryCriticalError(); return NULL; }
};

class KeySpline : public Animatable
{
public:
    DECLARE_ELEMENT(KeySpline, Animatable);

    IMPLEMENT_VALUE_PROPERTY(ControlPoint1, Point, false);
    IMPLEMENT_VALUE_PROPERTY(ControlPoint2, Point, false);

    Point get_ControlPoint1() const { return m_controlPoint1; }
    void set_ControlPoint1(const Point& pt) { m_controlPoint1 = pt; m_isDirty = true; }

    Point get_ControlPoint2() const { return m_controlPoint2; }
    void set_ControlPoint2(const Point& pt) { m_controlPoint2 = pt; m_isDirty = true; }

    float GetSplineProgress(float linearProgress);

private:

    void Build();
    float GetBezierValue(float b, float c, float t);
    void SetParameterFromX(float x);
    void GetXAndDx(float t, float* px, float* pdx);

 

    Point m_controlPoint1;
    Point m_controlPoint2;
    float m_bx;
    float m_by;
    float m_cx;
    float m_cy;
    float m_cx_bx;
    bool m_isDirty;
    bool m_isSpecified;
    float m_parameter;
    float m_three_cx;
};


}; // namespace AVUI
