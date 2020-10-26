#pragma once

#include <AVUIDependencyObject.h>

namespace AVUI {

class ContentElement : public DependencyObject
{
friend class FrameworkObject;

public:

    DECLARE_ELEMENT(ContentElement, DependencyObject);

protected:

    virtual DependencyObject* GetUIParentCore() { return NULL; }

private:

};


}; // namespace AVUI
