#pragma once

#include <AVUIControl.h>

namespace AVUI {

class Separator : public Control
{
public:
    Separator() { };
    ~Separator() { };

    DECLARE_ELEMENT(Separator, Control);


    static void PrepareContainer(Control* pContainer);

private:

};


}; // namespace AVUI


