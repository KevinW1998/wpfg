#pragma once

#include <AVUISize.h>
#include <AVUIRect.h>
#include <AVUIWindowCollection.h>
#include <AVUIEventableObject.h>

namespace AVUI {

class IClientRendererInterface;
class Renderer;
class Dispatcher;
class GeometryBuilder;
class XamlParser;
class Object;
class UIElement;
class Type;
class TextBlock;
class ResourceDictionary;
class DirtyRectList;
class VisualBrush;
class Window;
class XamlParser;
class WindowManager;

typedef void (*PFNERRORFUNC)(const char* szError, const char* szFile, int line);
typedef void* (*PFNALLOCFUNC)(size_t size);
typedef void (*PFNFREEFUNC)(void* p);

class Application : public EventableObject
{
    friend class Visual;
    friend class VisualBrush;

public:
    Application();
    ~Application();

    DECLARE_ELEMENT(Application, Object);

    static void RegisterMemoryAllocator(PFNALLOCFUNC pfnAllocFunc, PFNFREEFUNC pfnFreeFunc)
    {
        ms_pfnAllocFunc = pfnAllocFunc;
        ms_pfnFreeFunc = pfnFreeFunc;
    }

    static void RegisterErrorHandler(PFNERRORFUNC pfnErrorHandler)
    {
        ms_pfnErrorHandler = pfnErrorHandler;
    }

    static PFNERRORFUNC GetErrorHandler() { return ms_pfnErrorHandler; }
    static PFNALLOCFUNC GetAllocFunc() { return ms_pfnAllocFunc; }
    static PFNFREEFUNC GetFreeFunc() { return ms_pfnFreeFunc; }

    static void CleanupStaticAllocations();

    void Startup();

    void set_ClientRendererInterface(IClientRendererInterface* pClientRendererInterface);

    void Render();
    void AddDirtyRect(const Rect& bboxInvalid);
    bool IsRectDirty(const Rect& bbox);

    void Update(float timeSinceLastFrame);

    Dispatcher* get_Dispatcher() { return m_pDispatcher; };
    WindowManager* get_WindowManager() { return m_pWindowManager; };
    IClientRendererInterface* get_ClientRendererInterface() { return m_pClientRendererInterface; }
    static Application* get_Current() { return ms_pCurrentApplication; }
    static void set_Current(Application* pApplication) { ms_pCurrentApplication = pApplication; }

    void set_PresentationSourceSize(const Size& size);
    Size get_PresentationSourceSize() { return m_presentationSourceSize; }

    ResourceDictionary* get_Resources() { return m_pResources; }
    void set_Resources(ResourceDictionary* pResources) { m_pResources = pResources; }

    ResourceDictionary* get_SystemResources() { return m_pSystemResources; }
    void set_SystemResources(ResourceDictionary* pSystemResources) { m_pSystemResources = pSystemResources; };

    Object* FindResource(const Object* pKey);
    
    void set_MainWindow(Window* pWindow);
    Window* get_MainWindow() { return m_pMainWindow; }

    Window* get_ActiveWindow();

    WindowCollection* get_Windows() { return &m_windows; }    

    static void RegisterType(const Type& type);

    void AddVisualBrush(VisualBrush* pVisualBrush);
    void RemoveVisualBrush(VisualBrush* pVisualBrush);

    void set_StartupUri(String* pStartupUri) { m_pStartupUri = pStartupUri; }
    String* get_StartupUri() { return m_pStartupUri; }

    IMPLEMENT_REFERENCE_PROPERTY(StartupUri, String, false);
    IMPLEMENT_REFERENCE_PROPERTY(Resources, ResourceDictionary, false);

    bool get_IsDirtyRegionSupported() { return m_isDirtyRegionSupported; }
    bool set_IsDirtyRegionSupported(bool isDirtyRegionSupported) { m_isDirtyRegionSupported = isDirtyRegionSupported; }

    void ReleaseRenderData();

    IMPLEMENT_EVENT(RenderDeviceLost, EventArgs);

private:

    bool IsVisualBrushRoot(Visual* pVisual, VisualBrush* pBrushIgnore);
    GeometryBuilder* get_GeometryBuilder() { return m_pGeometryBuilder; }

    static bool RegisterTypes();
    void InvalidateTreeMeasure(UIElement* pElement);

    static bool m_fRegisterTypesSucceeded;
    static PFNERRORFUNC ms_pfnErrorHandler;
    static PFNALLOCFUNC ms_pfnAllocFunc;
    static PFNFREEFUNC ms_pfnFreeFunc;


    IClientRendererInterface* m_pClientRendererInterface;
    Renderer* m_pRenderer;
    GeometryBuilder* m_pGeometryBuilder;
    Dispatcher* m_pDispatcher;
    WindowManager* m_pWindowManager;

    Size m_presentationSourceSize;

    TRefCountedPtr<ResourceDictionary> m_pResources;
    TRefCountedPtr<ResourceDictionary> m_pSystemResources;

    TRefCountedPtr<Window> m_pMainWindow;

    DirtyRectList* m_pInvalidRects;

    ItemStructList<VisualBrush*> m_visualBrushes;

    TRefCountedPtr<String> m_pStartupUri;

    bool m_isDirtyRegionSupported;

    static TRefCountedPtr<Application> ms_pCurrentApplication;

    WindowCollection m_windows;
};

}; // namespace AVUI
