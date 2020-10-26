#pragma once

#include <AVUIFrameworkElement.h>

namespace AVUI {

class Decorator;
class Popup;

class PopupRoot : public FrameworkElement
{
    friend class Popup;
public:

    DECLARE_ELEMENT(PopupRoot, FrameworkElement);

    PopupRoot() : m_isWindowDisposed(false), m_isWindowShowing(false), m_isWindowRegistered(false), m_isRegisteredForLayoutUpdated(false)
    { 
        Initialize();
    }
    ~PopupRoot(); 

    void set_Child(UIElement* pChild);
    UIElement* get_Child();

    bool IsWindowAlive();

    void ShowWindow();
    void HideWindow();

    virtual unsigned int get_VisualChildCount() { return m_pTransformDecorator != NULL ? 1 :  0; }
    virtual Visual* get_VisualChild(unsigned int idx);

    bool get_IsShowing() { return m_isWindowShowing; }
    Vector get_PopupOffset() { return m_popupOffset; }

protected:

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    virtual bool GetLayoutClip(const Size& layoutSlotSize, Rect* pRect);

private:
    void Initialize();

    void set_Transform(Transform* pTransform);
    Matrix GetTransformToDevice();
    Matrix GetTransformToDevice(Visual* pVisual);
    void SetupLayoutBindings(Popup* pPopup); 
    void BuildWindow(int x, int y, Visual* pPlacementTarget, bool transparent);
    void DestroyWindow();
    void SetPopupPos(bool position, int x, int y, bool size, int width, int height);

    void StopAnimations();
    void SetupFadeAnimation(const Duration& duration, bool isVisible);
    void SetupTranslateAnimations(PopupAnimation::Enum, const Duration& duration, bool animateFromRight, bool animateFromBottom);
    Vector get_AnimationOffset();
    void OnLayoutUpdated(Object* pSender, EventArgs* pArgs);
    void ClearLayoutUpdatedHandler();
    void EnsureLayoutUpdatedHandler();

    TRefCountedPtr<Decorator> m_pTransformDecorator;

    bool m_isWindowRegistered : 1;
    bool m_isWindowDisposed : 1;
    bool m_isWindowShowing : 1;
    bool m_isRegisteredForLayoutUpdated : 1;
    Vector m_popupOffset;
};



}; // namespace AVUI

