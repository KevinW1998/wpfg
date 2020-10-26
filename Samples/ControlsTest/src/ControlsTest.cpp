#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUIWindow.h>

#ifdef DEBUG
#include <mmgr.h>
#endif

extern const WCHAR* g_wzCmdLine;

using namespace AVUI;


ObjectPtr LoadXaml(const WCHAR* wzXamlFile);

TRefCountedPtr<Window> m_pMainWindow;
TRefCountedPtr<Window> m_pSecondWindow;

void SampleInit()
{
    m_pMainWindow = object_allocate<Window>();
    m_pSecondWindow = object_allocate<Window>();

    m_pMainWindow->set_Content(LoadXaml(L"Media\\Xaml\\FirstPage.xaml"));
    m_pSecondWindow->set_Content(LoadXaml(L"Media\\Xaml\\SecondPage.xaml"));

    m_pSecondWindow->Show();
    m_pMainWindow->Show();
}

void SampleTick()
{
}

void SampleShutdown()
{
    m_pMainWindow = NULL;
    m_pSecondWindow = NULL;
}

