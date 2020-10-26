#pragma once

namespace AVUI {

class Property;
class Object;

class IXamlResolutionContext
{
public:
    virtual const Type* GetPropertyResolutionContext(Object* pObjectCurrent) = 0;
    virtual const Property* GetValueConverterContext(Object* pObjectCurrent) = 0;
};

};
