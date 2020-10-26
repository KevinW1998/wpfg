
#pragma once

namespace AVUI
{

class IAddChild
{
public:
    virtual void AddChild(Object* pValue) = 0;
    virtual void AddText(String* pText) = 0;
};


};



