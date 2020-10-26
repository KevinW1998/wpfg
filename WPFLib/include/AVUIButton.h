#pragma once

#include <AVUIButtonBase.h>

namespace AVUI {

class Button : public ButtonBase
{
public:
    Button() { };
    ~Button() { };

    DECLARE_ELEMENT(Button, ButtonBase);

};

}; // namespace AVUI
