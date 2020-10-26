#pragma once

#include <AVUIInline.h>

namespace AVUI {

class Storyboard;

class Run : public Inline
{
public:
    Run() { }

    DECLARE_ELEMENT(Run, Inline);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Text, String, NULL, PropertyFlag::IsContentProperty | PropertyFlag::AffectsMeasure);
};


}; // namespace AVUI
 
