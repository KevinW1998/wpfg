#pragma once

namespace AVUI {

class IComparer
{
public:
    virtual int Compare(Object* pX, Object* pY) = 0;
};

class IComparable
{
public:
    virtual int CompareTo(Object* pObj) = 0;
};


};

