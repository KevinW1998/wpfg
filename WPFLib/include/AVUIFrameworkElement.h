#pragma once

#include <AVUIUIElement.h>

namespace AVUI {

class FrameworkElement : public UIElement
{
    friend class FrameworkObject;
public:
    DECLARE_ELEMENT(FrameworkElement, UIElement);

    FrameworkElement() { };
    ~FrameworkElement() { };
};
 

 



}; // namespace AVUI
