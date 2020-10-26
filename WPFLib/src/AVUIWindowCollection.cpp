#include <AVUICommon.h>
#include <AVUIWindowCollection.h>
#include <AVUIWindow.h>
#include <AVUIApplication.h>
#include <AVUIWindowManager.h>

namespace AVUI
{

void WindowCollection::Add(Window* pWindow)
{
    Application::get_Current()->get_WindowManager()->AddTopLevelWindow(pWindow);

    m_windows.AddItem(pWindow);
}

void WindowCollection::Remove(Window* pWindow)
{
    Application::get_Current()->get_WindowManager()->RemoveTopLevelWindow(pWindow);

    m_windows.Remove(pWindow);
}

}; // End namespace AVUI
