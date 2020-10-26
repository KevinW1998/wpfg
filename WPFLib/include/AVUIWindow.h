#pragma once

#include <AVUIContentControl.h>
#include <AVUIWindowCollection.h>

namespace AVUI {

class WindowFrame;

class Window : public ContentControl
{
    friend class WindowCollection;
    friend class WindowManager;
    friend class WindowFrame;
    friend class Mouse;
public:
    Window() : m_isShowingAsDialog(false), m_pWindowNext(NULL), m_pWindowPrevious(NULL) { Initialize(); };
    ~Window() { };

    DECLARE_ELEMENT(Window, ContentControl);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Title, String, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Topmost, bool, false, PropertyFlag::None);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(ResizeMode, ResizeMode, ResizeMode::CanResize, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, Window::OnResizeModeChanged));
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(WindowState, WindowState, WindowState::Normal, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, Window::OnWindowStateChanged));
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(WindowStyle, WindowStyle, WindowStyle::SingleBorderWindow, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, Window::OnWindowStyleChanged));

    WindowCollection* get_OwnedWindows() { return &m_ownedWindows; }

    Window* get_Owner() { return m_pOwnerWindow; }
    void set_Owner(Window* pNewOwner);    

    Window* get_ZOrderNext() { return m_pWindowNext; }
    Window* get_ZOrderPrevious() { return m_pWindowPrevious; }

    void Activate();
    void Show();
    void Hide();
    void ShowDialog();
    void EndDialog();

    bool get_IsShowing() { return get_Visibility() == Visibility::Visible; }
    bool get_IsShowingAsDialog() { return m_isShowingAsDialog; }

    unsigned int get_VisualChildCount();
    Visual* get_VisualChild(unsigned int idx);

    Point get_WindowLocation() const { return m_windowArea.get_Location(); }
    Size get_WindowSize() const { return m_windowArea.get_Size(); }

protected:

    virtual void OnWindowActivated();
    virtual void OnWindowDeactivated();

    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    WindowCollection m_ownedWindows;
    TWeakPtr<Window> m_pOwnerWindow;

    virtual bool GetLayoutClip(const Size& layoutSlotSize, Rect* pRect);

private:

    void OnPresentationSourceSizeChanged();

    static void OnResizeModeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnWindowStateChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnWindowStyleChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    void set_WindowLocation(const Point& newLocation) { m_windowArea.set_Location(newLocation); }
    void set_WindowSize(const Size& newSize) { m_windowArea.set_Size(newSize); InvalidateMeasure(); }

    void Initialize();
    bool m_isShowingAsDialog;

    // Sorted / Z order
    Window* m_pWindowNext;
    Window* m_pWindowPrevious;

    TRefCountedPtr<WindowFrame> m_pWindowFrame;
    Rect m_windowArea;
    Rect m_restoreArea;
};


}; // namespace AVUI
