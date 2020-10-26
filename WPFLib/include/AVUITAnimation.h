#pragma once

#include <AVUITAnimationBase.h>
#include <AVUIClockGroup.h>
#include <AVUIAnimationClock.h>
#include <AVUIAnimatedTypeHelpers.h>

#include <memory.h>

namespace AVUI {


template<class T>
class TValueAnimation : public TValueAnimationBase<T>
{

protected:
    TValueAnimation() : m_isAnimationFunctionValid(false) { }

    virtual const bool& get_IsAdditive() = 0;
    virtual const bool& get_IsCumulative() = 0;
    virtual const Nullable<T>& get_From() = 0;
    virtual const Nullable<T>& get_To() = 0;
    virtual const Nullable<T>& get_By() = 0;


    virtual T GetCurrentValueCore(const T& defaultOriginValue, const T& defaultDestinationValue, AnimationClock* pAnimationClock)
    {
        if(!m_isAnimationFunctionValid)
        {
            ValidateAnimationFunction();
        }

        float progress = pAnimationClock->get_CurrentProgress().get_Value();

        T origin = AnimatedTypeHelpers::GetZeroValue<T>();
        T destination = AnimatedTypeHelpers::GetZeroValue<T>();
        T num5 = AnimatedTypeHelpers::GetZeroValue<T>();
        T num3 = AnimatedTypeHelpers::GetZeroValue<T>();


        switch (m_animationType)
        {
        case AnimationType::Automatic:
            origin = defaultOriginValue;
            destination = defaultDestinationValue;
            break;

        case AnimationType::From:
            origin = m_keyValues[0];
            destination = defaultDestinationValue;
            break;

        case AnimationType::To:
            origin = defaultOriginValue;
            destination = m_keyValues[0];
            break;

        case AnimationType::By:
            destination = m_keyValues[0];
            num3 = defaultOriginValue;
            break;

        case AnimationType::FromTo:
            origin = m_keyValues[0];
            destination = m_keyValues[1];

            if (get_IsAdditive())
            {
                num3 = defaultOriginValue;
            }
            break;

        case AnimationType::FromBy:
            origin = m_keyValues[0];
            destination = AnimatedTypeHelpers::Add(m_keyValues[0], m_keyValues[1]);

            if (get_IsAdditive())
            {
                num3 = defaultOriginValue;
            }
            break;
        }

        if (get_IsCumulative())
        {
            Nullable<int> nullable3 = pAnimationClock->get_CurrentIteration();
            Nullable<int> nullable2 = nullable3.get_HasValue() ? Nullable<int>(nullable3.get_ValueOrDefault() - 1) : Nullable<int>();
            float scale = (float) nullable2.get_Value();
            if (scale > 0)
            {
                T num6 = AnimatedTypeHelpers::Subtract(destination, origin);
                num5 = AnimatedTypeHelpers::Scale(num6, scale);
            }
        }
	        return AnimatedTypeHelpers::Add(num3, AnimatedTypeHelpers::Add(num5, AnimatedTypeHelpers::Interpolate(origin, destination, progress)));
    }

    void InvalidateAnimationFunction() { m_isAnimationFunctionValid = false; }

private:

    void ValidateAnimationFunction()
    {
        m_animationType = AnimationType::Automatic;

        const Nullable<T>& from = get_From();
        const Nullable<T>& to = get_To();
        const Nullable<T>& by = get_By();

      if (from.get_HasValue())
      {
            if (to.get_HasValue())
            {
                  m_animationType = AnimationType::FromTo;
                  m_keyValues[0] = from.get_Value();
                  m_keyValues[1] = to.get_Value();
            }
            else if (by.get_HasValue())
            {
                  m_animationType = AnimationType::FromBy;
                  m_keyValues[0] = from.get_Value();
                  m_keyValues[1] = by.get_Value();
            }
            else
            {
                  m_animationType = AnimationType::From;
                  m_keyValues[0] = from.get_Value(); 
            }
      }
      else if (to.get_HasValue())
      {
            m_animationType = AnimationType::To;
            m_keyValues[0] = to.get_Value();
      }
      else if (by.get_HasValue())
      {
            m_animationType = AnimationType::By;
            m_keyValues[0] = by.get_Value();
      }
      m_isAnimationFunctionValid = true;
    }

    AnimationType::Enum m_animationType;
    bool m_isAnimationFunctionValid;

    T m_keyValues[2];
};



template<class T>
class TReferenceAnimation : public TReferenceAnimationBase<T>
{

protected:
    TReferenceAnimation() : m_isAnimationFunctionValid(false) { }

