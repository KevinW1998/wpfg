#include <windows.h>
#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUIWindow.h>
#include <AVUITextBlock.h>

#ifdef DEBUG
#include <mmgr.h>
#endif

extern const WCHAR* g_wzCmdLine;

using namespace AVUI;


ObjectPtr LoadXaml(const WCHAR* wzXamlFile);

FILETIME g_lastWriteTime;

FILETIME GetLastFileWriteTime(const WCHAR* wzFile)
{
    WIN32_FIND_DATAW findData;

    HANDLE hFile = FindFirstFileW(wzFile, &findData);

    FindClose(hFile);

    return findData.ftLastWriteTime;
}

void LoadContent(const WCHAR* wzXamlFile)
{
    ObjectPtr pContent;
    try
    {
        pContent = LoadXaml(g_wzCmdLine);
    }
    catch(std::exception e)
    {
        TRefCountedPtr<TextBlock> pErrorText = object_allocate<TextBlock>();

        pErrorText->set_Text(object_allocate<String>(e.what()));
        pErrorText->set_Foreground(object_allocate<SolidColorBrush>(KnownColor::White));
        pErrorText->set_FontSize(30.0f);

        pContent = pErrorText;
    }

    Application::get_Current()->get_MainWindow()->set_Content(pContent);
}

void UIError(const char* szError, const char* szFile, int line)
{
    throw std::exception(szError);
}


void SampleInit()
{
    Application::RegisterErrorHandler(&UIError);
    Application::get_Current()->set_MainWindow(object_allocate<Window>());

    if(g_wzCmdLine == NULL || g_wzCmdLine[0] == L'\0')
    {
        g_wzCmdLine = L"Media\\Xaml\\SamplePage.xaml";
    }

    LoadContent(g_wzCmdLine);

    Application::get_Current()->get_MainWindow()->set_WindowState(WindowState::Maximized);
    Application::get_Current()->get_MainWindow()->Show();

    g_lastWriteTime = GetLastFileWriteTime(g_wzCmdLine);
}

void SampleTick()
{
    DWORD currentTime = timeGetTime();
    static DWORD dwFileTime = timeGetTime();

    if(currentTime - dwFileTime > 1000)
    {
        FILETIME writeTime = GetLastFileWriteTime(g_wzCmdLine);
        if(memcmp(&writeTime, &g_lastWriteTime, sizeof(FILETIME)) != 0)
        {                                                   
            g_lastWriteTime = writeTime;

            LoadContent(g_wzCmdLine);
        }

        dwFileTime = currentTime;           
    }
}

void SampleShutdown()
{

}

