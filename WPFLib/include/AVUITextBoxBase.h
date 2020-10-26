#pragma once

#include <AVUIControl.h>

namespace AVUI {

class TextBoxBase : public Control
{
public:

    DECLARE_ELEMENT(TextBoxBase, Control);

    IMPLEMENT_ROUTED_EVENT(TextChanged, RoutedEventArgs, RoutingStrategy::Bubble);
};


}; // namespace AVUI
