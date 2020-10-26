#include <Windows.h>

#pragma warning(disable:4996)

#include <d3d9.h>
#include <dshow.h>
#include <vmr9.h>
#include <AVUIIClientRendererInterface.h>
#include <D3DUIVideo.h>
#include <D3DUIRenderer.h>

UIVideo::UIVideo(UIRenderer* pRenderer, const WCHAR* wzFileName) : m_pTexture(NULL), m_pRenderer(pRenderer), m_pSurfaces(NULL), m_pMediaSeeking(NULL), m_pBasicAudio(NULL)
{
    wcscpy_s(m_wzFileName, wzFileName);
    m_pRenderer->AddActiveVideo(this);
    m_volume = 0.5;
}                

UIVideo::~UIVideo()
{
    m_pRenderer->RemoveActiveVideo(this);
    Cleanup();
}


HRESULT __stdcall UIVideo::QueryInterface(REFIID riid, void **ppvObject)
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if( ppvObject == NULL ) {
        hr = E_POINTER;
    } 
    else if( riid == IID_IVMRSurfaceAllocator9 ) {
        *ppvObject = static_cast<IVMRSurfaceAllocator9*>( this );
        hr = S_OK;
    } 
    else if(riid == IID_IVMRImagePresenter9)
    {
        *ppvObject = static_cast<IVMRImagePresenter9*>( this );
        hr = S_OK;
    }
    else if( riid == IID_IUnknown ) {
        *ppvObject = 
            static_cast<IUnknown*>( 
            static_cast<IVMRSurfaceAllocator9*>( this ) );
        hr = S_OK;    
    }

    return hr;
}

unsigned long __stdcall UIVideo::AddRef()
{
    return 1;
}

unsigned long __stdcall UIVideo::Release()
{
    return 1;
}

HRESULT UIVideo::Initialize()
{
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IFilterGraph, (LPVOID*) &m_pGraph);

    if(FAILED(hr))
    {
        return hr;
    }

    hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID*) &m_pFilter);

    if(FAILED(hr))
    {
        return hr;
    }

    IVMRFilterConfig9* pFilterConfig;
    hr = m_pFilter->QueryInterface(IID_IVMRFilterConfig9, (void**) &pFilterConfig);
    if(FAILED(hr))
    {
        return hr;
    }

    hr = pFilterConfig->SetRenderingMode(VMR9Mode_Renderless);
    if(FAILED(hr))
    {
        return hr;
    }

    pFilterConfig->Release();
    pFilterConfig = NULL;

    SetAllocatorPresenter();

    m_pGraph->AddFilter(m_pFilter, L"VMR9");
    m_pGraph->QueryInterface(IID_IMediaControl, (void**) &m_pMediaControl);

    m_pGraph->QueryInterface(IID_IMediaSeeking, (void**) &m_pMediaSeeking);

    IGraphBuilder* pGB = NULL;
    m_pGraph->QueryInterface(IID_IGraphBuilder, (void**) &pGB);

    hr = pGB->RenderFile(m_wzFileName, NULL);

    m_pMediaControl->Run();

    pGB->QueryInterface(IID_IBasicAudio, (void**) &m_pBasicAudio);

    pGB->Release();
    pGB = NULL;

    return S_OK;
}

void UIVideo::Cleanup()
{
    if(m_pMediaControl != NULL)
    {
        m_pMediaControl->Stop();
        m_pMediaControl->Release();
        m_pMediaControl = NULL;
    }

    if(m_pMediaSeeking != NULL)
    {
        m_pMediaSeeking->Release();
        m_pMediaSeeking = NULL;
    }

    if(m_pBasicAudio != NULL)
    {
        m_pBasicAudio->Release();
        m_pBasicAudio = NULL;
    }

    if(m_pIVMRSurfAllocNotify != NULL)
    {
        m_pIVMRSurfAllocNotify->Release();
        m_pIVMRSurfAllocNotify = NULL;
    }

    if(m_pFilter != NULL)
    {
        m_pFilter->Release();
        m_pFilter = NULL;
    }

    if(m_pGraph != NULL)
    {
        m_pGraph->Release();
        m_pGraph = NULL;
    }

    ReleaseSurfaces();
}


HRESULT UIVideo::SetAllocatorPresenter()
{
    HRESULT hr;

    hr = m_pFilter->QueryInterface(IID_IVMRSurfaceAllocatorNotify9, (void**) &m_pIVMRSurfAllocNotify);

    hr = m_pIVMRSurfAllocNotify->AdviseSurfaceAllocator(0, this);

    IDirect3D9* pD3D;

    m_pRenderer->get_Device()->GetDirect3D(&pD3D);

    HMONITOR hMonitor = pD3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );
    m_pIVMRSurfAllocNotify->SetD3DDevice( m_pRenderer->get_Device(), hMonitor );

    pD3D->Release();
    pD3D = NULL;

    return hr;
}

