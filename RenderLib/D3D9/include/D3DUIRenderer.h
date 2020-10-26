
#pragma once

namespace AVUI
{
    class UISystem;
    class Rect;
    class Matrix;
    class Matrix3D;
}

#include <AVUIIClientRendererInterface.h>
#include <D3DX9.h>

class ClientBrush;
class ClientFont;
class UIVideo;
class FontManager;
class TextureManager;

class UIRenderer : public AVUI::IClientRendererInterface
{
    friend class ClientRenderGeometry;
    friend class UIVideo;
    friend class ClientSubgraphCapture;
public:
	UIRenderer(IDirect3DDevice9* pDevice);
	virtual ~UIRenderer(void);

    void set_ClearMainSurfaceOnRender(bool clearMainSurfaceOnRender) { m_clearMainSurface = clearMainSurfaceOnRender; }
    bool get_ClearMainSurfaceOnRender() { return m_clearMainSurface; }

    void Render(void);

    virtual void BeginUIRender();

    virtual void BeginMainSurfaceRender();
    virtual void AddDirtyRects(const AVUI::Rect* pRects, unsigned int cRects);
    virtual void EndMainSurfaceRender();

    virtual void EndUIRender();

    virtual void BeginSubgraphCapture(const AVUI::Rect& rectVisibleSubgraph);
    virtual AVUI::IClientSubgraphCapture* EndSubgraphCapture();

    virtual void Begin3DRender();
    virtual void SetCurrent3DTransform(const AVUI::Matrix3D& wvpMatrix);
    virtual void End3DRender();

    virtual void OpacityMask(const AVUI::Rect& rectSubgraph, AVUI::IClientSubgraphCapture* pCapture);

    virtual AVUI::IClientRenderGeometry* CreateGeometry();
    virtual AVUI::IClientTextGeometry* CreateTextGeometry();

    virtual void RenderGeometry(AVUI::IClientRenderGeometry* pGeometry, bool isStrokeGeometry);
    virtual void RenderGeometry3D(AVUI::IClientRenderGeometry* pGeometry);
    virtual void RenderTextGeometry(AVUI::IClientTextGeometry* pText);
    virtual void RenderMediaElement(AVUI::IClientMediaElement* pMediaElement, const AVUI::Rect& rectArea);

    virtual AVUI::IClientFont* CreateClientFont(const WCHAR* wzFontName, float pixelSize);

    virtual void set_CurrentBrush(const AVUI::SolidColorBrush* pBrush);
    virtual void set_CurrentBrush(const AVUI::LinearGradientBrush* pBrush, AVUI::IClientLinearGradientBrush* pClientBrush);
    virtual void set_CurrentBrush(const AVUI::RadialGradientBrush* pBrush, AVUI::IClientRadialGradientBrush* pClientBrush);
    virtual void set_CurrentBrush(const AVUI::ImageBrush* pBrush, AVUI::IClientImageSource* pClientSource);
    virtual void set_CurrentBrush(const AVUI::VisualBrush* pBrush, AVUI::IClientSubgraphCapture* pClientBrush);

    virtual void set_Opacity(float opacity);
    virtual void PushClipRect(const AVUI::Rect& rect);
    virtual void PopClipRect();

    virtual void SetCurrentTransform(const AVUI::Matrix& mtx);
    virtual void PlaySound(const WCHAR* wzSoundName);
    virtual void set_Cursor(unsigned int cursorType);

    virtual AVUI::IClientLinearGradientBrush* CreateLinearGradientBrush();
    virtual AVUI::IClientRadialGradientBrush* CreateRadialGradientBrush();
    virtual AVUI::IClientImageSource* CreateImageSource(const WCHAR*);
    virtual AVUI::IClientMediaElement* CreateMediaElement(const WCHAR* wzSource);

    virtual AVUI::IClientNavResult* NavigateTo(const WCHAR* wzNavURI);

    void DeviceLost();
    void DeviceReset(IDirect3DDevice9* pDevice);

    IDirect3DDevice9* get_Device() { return m_pDevice; }
    void RemoveActiveBrush(ClientBrush* pBrush);


    static int NextPow2(int value, int maxValue);

private:

    void CleanupOldRenderTargets();
    void ClearCachedRenderInterfaces();
    void CleanupDeviceData();
    void InitDeviceData();