    virtual const bool& get_IsAdditive() = 0;
    virtual const bool& get_IsCumulative() = 0;
    virtual T* get_From() = 0;
    virtual T* get_To() = 0;
    virtual T* get_By() = 0;


    virtual TRefCountedPtr<T> GetCurrentValueCore(T* pDefaultOriginValue, T* pDefaultDestinationValue, AnimationClock* pAnimationClock)
    {
        if(!m_isAnimationFunctionValid)
        {
            ValidateAnimationFunction();
        }

        float progress = pAnimationClock->get_CurrentProgress().get_Value();

        TRefCountedPtr<T> pOrigin;
        TRefCountedPtr<T> pDestination;
        TRefCountedPtr<T> pNum5;
        TRefCountedPtr<T> pNum3;


        switch (m_animationType)
        {
        case AnimationType::Automatic:
            pOrigin = pDefaultOriginValue;
            pDestination = pDefaultDestinationValue;
            pNum5 = NULL;
            pNum3 = NULL;
            break;

        case AnimationType::From:
            pOrigin = m_keyValues[0];
            pDestination = pDefaultDestinationValue;
            pNum5 = NULL;
            pNum3 = NULL;
            break;

        case AnimationType::To:
            pOrigin = pDefaultOriginValue;
            pDestination = m_keyValues[0];
            pNum5 = NULL;
            pNum3 = NULL;
            break;

        case AnimationType::By:
            pDestination = m_keyValues[0];
            pNum3 = pDefaultOriginValue;
            pNum5 = NULL;;
            pOrigin = NULL;
            break;

        case AnimationType::FromTo:
            pOrigin = m_keyValues[0];
            pDestination = m_keyValues[1];
            pNum5 = NULL;

            if (get_IsAdditive())
            {
                pNum3 = pDefaultOriginValue;
            }
            else
            {
                pNum3 = NULL;
            }
            break;

        case AnimationType::FromBy:
            pOrigin = m_keyValues[0];
            pDestination = AnimatedReferenceTypeHelpers::Add<T>(m_keyValues[0], m_keyValues[1]);
            pNum5 = NULL;

            if (get_IsAdditive())
            {
                pNum3 = pDefaultOriginValue;
            }
            else
            {
                pNum3 = NULL;
            }
            break;
        }

        if (get_IsCumulative())
        {
            Nullable<int> nullable3 = pAnimationClock->get_CurrentIteration();
            Nullable<int> nullable2 = nullable3.get_HasValue() ? Nullable<int>(nullable3.get_ValueOrDefault() - 1) : Nullable<int>();
            float scale = (float) nullable2.get_Value();
            if (scale > 0)
            {
                TRefCountedPtr<T> pNum6 = AnimatedReferenceTypeHelpers::Subtract<T>(pDestination, pOrigin);
                pNum5 = AnimatedReferenceTypeHelpers::Scale<T>(pNum6, scale);
            }
        }
        return AnimatedReferenceTypeHelpers::Add<T>(pNum3, AnimatedReferenceTypeHelpers::Add<T>(pNum5, AnimatedReferenceTypeHelpers::Interpolate<T>(pOrigin, pDestination, progress)));
    }

    void InvalidateAnimationFunction() { m_isAnimationFunctionValid = false; }

private:

    void ValidateAnimationFunction()
    {
        m_animationType = AnimationType::Automatic;

        TRefCountedPtr<T> pFrom = get_From();
        TRefCountedPtr<T> pTo = get_To();
        TRefCountedPtr<T> pBy = get_By();

      if (pFrom != NULL)
      {
            if (pTo != NULL)
            {
                  m_animationType = AnimationType::FromTo;
                  m_keyValues[0] = pFrom;
                  m_keyValues[1] = pTo;
            }
            else if (pBy != NULL)
            {
                  m_animationType = AnimationType::FromBy;
                  m_keyValues[0] = pFrom;
                  m_keyValues[1] = pBy;
            }
            else
            {
                  m_animationType = AnimationType::From;
                  m_keyValues[0] = pFrom; 
            }
      }
      else if (pTo != NULL)
      {
            m_animationType = AnimationType::To;
            m_keyValues[0] = pTo;
      }
      else if (pBy != NULL)
      {
            m_animationType = AnimationType::By;
            m_keyValues[0] = pBy;
      }
      m_isAnimationFunctionValid = true;
    }

    AnimationType::Enum m_animationType;
    bool m_isAnimationFunctionValid;

    TRefCountedPtr<T> m_keyValues[2];
};



}; // namespace AVUI
