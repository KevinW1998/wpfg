#pragma once

namespace AVUI
{

class Thickness
{
public:
    Thickness(float uniformThickness)
    {
        m_top = m_bottom = m_left = m_right = uniformThickness;
    }

    Thickness(float left, float top, float right, float bottom) 
    {
        set_Left(left);
        set_Right(right);
        set_Top(top);
        set_Bottom(bottom);
    }

    float get_Top() const { return m_top; };
    void set_Top(float top) { m_top = top; };

    float get_Bottom() const { return m_bottom; };
    void set_Bottom(float bottom) { m_bottom = bottom; };

    float get_Left() const { return m_left; };
    void set_Left(float left) { m_left = left; };

    float get_Right() const { return m_right; };
    void set_Right(float right) { m_right = right; };

    bool operator==(const Thickness& rhs) const { return m_top == rhs.m_top && m_bottom == rhs.m_bottom && m_left == rhs.m_left && m_right == rhs.m_right; }

    bool get_IsUniform() const;
    bool get_IsZero() const;

private:

    float m_top;
    float m_bottom;
    float m_left;
    float m_right;
};

class ThicknessTypeConverter : public StdValueTypeConverter<Thickness>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const Thickness& value) const;
};


};
