#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUILayoutManager.h>
#include <AVUIInputManager.h>
#include <AVUIVisual.h>
#include <AVUIGeometryBuilder.h>
#include <AVUIRenderer.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIXamlParser.h>
#include <AVUITypeManager.h>
#include <AVUIMatrix.h>
#include <AVUIVisualDrawingContext.h>
#include <AVUIUIElement.h>
#include <AVUIDispatcher.h>
#include <AVUITimeManager.h>
#include <AVUITextBlock.h>
#include <AVUIResourceDictionary.h>
#include <AVUIVisualBrush.h>
#include <AVUIWindow.h>
#include <AVUIDataBindEngine.h>
#include <AVUIWindowManager.h>
#include <AVUICommandManager.h>
#include <AVUIStyleHelper.h>
#include <AVUIRotation3D.h>

REGISTER_ELEMENT_BEGIN(AVUI::Application)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(StartupUri)
    REGISTER_PROPERTY(Resources)
REGISTER_ELEMENT_END()


namespace AVUI {

class DirtyRectList : public ItemStructList<Rect> { };

bool Application::m_fRegisterTypesSucceeded = false;
TRefCountedPtr<Application> Application::ms_pCurrentApplication = NULL;
PFNERRORFUNC Application::ms_pfnErrorHandler;
PFNALLOCFUNC Application::ms_pfnAllocFunc;
PFNFREEFUNC Application::ms_pfnFreeFunc;


Application::Application() : m_pClientRendererInterface(NULL), m_pResources(NULL), m_pMainWindow(NULL), m_isDirtyRegionSupported(false)
{
    ms_pCurrentApplication = this;

    m_pWindowManager = new WindowManager();
    m_pDispatcher = new Dispatcher();
    m_pRenderer = new Renderer();
    m_pGeometryBuilder = new GeometryBuilder();

    m_pDispatcher->get_TimeManager()->Start();

    m_pInvalidRects = new DirtyRectList();

    if(!m_fRegisterTypesSucceeded)
    {
        RegisterTypes();
        m_fRegisterTypesSucceeded = true;
    }
}

void Application::set_ClientRendererInterface(IClientRendererInterface* pClientRendererInterface) 
{ 
    m_pClientRendererInterface = pClientRendererInterface;
    m_pRenderer->set_ClientRendererInterface(pClientRendererInterface);
    m_pGeometryBuilder->set_ClientRendererInterface(pClientRendererInterface);
}

             
Application::~Application()
{
    m_windows.Clear();
    m_pMainWindow = NULL;

    delete m_pDispatcher;
    m_pDispatcher = NULL;

    delete m_pRenderer;
    m_pRenderer = NULL;

    delete m_pGeometryBuilder;
    m_pGeometryBuilder = NULL;

    delete m_pWindowManager;
    m_pWindowManager = NULL;

    delete m_pInvalidRects;
    m_pInvalidRects = NULL;
}

void Application::Render()
{
    static bool bFirst = true;

    m_pClientRendererInterface->BeginUIRender();
    m_pRenderer->set_CurrentOpacity(1.0f);

    if(m_isDirtyRegionSupported)
    {
        m_pClientRendererInterface->AddDirtyRects(m_pInvalidRects->get_Ptr(), m_pInvalidRects->get_Count());
    }

    for(int idx = 0; idx < m_visualBrushes.get_Count(); idx++)
    {
        Visual* pVisual = m_visualBrushes[idx]->get_Visual();

        if(pVisual != NULL)
        {
            m_pGeometryBuilder->set_GeometryScale(1.0f);
            pVisual->Precompute();
            pVisual->BuildGeometry(m_pGeometryBuilder);
        }
    }

    m_pWindowManager->BuildGeometryAndRenderDependencies(m_pRenderer, m_pGeometryBuilder);

    m_pClientRendererInterface->BeginMainSurfaceRender();

    m_pWindowManager->RenderWindows(m_pRenderer);

    m_pClientRendererInterface->EndMainSurfaceRender();


    m_pClientRendererInterface->EndUIRender();


    m_pRenderer->DestroySubgraphCaptures();

    m_pInvalidRects->Clear();

    for(int idx = 0; idx < m_visualBrushes.get_Count(); idx++)
    {
        m_visualBrushes[idx]->OnRenderFinished();
    }
}

void Application::RegisterType(const Type& type)
{
    TypeManager::RegisterType(const_cast<Type*>(&type));
}

#include <stdio.h>
#include <stdarg.h>

void GlobalError(const char* szFile, int line, const char* szMessage, ...)
{
   char szBuf[240];
   va_list vl;

   va_start(vl, szMessage);

   vsprintf_s(szBuf, sizeof(szBuf) / sizeof(szBuf)[0], szMessage, vl);

   va_end(vl);

   if(Application::GetErrorHandler() != NULL)
    {
        Application::GetErrorHandler()(szBuf, szFile, line);
    }
    else
    {
        int forceCrashUnhandledError = 0;
         int b = 1 / forceCrashUnhandledError;
    }
}

void Application::Update(float timeSinceLastFrame)
{
    DataBindEngine::get_CurrentDataBindEngine()->Run(false);

    m_pDispatcher->Update();

    m_pDispatcher->get_TimeManager()->Tick(timeSinceLastFrame);

    m_pWindowManager->LayoutWindows(m_presentationSourceSize);

    m_pDispatcher->get_LayoutManager()->Update();
    m_pDispatcher->get_InputManager()->Update();
}

void Application::set_PresentationSourceSize(const Size& newSize)
{
    if(!Float::AreClose(m_presentationSourceSize, newSize))
    {
        m_presentationSourceSize = newSize;

        AddDirtyRect(m_presentationSourceSize);

        m_pWindowManager->OnPresentationSourceSizeChanged();
    }
}


Object* Application::FindResource(const Object* pKey)
{
    Object* pValue = DependencyProperty::get_UnsetValue();

    if(m_pResources != NULL)
    {
        pValue = m_pResources->GetValue(pKey);
    }

    if(pValue != DependencyProperty::get_UnsetValue())
    {
        return pValue;
    }

    if(m_pSystemResources != NULL)
    {
        pValue = m_pSystemResources->GetValue(pKey);
    }

    return pValue;
}

void Application::set_MainWindow(Window* pWindow)
{
    if(m_pMainWindow == pWindow)
    {
        return;
    }

    if(m_pMainWindow != NULL)
    {
        m_pMainWindow = NULL;
    }

    m_pMainWindow = pWindow;
    
    m_pDispatcher->get_InputManager()->UpdateMouseOver();
}

void Application::AddDirtyRect(const Rect& bboxInvalid)
{
    if(m_pInvalidRects == NULL)
    {
        return;
    }

    Rect bboxInflated = bboxInvalid.Inflate(1.0f);

    int count = m_pInvalidRects->get_Count();
    for(int idx = 0; idx < count; idx++)
    {
        const Rect& bboxCur = (*m_pInvalidRects)[idx];

        if(bboxCur.Contains(bboxInflated))
        {
            return;
        }

        if(bboxCur.Intersects(bboxInflated))
        {
            Rect bboxSave = bboxCur;

            m_pInvalidRects->RemoveAt(idx);

            AddDirtyRect(Rect::Union(bboxSave, bboxInflated));
            
            return;
        }
    }

    m_pInvalidRects->AddItem(bboxInflated);
}

bool Application::IsRectDirty(const Rect& bbox)
{
    if(!m_isDirtyRegionSupported || m_pInvalidRects == NULL)
    {
        return true;
    }

    int count = m_pInvalidRects->get_Count();
    for(int idx = 0; idx < count; idx++)
    {
        const Rect& bboxCur = (*m_pInvalidRects)[idx];

        if(bboxCur.Intersects(bbox))
        {
            return true;
        }
    }

    return false;
}

void Application::AddVisualBrush(VisualBrush* pVisualBrush)
{
    m_visualBrushes.AddItem(pVisualBrush);
}

void Application::RemoveVisualBrush(VisualBrush* pVisualBrush)
{
    m_visualBrushes.Remove(pVisualBrush);
}

bool Application::IsVisualBrushRoot(Visual* pVisual, VisualBrush* pBrushIgnore)
{
    for(int idx = 0; idx < m_visualBrushes.get_Count(); idx++)
    {
        if(m_visualBrushes[idx] == pBrushIgnore)
        {
            continue;
        }

        if(m_visualBrushes[idx]->get_Visual() == pVisual)
        {
            return true;
        }
    }

    return false;
}


void Application::Startup()
{
    if(m_pResources != NULL)
    {
        m_pResources->Startup();
    }

    if(m_pStartupUri != NULL)
    {
        IClientNavResult* pNavResult = m_pClientRendererInterface->NavigateTo(m_pStartupUri->get_Ptr());

        if(pNavResult->GetNavStatus() == NavigationStatus::Navigated)
        {
            ObjectPtr pObjResult = pNavResult->GetNavResult();

            if(pObjResult->IsType(GET_TYPE(Window)))
            {
                m_pMainWindow = (Window*) (Object*) pObjResult;
            }
            else
            {
                if(m_pMainWindow == NULL)
                {
                    m_pMainWindow = object_allocate<Window>();
                    m_pMainWindow->set_Content(pObjResult);
                }
            }
        }
        pNavResult->Destroy();
    }
}

Window* Application::get_ActiveWindow()
{
    return m_pWindowManager->get_ActiveWindow();
}

void Application::ReleaseRenderData()
{
    EventArgs ea;
    invoke_RenderDeviceLost(this, &ea);

    for(int idx = 0; idx < m_visualBrushes.get_Count(); idx++)
    {
        Visual* pVisual = m_visualBrushes[idx]->get_Visual();

        if(pVisual != NULL)
        {
            pVisual->ReleaseRenderData();
        }
    }

    m_pWindowManager->ReleaseRenderData();
}


void Application::CleanupStaticAllocations()
{
    TypeManager::CleanupAllocatedData();
    CommandManager::CleanupStaticAllocations();
    EventManager::CleanupStaticAllocations();
    StyleHelper::CleanupStaticAllocations();
    Rotation3D::CleanupStaticAllocations();
}


}; // namespace AVUI
