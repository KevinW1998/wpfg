
#pragma once
#include <AVUIFrameworkElement.h>

namespace AVUI {

class RepeatButton;
class Thumb;

class Track : public FrameworkElement
{
public:

    DECLARE_ELEMENT(Track, FrameworkElement);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Minimum, float, 0.0f, PropertyFlag::AffectsArrange);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Maximum, float, 1.0f, PropertyFlag::AffectsArrange);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Value, float,   0.0f, PropertyFlag::AffectsArrange);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ViewportSize, float,   Float::get_NaN(), PropertyFlag::AffectsArrange);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Density, float,   0.0f, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsDirectionReversed, bool,   false, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Orientation, Orientation, Orientation::Horizontal, PropertyFlag::AffectsMeasure);

    Track();
    virtual ~Track();

    float ValueFromDistance(float horizontal, float vertical);

    virtual unsigned int get_VisualChildCount() { return m_cVisualChildren; }
    virtual Visual* get_VisualChild(unsigned int idx);

    void set_DecreaseRepeatButton(RepeatButton* pDecreaseRepeatButton);
    RepeatButton* get_DecreaseRepeatButton() { return m_pDecreaseButton; }

    void set_IncreaseRepeatButton(RepeatButton* pIncreaseRepeatButton);
    RepeatButton* get_IncreaseRepeatButton() { return m_pIncreaseButton; }

    void set_Thumb(Thumb* pThumb);
    Thumb* get_Thumb() { return m_pThumb; }


    IMPLEMENT_REFERENCE_PROPERTY(IncreaseRepeatButton, RepeatButton, false);
    IMPLEMENT_REFERENCE_PROPERTY(DecreaseRepeatButton, RepeatButton, false);
    IMPLEMENT_REFERENCE_PROPERTY(Thumb, Thumb, false);

protected:

    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);

private:

    void RebuildVisualChildren();

    void InitVisualTree();
    void CoerceLength(float* pComponentLength, float trackLength);
    void ComputeSliderLengths(const Size& arrangeSize, bool isOrientationVertical, float* pDecreaseButtonLength, float* pThumbLength, float* pIncreaseButtonLength);
    bool ComputeScrollBarLengths(const Size& arrangeSize, float viewportSize, bool isOrientationVertical, float* pDecreaseButtonLength, float* pThumbLength, float* pIncreaseButtonLength);

    TRefCountedPtr<RepeatButton> m_pDecreaseButton;
    TRefCountedPtr<RepeatButton> m_pIncreaseButton;
    TRefCountedPtr<Thumb> m_pThumb;

    float m_density;
    float m_thumbCenterOffset;
    bool m_isVisualTreeInitted;
    int m_cVisualChildren;

    static const int mc_TrackVisualChildMax = 3;

    Visual* m_rgVisualChildren[mc_TrackVisualChildMax];
};


}; // namespace AVUI
