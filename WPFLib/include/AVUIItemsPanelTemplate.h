#pragma once

#include <AVUITemplate.h>

namespace AVUI {

class ItemsPanelTemplate : public FrameworkTemplate
{
public:
    ItemsPanelTemplate() { };

    DECLARE_ELEMENT(ItemsPanelTemplate, FrameworkTemplate);


protected:

    virtual void Seal() { FrameworkTemplate::Seal(); }

private:
};



}; // namespace AVUI
