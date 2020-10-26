#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <wchar.h>
#include <exception>
#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUIDispatcher.h>
#include <AVUIInputManager.h>
#include <AVUIXMLParser.h>
#include <AVUIKeyInterop.h>
#include <AVUIXamlParser.h>
#include <AVUITextBlock.h>
#include <AVUISolidColorBrush.h>
#include <AVUIWindow.h>
#include <D3DUIRenderer.h>
#include <XInput.h>
#include <D3DInput.h>

#include <mmgr.h>

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

using namespace AVUI;


void SampleInit();
void SampleTick();
void SampleShutdown();

IDirect3D9* g_pD3D;
IDirect3DDevice9* g_pD3DDevice;
D3DVIEWPORT9 g_Viewport;
UIRenderer* g_pRenderer;
bool g_isDeviceLost = false;
bool g_isGamepadInputEnabled = false;
const WCHAR* g_wzCmdLine;

void CreateD3DDevice(HWND hwnd)
{
    D3DDISPLAYMODE d3ddm;
    g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm);

    D3DPRESENT_PARAMETERS d3dpp;

    memset(&d3dpp, 0, sizeof(d3dpp));

    d3dpp.hDeviceWindow = hwnd;
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    HRESULT hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, 
                         D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dpp, &g_pD3DDevice);
    if(FAILED(hr))
    {
        throw std::exception("Error initializing device.");
    }
}

void DestroyD3DDevice()
{
    g_pD3DDevice->Release();
    g_pD3DDevice = NULL;
}

void InitD3D(HWND hwnd)
{
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    CreateD3DDevice(hwnd);
}

ObjectPtr LoadXaml(const WCHAR* wzXamlFile)
{
    IClientNavResult* pResult = g_pRenderer->NavigateTo(wzXamlFile);

    ObjectPtr pNavResult = pResult->GetNavResult();

    pResult->Destroy();

    return pNavResult;
}               

void UpdateBackbufferSize(HWND hwnd)
{
    D3DPRESENT_PARAMETERS d3dpp;
    D3DDISPLAYMODE d3ddm;

    g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm);

    memset(&d3dpp, 0, sizeof(d3dpp));

    d3dpp.hDeviceWindow = hwnd;
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if(!g_isDeviceLost)
    {
        g_isDeviceLost = true;
        g_pRenderer->DeviceLost();
    }

    HRESULT hr = g_pD3DDevice->Reset(&d3dpp);

    if(!FAILED(hr))
    {
        DestroyD3DDevice();
        CreateD3DDevice(hwnd);

        g_pD3DDevice->GetViewport(&g_Viewport);
        Application::get_Current()->set_PresentationSourceSize(Size((float)g_Viewport.Width, (float)g_Viewport.Height));

        g_pRenderer->DeviceReset(g_pD3DDevice);
        g_isDeviceLost = false;
    }

}
void SceneRender(float timeSinceLastFrame)
{
    Application* pApplication = Application::get_Current();
    if(g_isDeviceLost)
    {
        UpdateBackbufferSize(GetActiveWindow());
        return;

    }

    try
    {
        pApplication->Update(timeSinceLastFrame);
    
        g_pD3DDevice->BeginScene();
    
        g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER,
                            0x0, 1.0f, 0);
    
        g_pRenderer->Render();
    
        g_pD3DDevice->EndScene();
        g_pD3DDevice->Present(NULL, NULL, NULL, NULL );
    }
    catch(std::exception& e)
    {
        TRefCountedPtr<TextBlock> pTextBlock = object_allocate<TextBlock>();

        pTextBlock->set_Text(object_allocate<String>(e.what()));
        pTextBlock->set_Foreground(object_allocate<SolidColorBrush>(KnownColor::White));
        pTextBlock->set_FontSize(30.0f);

        Application::get_Current()->get_MainWindow()->set_Content(pTextBlock);
    }
}

void InitCurrentDirectory()
{
    WCHAR wzModulePath[MAX_PATH];

    GetModuleFileNameW(NULL, wzModulePath, MAX_PATH);

    WCHAR* wch = wzModulePath + wcslen(wzModulePath);

    while(wch > wzModulePath)
    {
        if(*wch == L'\\')
        {
            *wch = L'\0';
            break;
        }
        wch--;
    }

    _wchdir(wzModulePath);
}

void* AllocateUIMemory(size_t size)
{   return malloc(size);
//    return m_allocator(__FILE__,__LINE__,__FUNCTION__,m_alloc_malloc, size);
}

