#include <AVUICommon.h>
#include <AVUIWindow.h>
#include <AVUIApplication.h>
#include <AVUIWindowManager.h>
#include <AVUIWindowFrame.h>

REGISTER_ELEMENT_BEGIN(AVUI::Window)

    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(Title)
    REGISTER_PROPERTY(Topmost)
REGISTER_ELEMENT_END()

namespace AVUI {

void Window::Initialize()
{
    set_BypassLayoutPolicies(true);

    m_windowArea = Rect(Point(), Size(640, 480));

    // This should be done via DP override
    set_Visibility(Visibility::Collapsed);

    Application::get_Current()->get_Windows()->Add(this);
    if (Application::get_Current()->get_MainWindow() == NULL)
    {
        Application::get_Current()->set_MainWindow(this);
    }

    set_IsAttachedToPresentationSource(true);

    m_pWindowFrame = object_allocate<WindowFrame>(this);

    AddVisualChild(m_pWindowFrame);

    set_ClipToBounds(true);

    SetTreeLevel(0);
}


void Window::set_Owner(Window* pNewOwner)
{
    if (m_isShowingAsDialog)
    {
        LibraryCriticalError();
    }

    if (m_pOwnerWindow != pNewOwner)
    {
        if (pNewOwner != NULL)
        {
            for (int i = 0; i < m_ownedWindows.get_Count(); i++)
            {
                if (m_ownedWindows[i] == pNewOwner)
                {
                    LibraryCriticalError();
                }
            }
        }
        if (m_pOwnerWindow != NULL)
        {
            m_pOwnerWindow->get_OwnedWindows()->Remove(this);
        }
        else
        {
            Application::get_Current()->get_Windows()->Remove(this);
        }

        m_pOwnerWindow = pNewOwner;

        if (m_pOwnerWindow != NULL)
        {
            m_pOwnerWindow->get_OwnedWindows()->Add(this);
        }
        else
        {
            Application::get_Current()->get_Windows()->Add(this);
        }
    }
}

void Window::Activate()
{
    Application::get_Current()->get_WindowManager()->ActivateWindow(this);
}

void Window::Show()
{
    set_Visibility(Visibility::Visible);

    Activate();
}

void Window::Hide()
{
    set_Visibility(Visibility::Hidden);

    Application::get_Current()->get_WindowManager()->DeactivateWindow(this);
}

void Window::ShowDialog()
{
    Show();
}

void Window::EndDialog()
{
    Hide();
}

void Window::OnWindowActivated()
{

}

void Window::OnWindowDeactivated()
{

}   

Size Window::MeasureOverride(const Size& constraintSize)
{
    Size windowSize = m_windowArea.get_Size();

    m_pWindowFrame->Measure(windowSize);

    if(get_VisualChildCount() > 1)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Measure(m_pWindowFrame->get_ContentArea(windowSize).get_Size());
        }
    }

    return constraintSize;
}

Size Window::ArrangeOverride(const Size& constraintSize)
{
    Size windowSize = m_windowArea.get_Size();

    m_pWindowFrame->Arrange(Rect(windowSize));

    if(get_VisualChildCount() > 1)
    {
        UIElement* pVisualChild = (UIElement*) get_VisualChild(0);

        if(pVisualChild != NULL)
        {
            pVisualChild->Arrange(m_pWindowFrame->get_ContentArea(windowSize));
        }
    }

    return windowSize;
}

unsigned int Window::get_VisualChildCount()
{
    return this->ContentControl::get_VisualChildCount() + 1;
}

Visual* Window::get_VisualChild(unsigned int idx)
{
    if(idx == ContentControl::get_VisualChildCount())
    {
        return m_pWindowFrame;
    }
    return ContentControl::get_VisualChild(idx);
}

void Window::OnResizeModeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    Window* pWindow = (Window*) pDO;

    if(pWindow->m_pWindowFrame != NULL)
    {
        pWindow->m_pWindowFrame->set_ResizeMode((ResizeMode::Enum)UnboxValue<int>(e.pNewValue));
    }
}

void Window::OnWindowStateChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    Window* pWindow = (Window*) pDO;

    if(pWindow->m_pWindowFrame != NULL)
    {
        WindowState::Enum oldWindowState = (WindowState::Enum)UnboxValue<int>(e.pOldValue);
        WindowState::Enum newWindowState = (WindowState::Enum)UnboxValue<int>(e.pNewValue);

        pWindow->m_pWindowFrame->set_WindowState(newWindowState);

        if(oldWindowState == WindowState::Normal)
        {
            pWindow->m_restoreArea = pWindow->m_windowArea;
        }

        if(newWindowState == WindowState::Normal)
        {
            pWindow->m_windowArea = pWindow->m_restoreArea;
        }
        else if(newWindowState == WindowState::Maximized)
        {
            pWindow->m_windowArea = Rect(Point(), Application::get_Current()->get_PresentationSourceSize());
        }
    }
}

void Window::OnWindowStyleChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    Window* pWindow = (Window*) pDO;

    if(pWindow->m_pWindowFrame != NULL)
    {
        pWindow->m_pWindowFrame->set_WindowStyle((WindowStyle::Enum)UnboxValue<int>(e.pNewValue));
    }
}

void Window::OnPresentationSourceSizeChanged()
{
    if(get_WindowState() == WindowState::Maximized)
    {
        m_windowArea = Rect(Point(), Application::get_Current()->get_PresentationSourceSize());
        InvalidateMeasure();
    }
}


bool Window::GetLayoutClip(const Size& layoutSlotSize, Rect* pRect)
{
    *pRect = Rect(m_windowArea.get_Size());
    return true;
}



}; // namespace AVUI

