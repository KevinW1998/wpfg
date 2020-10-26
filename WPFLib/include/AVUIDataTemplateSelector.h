#pragma once

#include <AVUIObject.h>

namespace AVUI {

class DataTemplateSelector : public Object
{
public:
    DataTemplateSelector() { };

    DECLARE_ELEMENT(DataTemplateSelector, Object);

    virtual TRefCountedPtr<DataTemplate> SelectTemplate(Object* pItem, DependencyObject* pContainer) { return NULL; }
};



}; // namespace AVUI

