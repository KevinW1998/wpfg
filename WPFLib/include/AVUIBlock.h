#pragma once

#include <AVUIDependencyObject.h>

namespace AVUI {

class Storyboard;

class Block : public DependencyObject
{
public:
    Block() { }

    DECLARE_ELEMENT(Block, DependencyObject);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(LineHeight, float, Float::get_NaN(), PropertyFlag::IsAttachedProperty);

private:
    static void StaticConstructor();

};


}; // namespace AVUI
 