// This is potentially called from any thread
HRESULT UIVideo::InitializeDevice(DWORD_PTR dwUserID, VMR9AllocationInfo *lpAllocInfo, DWORD *lpNumBuffers)
{
    ReleaseSurfaces();

    m_cSurfaces = *lpNumBuffers;

    m_pSurfaces = new IDirect3DSurface9*[m_cSurfaces];
    for(unsigned int idx = 0; idx < m_cSurfaces; idx++)
    {
        m_pSurfaces[idx] = NULL;
    }

    m_imageWidth = lpAllocInfo->dwWidth;
    m_imageHeight = lpAllocInfo->dwHeight;

    HRESULT hr = m_pIVMRSurfAllocNotify->AllocateSurfaceHelper(
                                        lpAllocInfo,
                                        lpNumBuffers,
                                        m_pSurfaces);

    if(FAILED(hr))
    {
        return hr;
    }

    m_pRenderer->get_Device()->CreateTexture(m_imageWidth, m_imageHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, 
                             &m_pTexture, NULL);

    m_cSurfaces = *lpNumBuffers;

    return hr;
}

void UIVideo::ReleaseSurfaces()
{
    if(m_pTexture != NULL)
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }

    if(m_pSurfaces != NULL)
    {
        for(unsigned int idx = 0; idx < m_cSurfaces; idx++)
        {
            if(m_pSurfaces[idx] != NULL)
            {
                m_pSurfaces[idx]->Release();
                m_pSurfaces[idx] = NULL;
            }
                
        }
        delete[] m_pSurfaces;
        m_pSurfaces = NULL;

        m_cSurfaces = 0;
    }
}


HRESULT UIVideo::TerminateDevice(DWORD_PTR dwID)
{
    ReleaseSurfaces();

    return S_OK;
}

HRESULT UIVideo::GetSurface(DWORD_PTR dwUserID, DWORD SurfaceIndex, DWORD SurfaceFlags, IDirect3DSurface9 **lplpSurface)
{
    m_pSurfaces[SurfaceIndex]->AddRef();
    *lplpSurface = m_pSurfaces[SurfaceIndex];

    return S_OK;
}
HRESULT UIVideo::AdviseNotify(IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify)
{

    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIVideo::StartPresenting(DWORD_PTR dwUserID)
{

    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIVideo::StopPresenting(DWORD_PTR dwUserID)
{

    return S_OK;
}

HRESULT STDMETHODCALLTYPE UIVideo::PresentImage(DWORD_PTR dwUserID, VMR9PresentationInfo *lpPresInfo)
{
    if(m_pTexture == NULL)
    {
        return NULL;
    }

    HRESULT hr = S_OK;

    IDirect3DSurface9* pSurface;

    hr = m_pTexture->GetSurfaceLevel(0, &pSurface);
    if(FAILED(hr))
    {
        return NULL;
    }

    hr = m_pRenderer->get_Device()->StretchRect(m_pSurfaces[0], NULL, pSurface, NULL,  D3DTEXF_NONE);
    if(FAILED(hr))
    {
        return NULL;
    }

    pSurface->Release();
    pSurface = NULL;

    return S_OK;
}

IDirect3DTexture9* UIVideo::GetTexture()
{
    return m_pTexture;
}

void UIVideo::Destroy()
{
    delete this;
}

void UIVideo::DeviceLost()
{
    if(m_pMediaSeeking != NULL)
    {
        m_pMediaSeeking->GetCurrentPosition(&m_savedPosition);
    }

    Cleanup();
}

void UIVideo::DeviceReset()
{
    Initialize();

    if(m_pMediaSeeking != NULL)
    {
        LONGLONG stopPosition;

        m_pMediaSeeking->GetStopPosition(&stopPosition);
        m_pMediaSeeking->SetPositions(&m_savedPosition, AM_SEEKING_AbsolutePositioning, &stopPosition, AM_SEEKING_AbsolutePositioning);
    }
    if(m_pBasicAudio != NULL)
    {
        set_Volume(m_volume);
    }
}

void UIVideo::set_Volume(float volume)
{
    m_volume = volume;

    if(m_pBasicAudio != NULL)
    {
        float dbVolume = 2000.0f * log10f(volume);

        dbVolume = max(dbVolume, -10000.0f);
        dbVolume = min(dbVolume, 0);

        m_pBasicAudio->put_Volume((long)dbVolume);
    }
}

