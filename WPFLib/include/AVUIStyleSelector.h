#pragma once

#include <AVUIObject.h>

namespace AVUI {

class StyleSelector : public Object
{
public:
    StyleSelector() { };

    DECLARE_ELEMENT(StyleSelector, Object);

    virtual TRefCountedPtr<Style> SelectStyle(Object* pItem, DependencyObject* pContainer) { return NULL; }
};



}; // namespace AVUI

