#include <AVUICommon.h>
#include <AVUIWindowManager.h>
#include <AVUIDispatcher.h>
#include <AVUILayoutManager.h>
#include <AVUIGeometryBuilder.h>
#include <AVUIWindow.h>
#include <AVUIPopupRoot.h>
#include <AVUITransform.h>

namespace AVUI {

void WindowManager::AddPopupWindow(PopupRoot* pPopup)
{
    m_pPopupWindows.Insert(0, pPopup);
}

void WindowManager::RemovePopupWindow(PopupRoot* pPopup)
{
    m_pPopupWindows.Remove(pPopup);
}

void WindowManager::AddTopLevelWindow(Window* pWindow)
{
    AddToZOrderList(pWindow);
}

void WindowManager::RemoveTopLevelWindow(Window* pWindow)
{
    RemoveFromZOrderList(pWindow);
}

bool WindowManager::ActivateWindow(Window* pWindow)
{
    if(pWindow == NULL)
    {
        return false;
    }

    if(pWindow == m_pActiveWindow)
    {
        return true;
    }

    BringToTop(pWindow);

    SetActiveWindow(pWindow);

    return true;
}

void WindowManager::SetActiveWindow(Window* pWindow)
{
    if(m_pActiveWindow == pWindow)
    {
        return;
    }

    if(m_pActiveWindow != NULL)
    {
        m_pActiveWindow->OnWindowDeactivated();
    }

    m_pActiveWindow = pWindow;

    if(m_pActiveWindow != NULL)
    {
        m_pActiveWindow->OnWindowActivated();
    }
}


void WindowManager::DeactivateWindow(Window* pWindow)
{
    if(pWindow == NULL || pWindow != m_pActiveWindow)
    {
        return;
    }

    SetActiveWindow(NULL);

    Window* pWindowCur = m_pWindowZOrder;

    while(pWindowCur != NULL)
    {
        if(pWindowCur != pWindow && pWindowCur->get_IsShowing())
        {
            ActivateWindow(pWindowCur);
            break;
        }

        pWindowCur = pWindowCur->get_ZOrderNext();
    }
}


void WindowManager::BuildGeometryAndRenderDependencies(Renderer* pRenderer, GeometryBuilder* pBuilder)
{
    Window* pWindowCur = m_pWindowZOrder;

    while(pWindowCur != NULL)
    {
        Point windowLocation = pWindowCur->get_WindowLocation();

        pBuilder->set_GeometryScale(1.0f);
        pWindowCur->Precompute();
        pWindowCur->BuildGeometry(pBuilder);
        pWindowCur->RenderDependencies(pRenderer, 
                                       Matrix::CreateTranslation(windowLocation.get_X(), windowLocation.get_Y()),
                                       m_lastDesktopSize);

        pWindowCur = pWindowCur->get_ZOrderNext();
    }

    for(int idx = 0; idx < m_pPopupWindows.get_Count(); idx++)
    {
        PopupRoot* pPopupCur = m_pPopupWindows[idx];

        pBuilder->set_GeometryScale(1.0f);
        pPopupCur->Precompute();
        pPopupCur->BuildGeometry(pBuilder);
        pPopupCur->RenderDependencies(pRenderer, Matrix::get_Identity(), m_lastDesktopSize);
    }
}

void WindowManager::RenderWindows(Renderer* pRenderer)
{
    if(m_pWindowZOrder == NULL)
    {
        return;
    }

    Window* pWindowLast = m_pWindowZOrder;

    while(pWindowLast->get_ZOrderNext() != NULL)
    {
        pWindowLast = pWindowLast->get_ZOrderNext();
    }

    while(pWindowLast != NULL)
    {
        if(pWindowLast->get_IsShowing())
        {
            Point windowLocation = pWindowLast->get_WindowLocation();

            pWindowLast->Render(pRenderer, 
                                Matrix::CreateTranslation(windowLocation.get_X(), windowLocation.get_Y()),
                                Rect(m_lastDesktopSize));
        }

        pWindowLast = pWindowLast->get_ZOrderPrevious();
    }

    for(int idx = m_pPopupWindows.get_Count() - 1; idx >=0; idx--)
    {
        PopupRoot* pPopupCur = m_pPopupWindows[idx];

        if(pPopupCur->get_IsShowing())
        {
            pPopupCur->Render(pRenderer, Matrix::get_Identity(), Rect(m_lastDesktopSize));
        }
    }
}

void WindowManager::LayoutWindows(const Size& desktopSize)
{
    bool forceRemeasure = false; 
    bool remeasureOccurred = false; 

    if(m_lastDesktopSize != desktopSize)
    {
        m_lastDesktopSize = desktopSize;
        forceRemeasure = true;
    }

    Window* pWindowCur = m_pWindowZOrder;

    while(pWindowCur != NULL)
    {
        if(forceRemeasure || pWindowCur->get_NeverMeasured() || pWindowCur->get_NeverArranged())
        {
            pWindowCur->Measure(desktopSize);
            pWindowCur->Arrange(Rect(Point(), desktopSize));
            remeasureOccurred = true;
        }

        pWindowCur = pWindowCur->get_ZOrderNext();
    }

    for(int idx = 0; idx < m_pPopupWindows.get_Count(); idx++)
    {
        PopupRoot* pPopupCur = m_pPopupWindows[idx];

        if(forceRemeasure || pPopupCur->get_NeverMeasured() || pPopupCur->get_NeverArranged())
        {
            pPopupCur->Measure(desktopSize);
            pPopupCur->Arrange(Rect(Point(pPopupCur->get_PopupOffset().get_X(), pPopupCur->get_PopupOffset().get_Y()), pPopupCur->get_DesiredSize()));
            remeasureOccurred = true;
        }
    }


    if(remeasureOccurred)
    {
        Dispatcher::get_CurrentDispatcher()->get_LayoutManager()->FireLayoutUpdatedIfClean();        
    }
}

void WindowManager::AddToZOrderList(Window* pWindow)
{
    pWindow->m_pWindowPrevious = NULL;
    pWindow->m_pWindowNext = m_pWindowZOrder;

    if(m_pWindowZOrder != NULL)
    {
        m_pWindowZOrder->m_pWindowPrevious = pWindow;
    }

    m_pWindowZOrder = pWindow;
}

void WindowManager::RemoveFromZOrderList(Window* pWindow)
{
    if(pWindow == m_pWindowZOrder)
    {
        m_pWindowZOrder = m_pWindowZOrder->get_ZOrderNext();
        if(m_pWindowZOrder != NULL)
        {
            m_pWindowZOrder->m_pWindowPrevious = NULL;
        }
    }
    else
    {
        pWindow->m_pWindowPrevious->m_pWindowNext = pWindow->m_pWindowNext;

        if(pWindow->m_pWindowNext != NULL)
        {
            pWindow->m_pWindowNext->m_pWindowPrevious = pWindow->m_pWindowPrevious;
        }
    }

    pWindow->m_pWindowNext = NULL;
    pWindow->m_pWindowPrevious = NULL;
}

void WindowManager::BringToTop(Window* pWindow)
{
    if(pWindow == m_pWindowZOrder)
    {
        return;
    }

    RemoveFromZOrderList(pWindow);
    AddToZOrderList(pWindow);
}

Visual* WindowManager::ResolvePointToWindow(const Point& pt)
{
    for(int idx = 0; idx < m_pPopupWindows.get_Count(); idx++)
    {
        PopupRoot* pPopupCur = m_pPopupWindows[idx];
        Point localPt = ToWindowSpace(pPopupCur, pt);

        if(pPopupCur->HitTest(localPt))
        {
            return pPopupCur;
        }
    }

    Window* pWindowCur = m_pWindowZOrder;

    while(pWindowCur != NULL)
    {
        if(pWindowCur->get_IsShowing())
        {
            Point localPt = ToWindowSpace(pWindowCur, pt);

            if(pWindowCur->HitTest(localPt))
            {
                return pWindowCur;
            }
        }

        pWindowCur = pWindowCur->get_ZOrderNext();
    }

    return NULL;
}

void WindowManager::ReleaseRenderData()
{
    Window* pWindowCur = m_pWindowZOrder;

    while(pWindowCur != NULL)
    {
        pWindowCur->ReleaseRenderData();

        pWindowCur = pWindowCur->get_ZOrderNext();
    }

    for(int idx = 0; idx < m_pPopupWindows.get_Count(); idx++)
    {
        PopupRoot* pPopupCur = m_pPopupWindows[idx];

        pPopupCur->ReleaseRenderData();
    }
}

void WindowManager::OnPresentationSourceSizeChanged()
{
    Window* pWindowCur = m_pWindowZOrder;

    while(pWindowCur != NULL)
    {
        pWindowCur->OnPresentationSourceSizeChanged();
        pWindowCur = pWindowCur->get_ZOrderNext();
    }
}

Point WindowManager::ToWindowSpace(Visual* pWindow, const Point& absolutePoint)
{
    Point point = absolutePoint;

    if(pWindow != NULL && pWindow->IsType(GET_TYPE(Window)))
    {
        Point windowLocation = ((Window*)pWindow)->get_WindowLocation();
        point = Point(point.get_X() - windowLocation.get_X(), point.get_Y() - windowLocation.get_Y());        
    }

    point = point - pWindow->get_VisualOffset();

    Transform* pTransform = pWindow->get_VisualTransform();
    if (pTransform != NULL)
    {
        Matrix matrix = pTransform->get_Value();
        if (matrix.HasInverse())
        {
            matrix.Invert();
            point = matrix.Transform(point);
        }
    }

    return point;
}



}; // namespace AVUI


