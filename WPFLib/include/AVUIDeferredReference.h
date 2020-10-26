#pragma once

namespace AVUI
{

class DeferredReference : public Object
{
public:
    DECLARE_ELEMENT(DeferredReference, Object);

    virtual ObjectPtr GetValue(BaseValueSource::Enum valueSource) = 0;
    virtual const Type& GetValueType() = 0;
};


}

