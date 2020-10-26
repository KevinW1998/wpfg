#pragma once

#include <AVUIType.h>

namespace AVUI {

class PropertyName : public Type
{
public:
    PropertyName(const char* szType) : Type(szType) { }
};


}; // namespace AVUI
