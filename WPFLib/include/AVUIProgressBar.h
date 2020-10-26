
#pragma once

#include <AVUIRangeBase.h>

namespace AVUI {
class Track;
class RoutedEventArgs;
class ProgressBar : public RangeBase
{
public:
    ProgressBar() : m_pTrack(NULL), m_pIndicator(NULL) { };
    ~ProgressBar() { };

    DECLARE_ELEMENT(ProgressBar, RangeBase);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Orientation, Orientation, Orientation::Horizontal, PropertyFlag::AffectsMeasure);

protected:
    virtual void OnApplyTemplate();
    virtual void OnMinimumChanged() { SetProgressBarIndicatorLength(); }
    virtual void OnMaximumChanged() { SetProgressBarIndicatorLength(); }
    virtual void OnValueChanged()   { SetProgressBarIndicatorLength(); }

private:

    void OnTrackSizeChanged(Object* pObject, RoutedEventArgs* pEventArgs) { SetProgressBarIndicatorLength(); }
    void SetProgressBarIndicatorLength();


    UIElement* m_pTrack;
    UIElement* m_pIndicator;
};


}; // namespace AVUI
