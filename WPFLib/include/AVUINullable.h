#pragma once

namespace AVUI {

template <class T>
class Nullable
{
public:
    Nullable() : m_hasValue(false) { }
    Nullable(const T& value) : m_value(value), m_hasValue(true) { }

    operator const T&() const
    {
        if(!m_hasValue)
        {
            LibraryCriticalError();
        }
        return m_value;
    }

    bool operator==(const Nullable<T>& rhs) const 
    { 
        if(m_hasValue == false && rhs.m_hasValue == false) 
            return true; 

        if(m_hasValue == false || rhs.m_hasValue == false)
        {
            return false;
        }
        return m_value == rhs.m_value;
    }

    bool get_HasValue() const { return m_hasValue; }
    T& get_Value() { return m_value; }
    const T& get_Value() const { if(!get_HasValue()) LibraryCriticalError(); return m_value; }
    const T& get_ValueOrDefault() const { return m_value; }
private:
    bool m_hasValue;
    T m_value;
};

};
