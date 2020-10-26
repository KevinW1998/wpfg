#pragma once

namespace AVUI
{

class AnimatedTypeHelpers
{
public:
    template <class T> 
    static T Add(const T& one, const T& two) { return one + two; }

    template <class T> 
    static T Subtract(const T& one, const T& two) { return one - two; }

    template <class T> 
    static T Scale(const T& one, float scale) { return one * scale; }

    template <class T> 
    static T Interpolate(const T& from, const T& to, float progress) { return from + (to - from) * progress; }

    template <class T> 
    static T GetZeroValue() { return T(); }
};

template <> 
inline float AnimatedTypeHelpers::GetZeroValue<float>() { return 0.0f; }

template <> 
inline int AnimatedTypeHelpers::GetZeroValue<int>() { return 0; }

template <> 
inline short AnimatedTypeHelpers::GetZeroValue<short>() { return 0; }

template <> 
inline double AnimatedTypeHelpers::GetZeroValue<double>() { return 0.0; }

template <> 
inline bool AnimatedTypeHelpers::GetZeroValue<bool>() { return false; }

template <> 
inline Quaternion AnimatedTypeHelpers::GetZeroValue<Quaternion>() { return Quaternion::get_Identity(); }

template <> 
inline Quaternion AnimatedTypeHelpers::Add<Quaternion>(const Quaternion& one, const Quaternion& two) 
{ 
    return one * two; 
}

template <> 
inline Quaternion AnimatedTypeHelpers::Subtract<Quaternion>(const Quaternion& one, const Quaternion& two) 
{ 
    Quaternion twoInv = two;

    twoInv.Invert();

    return one * two; 
}

template <> 
inline Quaternion AnimatedTypeHelpers::Scale<Quaternion>(const Quaternion& one, float scale) 
{ 
    return Quaternion(one.get_Axis(), one.get_Angle() * scale);
}

template <> 
inline Quaternion AnimatedTypeHelpers::Interpolate<Quaternion>(const Quaternion& from, const Quaternion& to, float progress) 
{ 
    return Quaternion::Slerp(from, to, progress);
}


template <> 
inline Point AnimatedTypeHelpers::Subtract<Point>(const Point& one, const Point& two) 
{
    return Point(one.get_X() - two.get_X(), one.get_Y() - two.get_Y());
}


class AnimatedReferenceTypeHelpers
{
public:
    template <class T> 
    static TRefCountedPtr<T> Add(T* pOne, T* pTwo);

    template <class T> 
    static TRefCountedPtr<T> Subtract(T* pOne, T* pTwo);

    template <class T> 
    static TRefCountedPtr<T> Scale(T* pOne, float scale);

    template <class T> 
    static TRefCountedPtr<T> Interpolate(T* pFrom, T* pTo, float progress);

    template <class T> 
    static TRefCountedPtr<T> GetZeroValue(T*);

};



}; // namespace AVUI
