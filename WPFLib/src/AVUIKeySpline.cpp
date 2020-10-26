#include <AVUICommon.h>
#include <AVUIKeySpline.h>
#include <AVUIStringUtil.h>

REGISTER_ELEMENT_BEGIN(AVUI::KeySpline)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_TYPE_CONVERTER(KeySplineTypeConverter)
    REGISTER_PROPERTY(ControlPoint1)
    REGISTER_PROPERTY(ControlPoint2)
REGISTER_ELEMENT_END()


namespace AVUI {

float KeySpline::GetSplineProgress(float linearProgress)
{
    if(m_isDirty)
    {
        Build();
    }

    if(!m_isSpecified)
    {
        return linearProgress;
    }

    SetParameterFromX(linearProgress);

    return GetBezierValue(m_by, m_cy, m_parameter);
}

void KeySpline::Build()
{
    if ((m_controlPoint1 == Point(0, 0)) && (m_controlPoint2 == Point(1, 1)))
    {
        m_isSpecified = false;
    }
    else
    {
        m_isSpecified = true;
        m_parameter = 0.0f;
        m_bx = 3 * m_controlPoint1.get_X();
        m_cx = 3 * m_controlPoint2.get_X();
        m_cx_bx = 2 * (m_cx - m_bx);
        m_three_cx = 3 - m_cx;
        m_by = 3 * m_controlPoint1.get_Y();
        m_cy = 3 * m_controlPoint2.get_Y();
    }
    m_isDirty = false;
}

float KeySpline::GetBezierValue(float b, float c, float t)
{
    float num1 = 1 - t;
    float num2 = t * t;
    return (((((b * t) * num1) * num1) + ((c * num2) * num1)) + (num2 * t));
}

void KeySpline::SetParameterFromX(float time)
{
    float num2 = 0;
    float num1 = 1;
    if (time == 0.0f)
    {
        m_parameter = 0.0f;
        return;
    }
    if (time == 1.0f)
    {
        m_parameter = 1.0f;
        return;
    }
    while ((num1 - num2) > 0.00001f)
    {
        float num4;
        float num6;

        GetXAndDx(m_parameter, &num4, &num6);

        float num5 = Float::Abs(num6);
        if (num4 > time)
        {
            num1 = m_parameter;
        }
        else
        {
            num2 = m_parameter;
        }
        if (Float::Abs((float) (num4 - time)) < (0.001f * num5))
        {
            return;
        }
        if (num5 > 0.00001f)
        {
            float num3 = m_parameter - ((num4 - time) / num6);
            if (num3 >= num1)
            {
                m_parameter = (m_parameter + num1) / 2;
            }
            else if (num3 <= num2)
            {
                m_parameter = (m_parameter + num2) / 2;
            }
            else
            {
                m_parameter = num3;
            }
        }
        else
        {
            m_parameter = (num2 + num1) / 2;
        }
    }
    return;

}

void KeySpline::GetXAndDx(float t, float* px, float* pdx)
{
    float num1 = 1 - t;
    float num2 = t * t;
    float num3 = num1 * num1;
    *px = (((m_bx * t) * num3) + ((m_cx * num2) * num1)) + (num2 * t);
    *pdx = ((m_bx * num3) + ((m_cx_bx * num1) * t)) + (m_three_cx * num2);
}




bool KeySplineTypeConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr KeySplineTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    if(pObjectFrom == NULL)
    {
        LibraryCriticalError();
        return NULL;
    }

    if(!CanConvertFrom(pObjectFrom->get_Type()))
    {
        LibraryCriticalError();
        return NULL;
    }

    const String* pString = object_cast<String>(pObjectFrom);

    float x1, y1, x2, y2;
    if(StringUtil::Scan(pString->get_Ptr(), L"%f, %f, %f, %f", &x1, &y1, &x2, &y2) != 4)
    {
        LibraryCriticalError();
    }

    TRefCountedPtr<KeySpline> pKeySpline = object_allocate<KeySpline>();

    pKeySpline->set_ControlPoint1(Point(x1, y1));
    pKeySpline->set_ControlPoint2(Point(x2, y2));

    return (Object*) pKeySpline;
}


}; // namespace AVUI

