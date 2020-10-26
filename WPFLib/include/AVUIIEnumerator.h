#pragma once

namespace AVUI
{

class IEnumerator
{
public:
    virtual bool MoveNext() = 0;
    virtual ObjectPtr get_Current() = 0;
    virtual void Reset() = 0;
    virtual bool IsInvalid() = 0;
};

};

