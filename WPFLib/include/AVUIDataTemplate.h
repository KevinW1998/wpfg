#pragma once

#include <AVUITemplate.h>

namespace AVUI {

class DataTemplate : public FrameworkTemplate
{
public:
    DataTemplate() { };

    DECLARE_ELEMENT(DataTemplate, FrameworkTemplate);


protected:

    virtual void Seal() { FrameworkTemplate::Seal(); };

private:
};



}; // namespace AVUI
