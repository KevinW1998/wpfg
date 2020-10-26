#pragma once

#include <AVUIRangeBase.h>

namespace AVUI {
class Track;
class RoutedEventArgs;
class DragDeltaEventArgs;
class DragStartedEventArgs;
class DragCompletedEventArgs;
class ScrollBar : public RangeBase
{
    friend class ScrollViewer;
public:
    ScrollBar() : m_pTrack(NULL), m_isStandalone(true) { };
    ~ScrollBar() { };

    DECLARE_ELEMENT(ScrollBar, RangeBase);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Orientation, Orientation, Orientation::Vertical, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ViewportSize, float, 1.0f, PropertyFlag::None);

    IMPLEMENT_ROUTED_COMMAND(LineUpCommand);
    IMPLEMENT_ROUTED_COMMAND(LineDownCommand);
    IMPLEMENT_ROUTED_COMMAND(LineLeftCommand);
    IMPLEMENT_ROUTED_COMMAND(LineRightCommand);
    IMPLEMENT_ROUTED_COMMAND(PageUpCommand);
    IMPLEMENT_ROUTED_COMMAND(PageDownCommand);
    IMPLEMENT_ROUTED_COMMAND(PageLeftCommand);
    IMPLEMENT_ROUTED_COMMAND(PageRightCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollToEndCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollToHomeCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollToRightEndCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollToLeftEndCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollToTopCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollToBottomCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollToHorizontalOffsetCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollToVerticalOffsetCommand);
    IMPLEMENT_ROUTED_COMMAND(ScrollHereCommand);

protected:
    virtual void OnApplyTemplate();

private:

    static void StaticConstructor();
    void set_IsStandalone(bool isStandalone) { m_isStandalone = isStandalone; }
    void ChangeValue(float newValue);

    void OnDragStarted(DragStartedEventArgs* pArgs);
    void OnDragDelta(DragDeltaEventArgs* pArgs);
    void OnDragCompleted(DragCompletedEventArgs* pArgs);

    static void OnDragStartedThunk(Object* pSender, DragStartedEventArgs* pArgs) { ((ScrollBar*) pSender)->OnDragStarted(pArgs); }
    static void OnDragDeltaThunk(Object* pSender, DragDeltaEventArgs* pArgs) { ((ScrollBar*) pSender)->OnDragDelta((DragDeltaEventArgs*)pArgs); }
    static void OnDragCompletedThunk(Object* pSender, DragCompletedEventArgs* pArgs) { ((ScrollBar*) pSender)->OnDragCompleted(pArgs); }

    Track* m_pTrack;

    bool m_isStandalone;
};


}; // namespace AVUI