void FreeUIMemory(void* p)
{
    free(p);
    //m_deallocator(__FILE__,__LINE__,__FUNCTION__,m_alloc_free, p);
}



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR szCmdLine, int nShowCmd)
{                                                       
    const int c_cmdLineMax = 240;
    WCHAR wzCmdLine[c_cmdLineMax];
    WNDCLASSEXW wcx;

    InitCurrentDirectory();

    CoInitialize(NULL);

    int cchCmdLine = strlen(szCmdLine);

    if(cchCmdLine >= c_cmdLineMax - 1) 
    {
        cchCmdLine = c_cmdLineMax - 1;
    }

    for(int idx = 0; idx < cchCmdLine; idx++)
    {
        wzCmdLine[idx] = szCmdLine[idx];
    }
    wzCmdLine[cchCmdLine] = L'\0';

    g_wzCmdLine = wzCmdLine;

    memset(&wcx, 0, sizeof(wcx));

    wcx.cbSize = sizeof(wcx);
    wcx.lpfnWndProc = MainWndProc;
    wcx.hInstance = GetModuleHandle(NULL);
    wcx.lpszClassName = L"AVUISample";

    RegisterClassExW(&wcx);

    HWND hwndMain = CreateWindowExW(WS_EX_APPWINDOW, L"AVUISample", L"AVUI Sample Application", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                                   CW_USEDEFAULT, 1280, 720, NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(hwndMain, nShowCmd);
    

    InitD3D(hwndMain);

    Application::RegisterMemoryAllocator(&AllocateUIMemory, &FreeUIMemory);

    Application::set_Current(object_allocate<Application>());

    g_pRenderer = new UIRenderer(g_pD3DDevice);

    Application::get_Current()->set_ClientRendererInterface(g_pRenderer);

    TRefCountedPtr<ResourceDictionary> pSystemResources = object_cast<ResourceDictionary>(LoadXaml(L"Media\\Xaml\\SystemResources.xaml"));

    Application::get_Current()->set_SystemResources(pSystemResources);

    pSystemResources = NULL;

    SampleInit();

    g_pD3DDevice->GetViewport(&g_Viewport);
    Application::get_Current()->set_PresentationSourceSize(Size((float)g_Viewport.Width, (float)g_Viewport.Height));

    MSG msg;
    ControllerXInputManager xinputManager;

    DWORD lastTime = timeGetTime();

    while(true)
    {
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);    

            if(msg.message == WM_SIZE)
            {
                break;
            }
        }
    
        if(msg.message == WM_QUIT)
        {
            SampleShutdown();

            Application::set_Current(NULL);
            delete g_pRenderer;

            g_pD3DDevice->Release();
            g_pD3D->Release();

            Application::CleanupStaticAllocations();

            m_dumpMemoryReport();

            return 0;
        }

        SampleTick();

        if(g_isGamepadInputEnabled)
        {
            xinputManager.Update();
        }

        DWORD currentTime = timeGetTime();

        SceneRender((currentTime - lastTime) * 1.0f / 1000.0f);

        lastTime = currentTime;
    }
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static Key::Enum lastKeyDown = Key::None;

    switch(uMsg)
    {
    case WM_COMMAND:
        return 0;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        {
            if(g_pD3DDevice != NULL)
            {
                UpdateBackbufferSize(hWnd);
                SceneRender(0.0f);
            }
            break;
        }
    case WM_CHAR:
        {
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageKeyChar(lastKeyDown, (::WCHAR)wParam);
            break;
        }
    case WM_KEYUP:
        {
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageKeyUp(KeyInterop::KeyFromVirtualKey(wParam));
            break;
        }
    case WM_KEYDOWN:
        {
            lastKeyDown = KeyInterop::KeyFromVirtualKey(wParam);
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageKeyDown(lastKeyDown);
            break;
        }
    case WM_SYSKEYDOWN:
        {
            lastKeyDown = KeyInterop::KeyFromVirtualKey(wParam);
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageKeyDown(lastKeyDown);
            break;
        }
    case WM_SYSKEYUP:
        {
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageKeyUp(KeyInterop::KeyFromVirtualKey(wParam));
            break;
        }
    case WM_LBUTTONDOWN:
        {
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageMouseButtonDown(MouseButton::Left);
            break;
        }
    case WM_LBUTTONUP:
        {
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageMouseButtonUp(MouseButton::Left);
            break;
        }
    case WM_RBUTTONDOWN:
        {
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageMouseButtonDown(MouseButton::Right);
            break;
        }
    case WM_RBUTTONUP:
        {
            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageMouseButtonUp(MouseButton::Right);
            break;
        }
    case WM_MOUSEMOVE:
        {
            int x=(short)LOWORD(lParam);
            int y=(short)HIWORD(lParam);

            Dispatcher::get_CurrentDispatcher()->get_InputManager()->MessageMousePosition(x, y, 0);
            break;
        }
    }


    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

