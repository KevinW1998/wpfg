#pragma once

#include <AVUIRangeBase.h>

namespace AVUI {
class Track;
class RoutedEventArgs;
class DragDeltaEventArgs;
class Slider : public RangeBase
{
public:
    Slider() : m_pTrack(NULL) { };
    ~Slider() { };

    DECLARE_ELEMENT(Slider, RangeBase);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Orientation, Orientation, Orientation::Horizontal, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsDirectionReversed, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsSnapToTickEnabled, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(TickFrequency, float, 1.0f, PropertyFlag::None);

    IMPLEMENT_ROUTED_COMMAND(IncreaseLarge);
    IMPLEMENT_ROUTED_COMMAND(DecreaseLarge);
    IMPLEMENT_ROUTED_COMMAND(IncreaseSmall);
    IMPLEMENT_ROUTED_COMMAND(DecreaseSmall);
    IMPLEMENT_ROUTED_COMMAND(MinimizeValue);
    IMPLEMENT_ROUTED_COMMAND(MaximizeValue);

protected:
    virtual void OnApplyTemplate();

    virtual void OnIncreaseLarge();
    virtual void OnDecreaseLarge();
    virtual void OnIncreaseSmall();
    virtual void OnDecreaseSmall();
    virtual void OnMinimizeValue();
    virtual void OnMaximizeValue();
 
private:

    void MoveToNextTick(float direction);

    float SnapToTick(float value);
    void UpdateValue(float newValue);
    static void StaticConstructor();
    void ChangeValue(float newValue);

    void OnDragStarted(RoutedEventArgs* pArgs);
    void OnDragDelta(DragDeltaEventArgs* pArgs);
    void OnDragCompleted(RoutedEventArgs* pArgs);

    static void OnDragStartedThunk(Object* pSender, RoutedEventArgs* pArgs) { ((Slider*) pSender)->OnDragStarted(pArgs); }
    static void OnDragDeltaThunk(Object* pSender, DragDeltaEventArgs* pArgs) { ((Slider*) pSender)->OnDragDelta((DragDeltaEventArgs*)pArgs); }
    static void OnDragCompletedThunk(Object* pSender, RoutedEventArgs* pArgs) { ((Slider*) pSender)->OnDragCompleted(pArgs); }
    static void OnIncreaseLargeCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs);
    static void OnDecreaseLargeCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs);
    static void OnIncreaseSmallCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs);
    static void OnDecreaseSmallCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs);
    static void OnMinimizeValueCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs);
    static void OnMaximizeValueCommand(Object* pSender, ExecutedRoutedEventArgs* pArgs);

 

 


    Track* m_pTrack;
};


}; // namespace AVUI


