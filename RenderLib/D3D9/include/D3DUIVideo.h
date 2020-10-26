#pragma once

struct IDirect3DTexture9;
struct IGraphBuilder;
struct IBaseFilter;
struct IMediaControl;
struct IVMRSurfaceAllocator9;
class UIRenderer;

class UIVideo : public IVMRSurfaceAllocator9, public IVMRImagePresenter9, public AVUI::IClientMediaElement
{
    friend class UIRenderer;
public:
    UIVideo(UIRenderer* pRenderer, const WCHAR* wzFileName);
    ~UIVideo();

    IDirect3DTexture9* GetTexture();

    virtual HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObject);

    virtual unsigned long __stdcall AddRef();
    virtual unsigned long __stdcall Release();

    virtual HRESULT __stdcall InitializeDevice(DWORD_PTR dwUserID, VMR9AllocationInfo *lpAllocInfo, DWORD *lpNumBuffers);
    virtual HRESULT __stdcall TerminateDevice(DWORD_PTR dwID);
    virtual HRESULT __stdcall GetSurface(DWORD_PTR dwUserID, DWORD SurfaceIndex, DWORD SurfaceFlags, IDirect3DSurface9 **lplpSurface);
    virtual HRESULT __stdcall AdviseNotify(IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify);

    virtual HRESULT STDMETHODCALLTYPE StartPresenting(DWORD_PTR dwUserID);
    virtual HRESULT STDMETHODCALLTYPE StopPresenting(DWORD_PTR dwUserID);
    virtual HRESULT STDMETHODCALLTYPE PresentImage(DWORD_PTR dwUserID, VMR9PresentationInfo *lpPresInfo);

    virtual void Destroy();

    void Cleanup();
    HRESULT Initialize();

    virtual void set_Volume(float volume);

private:

    void ReleaseSurfaces();
    void DeviceLost();
    void DeviceReset();

    HRESULT SetAllocatorPresenter();

    IVMRSurfaceAllocatorNotify9*    m_pIVMRSurfAllocNotify;
    IDirect3DSurface9**              m_pSurfaces;
    unsigned int                     m_cSurfaces;
    IBaseFilter*            m_pFilter;
    IDirect3DTexture9*      m_pTexture;
    IGraphBuilder*          m_pGraph;
    IMediaControl*          m_pMediaControl;
    IMediaSeeking*          m_pMediaSeeking;
    IBasicAudio*            m_pBasicAudio;

    LONGLONG m_savedPosition;

    UIRenderer*          m_pRenderer;
    unsigned int m_imageWidth;
    unsigned int m_imageHeight;

    float m_volume;

    CRITICAL_SECTION m_cs;

    WCHAR m_wzFileName[256];
};

