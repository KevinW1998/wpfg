#pragma once

#include <AVUIAnimationTimeline.h>

namespace AVUI {

template <class T>      
class TValueAnimationBase : public AnimationTimeline
{
public:

    virtual ObjectPtr GetCurrentValue(Object* pDefaultOriginValue, Object* pDefaultDestinationValue, AnimationClock* pAnimationClock)
    {
        if (pDefaultOriginValue == NULL || pDefaultDestinationValue == NULL)
        {
            LibraryCriticalError();
        }

        if (pDefaultOriginValue == NULL || pDefaultDestinationValue == NULL)
        {
            LibraryCriticalError();
        }

        if (pDefaultOriginValue->get_Type() != get_TargetPropertyType() || pDefaultDestinationValue->get_Type() != get_TargetPropertyType())
        {
            LibraryCriticalError();
        }


        T value = GetCurrentValue( ((BoxedValueTypeInstance<T> *) pDefaultOriginValue)->get_Value(), 
                                   ((BoxedValueTypeInstance<T> *) pDefaultDestinationValue)->get_Value(),
                                   pAnimationClock);

        return ObjectPtr(new BoxedValueTypeInstance<T>(value), false);
    }

    T GetCurrentValue(const T& defaultOriginValue, const T& defaultDestinationValue, AnimationClock* pAnimationClock)
    {
        return GetCurrentValueCore(defaultOriginValue, defaultDestinationValue, pAnimationClock);
    }

protected:

    virtual T GetCurrentValueCore(const T& defaultOriginValue, const T& defaultDestinationValue, AnimationClock* pAnimationClock) = 0;

private:
    const Type& get_TargetPropertyType() { return AVUI::get_Type( (T*) NULL); }
};



template <class T>      
class TReferenceAnimationBase : public AnimationTimeline
{
public:

    virtual ObjectPtr GetCurrentValue(Object* pDefaultOriginValue, Object* pDefaultDestinationValue, AnimationClock* pAnimationClock)
    {
        if (pDefaultOriginValue == NULL || pDefaultDestinationValue == NULL)
        {
            LibraryCriticalError();
        }

        if (pDefaultOriginValue == NULL || pDefaultDestinationValue == NULL)
        {
            LibraryCriticalError();
        }

        if (!pDefaultOriginValue->IsType(get_TargetPropertyType()) || !pDefaultDestinationValue->IsType(get_TargetPropertyType()))
        {
            LibraryCriticalError();
        }


        TRefCountedPtr<T> pValue = GetCurrentValue( (T*) pDefaultOriginValue, (T*) pDefaultDestinationValue, pAnimationClock);

        return (Object*)pValue;
    }

    TRefCountedPtr<T> GetCurrentValue(T* pDefaultOriginValue, T* pDefaultDestinationValue, AnimationClock* pAnimationClock)
    {
        return GetCurrentValueCore(pDefaultOriginValue, pDefaultDestinationValue, pAnimationClock);
    }

protected:

    virtual TRefCountedPtr<T> GetCurrentValueCore(T* pDefaultOriginValue, T* pDefaultDestinationValue, AnimationClock* pAnimationClock) = 0;

private:
    const Type& get_TargetPropertyType() { return AVUI::get_Type( (T*) NULL); }
};



}; // namespace AVUI
