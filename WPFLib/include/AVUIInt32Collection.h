#pragma once

#include <AVUIValueTypeCollection.h>

namespace AVUI {

class Int32Collection : public ValueTypeCollection<int>
{
public:
    DECLARE_ELEMENT(Int32Collection, Object);
};


}; // namespace AVUI
