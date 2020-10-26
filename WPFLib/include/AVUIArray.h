#pragma once

namespace AVUI {

class Array : public Object
{
public:
    DECLARE_ELEMENT(Array, Object);

    Array() { };
};

template<class T>
class ArrayInstance : public Array
{
public:
    ArrayInstance(unsigned int capacity) : m_capacity(capacity)
    {
        m_rgValues = new T[capacity];
    }

    ~ArrayInstance()
    {
        delete[] m_rgValues;
    }

    T& operator[](int index)
    {
        if(index >= (int)m_capacity)
        {
            LibraryCriticalError();
        }
        return m_rgValues[index];
    }

    const T& operator[](int index) const
    {
        if(index >= (int)m_capacity)
        {
            LibraryCriticalError();
        }
        return m_rgValues[index];
    }

    int get_Length() const { return m_capacity; }

private:
    T* m_rgValues;
    unsigned int m_capacity;
};

};
