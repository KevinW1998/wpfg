#pragma once

namespace AVUI {

class Size
{
public:
    Size() : m_width(0.0f), m_height(0.0f) {};
	Size(float width, float height) : m_width(width), m_height(height) {}
	bool operator==(const Size& rhs) const { return m_width == rhs.m_width && m_height == rhs.m_height; };
    bool operator!=(const Size& rhs) const { return !(*this == rhs); };

    float get_Width() const { return m_width; }
    float get_Height() const { return m_height; }
    void  set_Width(float width) { m_width = width; }
    void  set_Height(float height) { m_height = height; }

    DECLARE_VALUE_ELEMENT(Size);
    IMPLEMENT_VALUE_PROPERTY(Width, float, false);
    IMPLEMENT_VALUE_PROPERTY(Height, float, false);

private:
	float m_width, m_height;
};


};
