#pragma once

#include <AVUIAnimatable.h>
#include <AVUIAnimatedTypeHelpers.h>
#include <AVUIKeySpline.h>

namespace AVUI {

template <class T>
class TValueKeyFrame : public Animatable
{
public:

    T InterpolateValue(const T& baseValue, float keyFrameProgress)
    {
        if(keyFrameProgress < 0.0f || keyFrameProgress > 1.0f)
        {
            LibraryCriticalError();
        }

        return InterpolateValueCore(baseValue, keyFrameProgress);
    }

protected:

    virtual T InterpolateValueCore(const T& baseValue, float keyFrameProgress) = 0;

};


template<class T, class BaseClass>
class TValueLinearKeyFrame : public BaseClass
{

protected:
    virtual T InterpolateValueCore(const T& baseValue, float keyFrameProgress)
    {
        if(keyFrameProgress == 0.0f)
        {
            return baseValue;
        }

        const T& value = this->get_Value();

        if(keyFrameProgress == 1.0f)
        {
            return value;
        }

        return AnimatedTypeHelpers::Interpolate(baseValue, value, keyFrameProgress);
    }

};

template<class T, class BaseClass>
class TValueSplineKeyFrame : public BaseClass
{

protected:
    virtual KeySpline* get_KeySpline() = 0;

    virtual T InterpolateValueCore(const T& baseValue, float keyFrameProgress)
    {
        if(keyFrameProgress == 0.0f)
        {
            return baseValue;
        }

        const T& value = this->get_Value();

        if(keyFrameProgress == 1.0f)
        {
            return value;
        }

        float splineProgress = keyFrameProgress;

        KeySpline* pKeySpline = get_KeySpline();
        if(pKeySpline != NULL)
        {
            splineProgress = get_KeySpline()->GetSplineProgress(keyFrameProgress);

        }
        return AnimatedTypeHelpers::Interpolate(baseValue, value, splineProgress);
    }

};





template <class T>
class TReferenceKeyFrame : public Animatable
{
public:

    TRefCountedPtr<T> InterpolateValue(TRefCountedPtr<T> pBaseValue, float keyFrameProgress)
    {
        if(keyFrameProgress < 0.0f || keyFrameProgress > 1.0f)
        {
            LibraryCriticalError();
        }

        return InterpolateValueCore(pBaseValue, keyFrameProgress);
    }

protected:

    virtual TRefCountedPtr<T> InterpolateValueCore(T* pBaseValue, float keyFrameProgress) = 0;

};


template<class T, class BaseClass>
class TReferenceLinearKeyFrame : public BaseClass
{

protected:
    virtual TRefCountedPtr<T> InterpolateValueCore(T* pBaseValue, float keyFrameProgress)
    {
        if(keyFrameProgress == 0.0f)
        {
            return pBaseValue;
        }

        T* pValue = this->get_Value();

        if(keyFrameProgress == 1.0f)
        {
            return pValue;
        }

        return AnimatedReferenceTypeHelpers::Interpolate(pBaseValue, pValue, keyFrameProgress);
    }

};

template<class T, class BaseClass>
class TReferenceSplineKeyFrame : public BaseClass
{

protected:
    virtual KeySpline* get_KeySpline() = 0;

    virtual TRefCountedPtr<T> InterpolateValueCore(T* pBaseValue, float keyFrameProgress)
    {
        if(keyFrameProgress == 0.0f)
        {
            return pBaseValue;
        }

        T* pValue = this->get_Value();

        if(keyFrameProgress == 1.0f)
        {
            return pValue;
        }

        float splineProgress = keyFrameProgress;

        KeySpline* pKeySpline = get_KeySpline();
        if(pKeySpline != NULL)
        {
            splineProgress = get_KeySpline()->GetSplineProgress(keyFrameProgress);

        }
        return AnimatedReferenceTypeHelpers::Interpolate(pBaseValue, pValue, splineProgress);
    }
};







}; // namespace AVUI
