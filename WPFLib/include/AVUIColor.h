#pragma once

namespace AVUI {

struct Color
{
public:
    Color() : m_r(0.0f), m_g(0.0f), m_b(0.0f), m_a(0.0f) { }
    Color(float r, float g, float b, float a = 1.0f) : m_r(r), m_g(g), m_b(b), m_a(a) { };
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) : m_r(r / 255.0f), m_g(g / 255.0f), m_b(b / 255.0f), m_a(a / 255.0f) { };
    Color(unsigned int argb);

    unsigned int get_ARGB() const;

    Color operator+(const Color& rhs) const;
    Color operator-(const Color& rhs) const;
    Color operator*(float coeff) const;

    bool operator==(const Color& rhs) const { return m_r == rhs.m_r && m_g == rhs.m_g && m_b == rhs.m_b && m_a == rhs.m_a; }

    float get_R() const { return m_r; }
    float get_G() const { return m_g; }
    float get_B() const { return m_b; }
    float get_A() const { return m_a; }

private:

    float m_r;
    float m_g;
    float m_b;
    float m_a;
};


class ColorTypeConverter : public StdValueTypeConverter<Color>
{
protected:
    virtual ObjectPtr ConvertFromString(const String* pString) const;
    virtual ObjectPtr ConvertToString(const Color& value) const;

private:
    static unsigned char ParseHexChar(WCHAR wch);
};

class NullableColorTypeConverter : public NullableTypeConverter<Color> { };

};
