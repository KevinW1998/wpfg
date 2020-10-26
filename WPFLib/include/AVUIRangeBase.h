
#pragma once

#include <AVUIControl.h>

namespace AVUI {

class RangeBase : public Control
{
public:
    RangeBase() { };
    ~RangeBase() { };

    DECLARE_ELEMENT(RangeBase, Control);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Minimum, float, 0.0f, DependencyPropertyMetadata(PropertyFlag::None, OnMinimumChangedThunk));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Maximum, float, 1.0f, DependencyPropertyMetadata(PropertyFlag::None, OnMaximumChangedThunk));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Value, float,   0.0f, DependencyPropertyMetadata(PropertyFlag::None, OnValueChangedThunk));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(LargeChange, float,   1.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(SmallChange, float,   0.1f, PropertyFlag::None);

protected:

    virtual void OnMinimumChanged() { }
    virtual void OnMaximumChanged() { }
    virtual void OnValueChanged() { }

private:
    static void OnMinimumChangedThunk(Object* pDO, const DependencyPropertyChangedEventArgs& e) { ((RangeBase*) pDO)->OnMinimumChanged(); }
    static void OnMaximumChangedThunk(Object* pDO, const DependencyPropertyChangedEventArgs& e) { ((RangeBase*) pDO)->OnMaximumChanged(); }
    static void OnValueChangedThunk(Object* pDO, const DependencyPropertyChangedEventArgs& e) { ((RangeBase*) pDO)->OnValueChanged(); }


};


}; // namespace AVUI
