#pragma once

#include <AVUIIEnumerator.h>

namespace AVUI
{

class IEnumerable
{
public:
    virtual ObjectPtr GetEnumerator() = 0;
};

class EmptyEnumerator : public Object, public IEnumerator
{
public:

    DECLARE_ELEMENT(EmptyEnumerator, Object);

    virtual bool MoveNext() { return false; }
    virtual ObjectPtr get_Current() { LibraryCriticalError(); return NULL; }
    virtual void Reset() { }
    virtual bool IsInvalid() { return false; }

    static ObjectPtr get_Instance()
    {
        static EmptyEnumerator emptyEnumerator;

        return &emptyEnumerator;
    }

private:
    EmptyEnumerator() { }
};


class EmptyEnumerable : public Object, public IEnumerable
{
public:

    DECLARE_ELEMENT(EmptyEnumerable, Object);

    static ObjectPtr get_Instance()
    {
        static EmptyEnumerable emptyEnumerable;

        return &emptyEnumerable;
    }

    virtual ObjectPtr GetEnumerator()
    {
        return EmptyEnumerator::get_Instance();
    }

private:
    EmptyEnumerable() { }
};



};