    void ReleaseVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer);
    void ReleaseIndexBuffer(IDirect3DIndexBuffer9* pIndexBuffer);
    IDirect3DVertexBuffer9* CreateVertexBuffer(int cVerts); 
    IDirect3DIndexBuffer9* CreateIndexBuffer(int cIndices); 

    void InitRenderStates();

    void PushRenderContext();
    void PopRenderContext();
    D3DXMATRIX CreateViewMatrix(float width, float height);
    void DrawRect(const AVUI::Rect& rect, bool isOpacityMask);

    void AddActiveVideo(UIVideo* pVideo);
    void RemoveActiveVideo(UIVideo* pVideo);

    IDirect3DTexture9* GetRenderTarget(unsigned int width, unsigned int height);
    void FreeRenderTarget(IDirect3DTexture9* pRenderTarget);

    struct GeometryVertex 
    {
        float x, y, z;			//!< The position for the vertex.
        float tu1, tv1;			//!< texture coordinates
        float tu2, tv2;			//!< texture coordinates
    };

    struct CachedRenderTarget 
    {
        unsigned int width;
        unsigned int height;
        IDirect3DTexture9* pRenderTargetTexture;
        unsigned int lastFrameUsed;
    };

    struct CachedIndexBuffer
    {
        IDirect3DIndexBuffer9* pIndexBuffer;
        int cIndices;
    };

    struct CachedVertexBuffer
    {
        IDirect3DVertexBuffer9* pVertexBuffer;
        int cVerts;
    };

    IDirect3DDevice9* m_pDevice;
    IDirect3DVertexDeclaration9* m_pVertexDecl;
    IDirect3DTexture9* m_pAATexture;
    ClientBrush* m_pCurrentBrush;

    AVUI::ItemStructList<CachedIndexBuffer> m_cachedIndexBuffers;
    AVUI::ItemStructList<CachedVertexBuffer> m_cachedVertexBuffers;
    AVUI::ItemStructList<ClientBrush*> m_activeBrushes;
    AVUI::ItemStructList<UIVideo*> m_activeVideos;
    AVUI::ItemStructList<CachedRenderTarget> m_cachedRenderTargets;
    AVUI::ItemStructList<GeometryVertex> m_clientVertices;
    AVUI::ItemStructList<unsigned short> m_clientIndices;
    AVUI::ItemStructList<AVUI::Rect> m_clipStack;

    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matView;

    float m_opacityCurrent;

    FontManager* m_pFontManager;
    TextureManager* m_pTextureManager;

    AVUI::Rect m_mainSurfaceRect;

    IDirect3DSurface9* m_pMainRenderTarget;

    IDirect3DTexture9* m_pRenderTargetTexture;
    AVUI::Rect m_visibleSubgraph;

    bool m_clearMainSurface;
    unsigned int m_frameCount;
};


class ClientRenderGeometry : public AVUI::IClientRenderGeometry
{
    friend class UIRenderer;
public
:
    ClientRenderGeometry(UIRenderer* pUIRenderer);
    virtual ~ClientRenderGeometry();
    virtual void Destroy() { delete this; }

    virtual void AddVertex(float xPosition, float yPosition, bool isAAGeometry);
    void AddVertexUV(float xPosition, float yPosition, float u, float v);
    virtual void AddVertex3D(float xPosition, float yPosition, float zPosition, float textureU, float textureV);
    virtual void AddIndex(unsigned int index);

    virtual void AddIndexTriplet(unsigned int index1, unsigned int index2, unsigned int index3);
    virtual void BeginFigure();
    virtual void EndFigure();

    void AllocateBuffers();

    virtual void Seal();

    unsigned int GetVertexCount() const { return m_cVertices; }

    bool get_IsEmpty() { return m_isEmpty; }

    float get_Width() { return m_maxX - m_minX; }
    float get_Height() { return m_maxY - m_minY; }

private:

    IDirect3DVertexBuffer9* m_pVertexBuffer;
    IDirect3DIndexBuffer9* m_pIndexBuffer;
    float m_minX, m_minY, m_maxX, m_maxY;
    UIRenderer* m_pUIRenderer;
    int m_cVertices;
    int m_cIndices;
    bool m_isSealed;
    bool m_isEmpty;
    bool m_noTexCoordGen;
};


class ClientSubgraphCapture : public AVUI::IClientSubgraphCapture
{
public:
    ClientSubgraphCapture(UIRenderer* pRenderer, IDirect3DTexture9* pSubgraphTexture, const AVUI::Rect& rectVisible) : 
        m_pRenderer(pRenderer),
        m_rect(rectVisible), 
        m_pSubgraphTexture(pSubgraphTexture)
    {
        
    }
    virtual ~ClientSubgraphCapture()
    {
        m_pRenderer->FreeRenderTarget(m_pSubgraphTexture);
    }

    virtual const AVUI::Rect& get_CapturedRect() { return m_rect; };
    IDirect3DTexture9* get_Texture() { return m_pSubgraphTexture; };

    virtual void Destroy() { delete this; }
private:

    AVUI::Rect m_rect;
    IDirect3DTexture9* m_pSubgraphTexture;
    UIRenderer* m_pRenderer;

};

