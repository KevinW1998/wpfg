#pragma once

namespace AVUI {

class Size;
class Rect;
class Vector;
class Point;

class Int
{
public:
    static int Max(int i1, int i2) { return i1 > i2 ? i1 : i2; }
    static int Min(int i1, int i2) { return i1 < i2 ? i1 : i2; }
    static int Abs(int i) { return i < 0 ? -i : i; }

    static int Parse(String* pSource);
};

class Float
{
public:
    static float Max(float fl1, float fl2) { if(fl1 > fl2) return fl1; return fl2; }
    static float Min(float fl1, float fl2) { if(fl1 < fl2) return fl1; return fl2; }
    static float Abs(float fl) { return fl < 0.0f ? -fl : fl; }
    static float Round(float fl);

    static bool IsZero(float fl) { return Float::Abs(fl) < 10 * get_Epsilon(); }
    static bool IsInfinity(float fl) { return fl == get_PositiveInfinity() || fl == get_NegativeInfinity(); }
    static bool IsPositiveInfinity(float fl) { return fl == get_PositiveInfinity(); }
    static bool IsNegativeInfinity(float fl) { return fl == get_NegativeInfinity(); }
    static bool IsNaN(float fl) 
    { 
        unsigned int num = *(unsigned int*)&fl;

        return (((num >> 23) & 0xFF) == 0xFF) && ((num & 0x7FFFFF) != 0);
    }
    static bool AreClose(const Size& size1, const Size& size2);
    static bool AreClose(const Point& point1, const Point& point2);
    static bool AreClose(const Vector& vector1, const Vector& vector2);
    static bool AreClose(float one, float two);
    static bool AreClose(const Rect& one, const Rect& two);
    static bool LessThan(float one, float two);
    static bool LessThanOrClose(float one, float two);
    static bool GreaterThan(float one, float two);
    static bool GreaterThanOrClose(float one, float two);
    static int FloatToInt(float val) 
    { 
        if (0.0f >= val)
        {
            return (int) (val - 0.5f);
        }
        return (int) (val + 0.5f);
    }

    static float get_PositiveInfinity();
    static float get_MaxValue();
    static float get_NegativeInfinity();
    static float get_NaN();
    static float get_Epsilon();
    static float get_PI();
    static float get_2PI();

    static float Parse(String* pSource);
};

class M3DUtil
{
public:
    static float DegreesToRadians(float degrees) { return (degrees * Float::get_2PI()) / 360.0f; }
};

class Visual;
class Matrix;

class PointUtil
{
public:
    static Matrix GetVisualTransform(Visual* pVisual);
};

};
