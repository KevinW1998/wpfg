#pragma once

#include <AVUIScrollContentPresenter.h>

class AnimatingScrollContentPresenter : public AVUI::ScrollContentPresenter
{
public:
    DECLARE_ELEMENT(AnimatingScrollContentPresenter, ScrollContentPresenter);

protected:
    AVUI::Size ArrangeOverride(const AVUI::Size& constraintSize);

    virtual bool GetLayoutClip(const AVUI::Size& layoutSlotSize, AVUI::Rect* pRect);

private:

    AVUI::Vector m_lastContentOffset;
};

