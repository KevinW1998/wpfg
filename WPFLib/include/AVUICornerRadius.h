#pragma once

namespace AVUI
{

class CornerRadius
{
public:
    CornerRadius(float uniformRadius)
    {
        m_topLeft = m_topRight = m_bottomRight = m_bottomLeft = uniformRadius;
    }

    CornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft) 
    {
        set_TopLeft(topLeft);
        set_TopRight(topRight);
        set_BottomRight(bottomRight);
        set_BottomLeft(bottomLeft);
    }

    float get_TopLeft() const { return m_topLeft; };
    void set_TopLeft(float topLeft) { m_topLeft = topLeft; };

    float get_TopRight() const { return m_topRight; };
    void set_TopRight(float topRight) { m_topRight = topRight; };

    float get_BottomLeft() const { return m_bottomLeft; };
    void set_BottomLeft(float bottomLeft) { m_bottomLeft = bottomLeft; };

    float get_BottomRight() const { return m_bottomRight; };
    void set_BottomRight(float bottomRight) { m_bottomRight = bottomRight; };


    bool operator==(const CornerRadius& rhs) const { return m_topLeft == rhs.m_topLeft && m_topRight == rhs.m_topRight && 
                                                            m_bottomLeft == rhs.m_bottomLeft && m_bottomRight == rhs.m_bottomRight; }

private:

    float m_topLeft;
    float m_topRight;
    float m_bottomRight;
    float m_bottomLeft;
};

class CornerRadiusTypeConverter : public StdValueTypeConverter<CornerRadius>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const CornerRadius& value) const;
};


};
