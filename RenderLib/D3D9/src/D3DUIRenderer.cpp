#include <Windows.h>
#pragma warning(disable:4996)
#include <initguid.h>
#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUIXamlParser.h>
#include <AVUIMatrix.h>
#include <AVUIXMLParser.h>
#include <D3DUIRenderer.h>
#include <D3DUIBrushes.h>
#include <D3DUIFontManager.h>
#include <D3DUITextureManager.h>
#include <D3DUIFormattedText.h>

#include <dshow.h>
#include <vmr9.h>
#include <D3DUIVideo.h>
#include <D3DX9.h>
#include <wchar.h>

using namespace AVUI;



UIRenderer::UIRenderer(IDirect3DDevice9* pDevice) : m_pDevice(pDevice)
{
    m_clearMainSurface = true;

    m_pFontManager = new FontManager(this, L"Media\\Fonts");

    m_pTextureManager = new TextureManager(this, L"Media\\Images");

    InitDeviceData();

    m_pCurrentBrush = NULL;

    m_frameCount = 0;
}

void UIRenderer::InitDeviceData()
{
    D3DVERTEXELEMENT9 VertexElements[4] =
    {
        { 0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        D3DDECL_END()
    };

    m_pDevice->CreateVertexDeclaration( VertexElements, &m_pVertexDecl );

    D3DLOCKED_RECT lockedRect;
    IDirect3DSurface9* pSurface = NULL;

    m_pDevice->CreateTexture( 256, 1, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pAATexture, NULL);
    m_pAATexture->GetSurfaceLevel(0, &pSurface);

    pSurface->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD);

    unsigned int* pData = (unsigned int*) lockedRect.pBits;

    for(unsigned int idx = 0; idx < 256; idx++)
    {
        pData[idx] = D3DCOLOR_ARGB(idx, 255, 255, 255);
    }

    pSurface->UnlockRect();
    pSurface->Release();
    pSurface = NULL;

    ClientBrush::StaticInitBrushes(m_pDevice);
}

void UIRenderer::CleanupDeviceData()
{
    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, NULL);

    m_pVertexDecl->Release();
    m_pVertexDecl = NULL;

    m_pAATexture->Release();
    m_pAATexture = NULL;

    ClientBrush::StaticCleanupBrushes(m_pDevice);
}


UIRenderer::~UIRenderer(void)
{
    ClearCachedRenderInterfaces();
    CleanupDeviceData();

    delete m_pTextureManager;
    m_pTextureManager = NULL;

    delete m_pFontManager;
    m_pFontManager = NULL;
}

void UIRenderer::ClearCachedRenderInterfaces()
{
    for(int idx = 0; idx < m_cachedRenderTargets.get_Count(); idx++)
    {
        m_cachedRenderTargets[idx].pRenderTargetTexture->Release();
    }
    m_cachedRenderTargets.Clear();

    for(int i = 0; i < m_cachedIndexBuffers.get_Count(); i++)
    {
        m_cachedIndexBuffers[i].pIndexBuffer->Release();
        m_cachedIndexBuffers[i].pIndexBuffer = NULL;
    }
    m_cachedIndexBuffers.Clear();

    for(int i = 0; i < m_cachedVertexBuffers.get_Count(); i++)
    {
        m_cachedVertexBuffers[i].pVertexBuffer->Release();
        m_cachedVertexBuffers[i].pVertexBuffer = NULL;
    }
    m_cachedVertexBuffers.Clear();
}

void UIRenderer::CleanupOldRenderTargets()
{
    if(m_frameCount == 0)
    {
        return;
    }

    for(int idx = 0; idx < m_cachedRenderTargets.get_Count(); idx++)
    {
        if(m_cachedRenderTargets[idx].lastFrameUsed < (m_frameCount - 1))
        {
            m_cachedRenderTargets[idx].pRenderTargetTexture->Release();
            m_cachedRenderTargets.RemoveAt(idx);
            idx--;
        }
    }
}


D3DXMATRIX UIRenderer::CreateViewMatrix(float width, float height)
{
    D3DXMATRIX matrix;

    D3DXMatrixScaling(&matrix, 2.0f / width,                                 
                      -2.0f / height, 1.0f / 10000.0f);

    matrix._41 = -1.0f;
    matrix._42 = 1.0f;

    return matrix;
}                                                             


void UIRenderer::InitRenderStates()
{
    m_pDevice->SetVertexDeclaration( m_pVertexDecl );

    m_pDevice->SetRenderState( D3DRS_BLENDFACTOR, D3DCOLOR_ARGB(0x00,0x00,0x00,0x00) );
    m_pDevice->SetRenderState( D3DRS_LASTPIXEL, FALSE );

    m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pDevice->SetRenderState( D3DRS_SRCBLEND,         D3DBLEND_ONE );
    m_pDevice->SetRenderState( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );
    m_pDevice->SetRenderState( D3DRS_BLENDOP,          D3DBLENDOP_ADD );

    m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    m_pDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
    m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    m_pDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, D3DCOLOR_RGBA(0x00, 0x00, 0x00, 0x00));

    m_pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    m_pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

    m_pDevice->SetTexture(0, NULL);
    m_pDevice->SetTexture(1, m_pAATexture);
}


void UIRenderer::Render(void)
{
    InitRenderStates();

    Application::get_Current()->Render();

    m_pDevice->SetStreamSource(0, NULL, 0, 0);
    m_pDevice->SetIndices(NULL);

    CleanupOldRenderTargets();

    m_frameCount++;
}

void UIRenderer::PushRenderContext()
{
}

void UIRenderer::PopRenderContext()
{

}


IClientRenderGeometry* UIRenderer::CreateGeometry()
{ 
    return new ClientRenderGeometry(this);
}

void UIRenderer::BeginUIRender()
{
    m_mainSurfaceRect = Rect(Point(), Application::get_Current()->get_PresentationSourceSize());
    m_pDevice->GetRenderTarget(0, &m_pMainRenderTarget);
}

void UIRenderer::EndUIRender()
{
    if(m_pMainRenderTarget != NULL)
    {
        m_pMainRenderTarget->Release();
        m_pMainRenderTarget = NULL;
    }
    m_pCurrentBrush = NULL;
}

void UIRenderer::BeginMainSurfaceRender()
{
    m_clipStack.Clear();
    m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    m_matView = CreateViewMatrix(Application::get_Current()->get_PresentationSourceSize().get_Width(), 
                                 Application::get_Current()->get_PresentationSourceSize().get_Height());

    m_pDevice->SetRenderTarget(0, m_pMainRenderTarget);

    DWORD flags = D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL;

    if(m_clearMainSurface)
    {
        flags |= D3DCLEAR_TARGET;
    }

    m_pDevice->Clear( 0L, NULL, flags, 
                         D3DCOLOR_ARGB(0xFF, 0,0,0), 1.0f, 0L );
}

void UIRenderer::EndMainSurfaceRender()
{
}

// Big issue - Scissors rect is in pixel coordinates, so is incorrect when pixels / virtual pixels do not match.
void UIRenderer::BeginSubgraphCapture(const AVUI::Rect& rectVisibleSubgraph)
{
    m_clipStack.Clear();
    m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    IDirect3DTexture9* pSubgraphTexture = NULL;
    IDirect3DSurface9* pRenderTargetSurface = NULL;

    unsigned int textureWidth = (unsigned int)rectVisibleSubgraph.get_Width();
    unsigned int textureHeight = (unsigned int)rectVisibleSubgraph.get_Height();

    if(textureWidth > m_mainSurfaceRect.get_Width())
    {
        textureWidth = (unsigned int) m_mainSurfaceRect.get_Width();
    }

    if(textureHeight > m_mainSurfaceRect.get_Height())
    {
        textureHeight = (unsigned int) m_mainSurfaceRect.get_Height();
    }

    pSubgraphTexture = GetRenderTarget(textureWidth, textureHeight);

    pSubgraphTexture->GetSurfaceLevel(0, &pRenderTargetSurface);

    m_pDevice->SetRenderTarget(0, pRenderTargetSurface);

    pRenderTargetSurface->Release();

    m_pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
                          D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );

    m_matView = CreateViewMatrix(rectVisibleSubgraph.get_Width(), rectVisibleSubgraph.get_Height());

    m_pRenderTargetTexture = pSubgraphTexture;
    m_visibleSubgraph = rectVisibleSubgraph;

}

AVUI::IClientSubgraphCapture* UIRenderer::EndSubgraphCapture()
{
    return new ClientSubgraphCapture(this, m_pRenderTargetTexture, m_visibleSubgraph);
}

void UIRenderer::DrawRect(const Rect& rect, bool isOpacityMask)
{
    GeometryVertex verts[4];

    verts[0].x = 0.0f; 
    verts[0].y = 0.0f; 
    verts[0].z = 1.0f; 
    verts[0].tu1 = 0.0f;
    verts[0].tv1 = 0.0f;
    verts[0].tu2 = 0.0f;
    verts[0].tv2 = 0.0f;

    verts[1].x = rect.get_Width(); 
    verts[1].y = 0.0f; 
    verts[1].z = 1.0f; 
    verts[1].tu1 = 1.0f;
    verts[1].tv1 = 0.0f;
    verts[1].tu2 = 1.0f;
    verts[1].tv2 = 0.0f;

    verts[2].x = rect.get_Width(); 
    verts[2].y = rect.get_Height(); 
    verts[2].z = 1.0f; 
    verts[2].tu1 = 1.0f;
    verts[2].tv1 = 1.0f;
    verts[2].tu2 = 1.0f;
    verts[2].tv2 = 1.0f;

    verts[3].x = 0.0f; 
    verts[3].y = rect.get_Height(); 
    verts[3].z = 1.0f; 
    verts[3].tu1 = 0.0f;
    verts[3].tv1 = 1.0f;
    verts[3].tu2 = 0.0f;
    verts[3].tv2 = 1.0f;

    if(!isOpacityMask)
    {
        for(unsigned int idx = 0; idx < 4; idx++)
        {
            verts[idx].tu2 = 1.0f;
            verts[idx].tv2 = 1.0f;
        }
    }

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &verts, sizeof(GeometryVertex));
}

void UIRenderer::OpacityMask(const Rect& rectSubgraph, IClientSubgraphCapture* pClientCapture)
{
    ClientSubgraphCapture* pCapture = (ClientSubgraphCapture*) pClientCapture;

    m_pDevice->SetTexture(1, pCapture->get_Texture());

    if(m_pCurrentBrush != NULL)
    {
        m_pCurrentBrush->SetGeometryRenderStates(m_pDevice, rectSubgraph);
    }

    DrawRect(rectSubgraph, true);

    m_pDevice->SetTexture(1, m_pAATexture);
}

class ClientTextGeometry : public IClientTextGeometry
{
public:
    ClientTextGeometry(UIRenderer* pRenderer) : m_clientGeometry(pRenderer) { }
    virtual void Destroy() { delete this; }

    virtual float get_Width() { return m_clientGeometry.get_Width(); }
    virtual bool get_IsEmpty() { return m_clientGeometry.get_IsEmpty(); }
    virtual void Seal() 
    { 
        m_clientGeometry.Seal();
    }

    virtual void AddFormattedText(IClientFormattedText* pText, float xPosition, float yPosition)
    {
        ClientFormattedText* pFormattedText = (ClientFormattedText*) pText;

        m_pTexture = pFormattedText->GetFontTexture();

        pFormattedText->AddToGeometryData(&m_clientGeometry, xPosition, yPosition);            
    }

    ClientRenderGeometry* GetClientRenderGeometry() { return &m_clientGeometry; }
    IDirect3DTexture9* GetFontTexture() { return m_pTexture; }

private:
    ClientRenderGeometry m_clientGeometry;
    IDirect3DTexture9* m_pTexture;
};

IClientTextGeometry* UIRenderer::CreateTextGeometry()
{ 
    return new ClientTextGeometry(this);
}


IClientFont* UIRenderer::CreateClientFont(const WCHAR* wzFontName, float pixelSize)
{
    return m_pFontManager->FindMatchingFont(wzFontName, pixelSize);
}

void UIRenderer::set_Opacity(float opacity)
{
    m_opacityCurrent = opacity;
}

void UIRenderer::PushClipRect(const AVUI::Rect& clipRect)
{
    if(m_clipStack.get_Count() == 0)
    {
        m_clipStack.AddItem(clipRect);
    }
    else
    {
        AVUI::Rect rectIntersection;

        if(Rect::Intersect(m_clipStack[m_clipStack.get_Count() - 1], clipRect, &rectIntersection))
        {
            m_clipStack.AddItem(rectIntersection);
        }
        else
        {
            m_clipStack.AddItem(Rect());
        }

    }


    AVUI::Rect fullClipRect = m_clipStack[m_clipStack.get_Count() - 1];

    RECT rect;

    rect.top = (int)fullClipRect.get_Top();
    rect.left = (int)fullClipRect.get_Left();
    rect.right = (int)fullClipRect.get_Right();
    rect.bottom = (int)fullClipRect.get_Bottom();

    m_pDevice->SetScissorRect(&rect);
    m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
}


void UIRenderer::PopClipRect()
{
    m_clipStack.RemoveAt(m_clipStack.get_Count() - 1);

    if(m_clipStack.get_Count() > 0)
    {
        m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

        AVUI::Rect clipRect = m_clipStack[m_clipStack.get_Count() - 1];
        RECT rect;

        rect.top = (int)clipRect.get_Top();
        rect.left = (int)clipRect.get_Left();
        rect.right = (int)clipRect.get_Right();
        rect.bottom = (int)clipRect.get_Bottom();

        m_pDevice->SetScissorRect(&rect);
    }
    else
    {
        m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    }
}
void UIRenderer::SetCurrentTransform(const Matrix& mtx)
{
    m_matWorld = D3DXMATRIX(mtx.GetM11(), mtx.GetM12(), 0.0f, 0.0f,
                            mtx.GetM21(), mtx.GetM22(), 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            mtx.get_OffsetX(), mtx.get_OffsetY(), 0.0f, 1.0f);

    D3DXMATRIX matFinal = m_matWorld * m_matView;

    m_pDevice->SetVertexShaderConstantF( 0, (FLOAT*)&matFinal, 4 );
}

void UIRenderer::PlaySound(const WCHAR* wzSoundName)
{
}

void UIRenderer::set_Cursor(unsigned int cursorType)
{    
    switch(cursorType)
    {
    case CursorType::None:
        ShowCursor(FALSE);
        break;

    case CursorType::No:
        SetCursor(LoadCursor(0, IDC_NO));
        break;

    case CursorType::Arrow:
        SetCursor(LoadCursor(0, IDC_ARROW));
        break;

    case CursorType::AppStarting:
        SetCursor(LoadCursor(0, IDC_APPSTARTING));
        break;

    case CursorType::Cross:
        SetCursor(LoadCursor(0, IDC_CROSS));
        break;

    case CursorType::Help:
        SetCursor(LoadCursor(0, IDC_HELP));
        break;

    case CursorType::IBeam:
        SetCursor(LoadCursor(0, IDC_IBEAM));
        break;

    case CursorType::SizeAll:
        SetCursor(LoadCursor(0, IDC_SIZEALL));
        break;

    case CursorType::SizeNESW:
        SetCursor(LoadCursor(0, IDC_SIZENESW));
        break;

    case CursorType::SizeNS:
        SetCursor(LoadCursor(0, IDC_SIZENS));
        break;

    case CursorType::SizeNWSE:
        SetCursor(LoadCursor(0, IDC_SIZENWSE));
        break;

    case CursorType::SizeWE:
        SetCursor(LoadCursor(0, IDC_SIZEWE));
        break;

    case CursorType::UpArrow:
        SetCursor(LoadCursor(0, IDC_UPARROW));
        break;

    case CursorType::Wait:
        SetCursor(LoadCursor(0, IDC_WAIT));
        break;

    case CursorType::Hand:
        SetCursor(LoadCursor(0, IDC_HAND));
        break;

        /*
    case CursorType::Pen:
        SetCursor(LoadCursor(0, IDC_PEN));
        break;

    case CursorType::ScrollNS:
        SetCursor(LoadCursor(0, IDC_SCROLLNS));
        break;

    case CursorType::ScrollWE:
        SetCursor(LoadCursor(0, IDC_SCROLLWE));
        break;

    case CursorType::ScrollAll:
        SetCursor(LoadCursor(0, IDC_SCROLLALL));
        break;

    case CursorType::ScrollN:
        SetCursor(LoadCursor(0, IDC_SCROLLN));
        break;

    case CursorType::ScrollS:
        SetCursor(LoadCursor(0, IDC_SCROLLS));
        break;

    case CursorType::ScrollW:
        SetCursor(LoadCursor(0, IDC_SCROLLW));
        break;

    case CursorType::ScrollE:
        SetCursor(LoadCursor(0, IDC_SCROLLE));
        break;

    case CursorType::ScrollNW:
        SetCursor(LoadCursor(0, IDC_SCROLLNW));
        break;

    case CursorType::ScrollNE:
        SetCursor(LoadCursor(0, IDC_SCROLLNE));
        break;

    case CursorType::ScrollSW:
        SetCursor(LoadCursor(0, IDC_SCROLLSW));
        break;

    case CursorType::ScrollSE:
        SetCursor(LoadCursor(0, IDC_SCROLLSE));
        break;

    case CursorType::ArrowCD:
        SetCursor(LoadCursor(0, IDC_ARROWCD));
        break;
        */
        
    }
}

IClientImageSource* UIRenderer::CreateImageSource(const WCHAR* wzName)
{
    return m_pTextureManager->LoadImageSource(wzName);
}

IClientLinearGradientBrush* UIRenderer::CreateLinearGradientBrush()
{
    ClientLinearGradientBrush* pLinearGradientBrush = new ClientLinearGradientBrush(this);

    m_activeBrushes.AddItem(pLinearGradientBrush);

    return pLinearGradientBrush;
}

IClientRadialGradientBrush* UIRenderer::CreateRadialGradientBrush()
{
    ClientRadialGradientBrush* pRadialGradientBrush = new ClientRadialGradientBrush(this);

    m_activeBrushes.AddItem(pRadialGradientBrush);

    return pRadialGradientBrush;
}

IClientMediaElement* UIRenderer::CreateMediaElement(const WCHAR* wzSource)
{
    UIVideo* pVideo = new UIVideo(this, wzSource);

    pVideo->Initialize();

    return pVideo;
}


void UIRenderer::set_CurrentBrush(const SolidColorBrush* pBrush)
{
    m_pCurrentBrush = NULL;

    ClientSolidColorBrush::SetRenderStates(m_pDevice, pBrush, m_opacityCurrent);
}

void UIRenderer::set_CurrentBrush(const LinearGradientBrush* pBrush, IClientLinearGradientBrush* pClientBrush)
{
    m_pCurrentBrush = (ClientLinearGradientBrush*) pClientBrush;

    ((ClientLinearGradientBrush*)pClientBrush)->SetRenderStates(m_pDevice, pBrush, m_opacityCurrent);
}

void UIRenderer::set_CurrentBrush(const RadialGradientBrush* pBrush, IClientRadialGradientBrush* pClientBrush)
{
    m_pCurrentBrush = (ClientRadialGradientBrush*) pClientBrush;

    ((ClientRadialGradientBrush*)pClientBrush)->SetRenderStates(m_pDevice, pBrush, m_opacityCurrent);
}

void UIRenderer::set_CurrentBrush(const ImageBrush* pBrush, IClientImageSource* pClientImageSource)
{
    m_pCurrentBrush = NULL;

    ClientImageBrush::SetRenderStates(m_pDevice, pBrush, m_opacityCurrent, (BitmapSourceData*)pClientImageSource);
}

void UIRenderer::set_CurrentBrush(const AVUI::VisualBrush* pBrush, IClientSubgraphCapture* pClientBrush)
{
    m_pCurrentBrush = NULL;

    IDirect3DTexture9* pTexture = NULL;
    if(pClientBrush != NULL)
    {
        pTexture = ((ClientSubgraphCapture*)pClientBrush)->get_Texture();
    }

    ClientVisualBrush::SetRenderStates(m_pDevice, pBrush, pTexture, m_opacityCurrent);
}

void UIRenderer::AddDirtyRects(const AVUI::Rect* pRects, unsigned int cRects)
{
}

void UIRenderer::RenderGeometry3D(AVUI::IClientRenderGeometry* pGeometry)
{
    ClientRenderGeometry* pClientGeometry = (ClientRenderGeometry*) pGeometry;

    if(pClientGeometry->get_IsEmpty())
    {
        return;
    }

    m_pDevice->SetStreamSource( 0, pClientGeometry->m_pVertexBuffer, 0, sizeof(GeometryVertex) );
    m_pDevice->SetIndices(pClientGeometry->m_pIndexBuffer);
    m_pDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CW );
    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pClientGeometry->m_cVertices, 0, pClientGeometry->m_cIndices / 3);
    m_pDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
}


void UIRenderer::RenderGeometry(IClientRenderGeometry* pGeometry, bool isStrokeGeometry)
{   
    ClientRenderGeometry* pClientGeometry = (ClientRenderGeometry*) pGeometry;

    if(pClientGeometry->get_IsEmpty())
    {
        return;
    }

    if(m_pCurrentBrush != NULL)
    {
        m_pCurrentBrush->SetGeometryRenderStates(m_pDevice, AVUI::Rect(AVUI::Point(), AVUI::Size(pClientGeometry->get_Width(), pClientGeometry->get_Height())));
    }

    m_pDevice->SetStreamSource( 0, pClientGeometry->m_pVertexBuffer, 0, sizeof(GeometryVertex) );

    m_pDevice->SetIndices(pClientGeometry->m_pIndexBuffer);
    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pClientGeometry->m_cVertices, 0, pClientGeometry->m_cIndices / 3);
}

void UIRenderer::RenderTextGeometry(IClientTextGeometry* pClientTextGeometry)
{   

    ClientTextGeometry* pTextGeometry = (ClientTextGeometry*) pClientTextGeometry;

    if(pTextGeometry->get_IsEmpty())
    {
        return;
    }

    m_pDevice->SetTexture(1, pTextGeometry->GetFontTexture());

    ClientRenderGeometry* pClientGeometry = pTextGeometry->GetClientRenderGeometry();

    m_pDevice->SetStreamSource( 0, pClientGeometry->m_pVertexBuffer, 0, sizeof(GeometryVertex) );
    m_pDevice->SetIndices(pClientGeometry->m_pIndexBuffer);
    m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pClientGeometry->m_cVertices, 0, pClientGeometry->m_cIndices / 3);

    m_pDevice->SetTexture(1, m_pAATexture);
}

void UIRenderer::RenderMediaElement(AVUI::IClientMediaElement* pMediaElement, const AVUI::Rect& rectArea)
{
    ClientVisualBrush::SetRenderStates(m_pDevice, NULL, ((UIVideo*)pMediaElement)->GetTexture(), m_opacityCurrent);

    DrawRect(rectArea, false);
}

void UIRenderer::Begin3DRender()
{
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
}

void UIRenderer::SetCurrent3DTransform(const Matrix3D& wvpMatrix)
{                                                           
    m_matWorld = *(D3DXMATRIX*)&wvpMatrix;
    D3DXMATRIX matFinal = m_matWorld * m_matView;

    m_pDevice->SetVertexShaderConstantF( 0, (FLOAT*)&matFinal, 4 );
}

void UIRenderer::End3DRender()
{
    m_pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
}


class FileLoader : public AVUI::IXMLParserDataProvider
{
public:
    FileLoader(const WCHAR* wzFileName)
    {
        m_hFile = CreateFileW(wzFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(m_hFile == INVALID_HANDLE_VALUE)
        {
            Sleep(500);
            m_hFile = CreateFileW(wzFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        }
        
    }

    ~FileLoader()
    {
        CloseHandle(m_hFile);
    }

    virtual void ReadData(unsigned char *pbData, int cbMaxData, int *pcbRead)
    {
        ReadFile(m_hFile, pbData, cbMaxData, (DWORD*) pcbRead, NULL);
    }

    HANDLE m_hFile;
};


static AVUI::Object* LoadXaml(const WCHAR* wzXamlFile)
{
    using namespace AVUI;

    XamlParser xamlParser;

    FileLoader fileLoader(wzXamlFile);

    AVUI::XmlParser xmlParser(&fileLoader, &xamlParser);

    ObjectPtr pObject = NULL;

    xmlParser.Parse();
    pObject = xamlParser.get_RootElement();

    pObject->AddRef();

    return pObject;
}

class NavResult : public AVUI::IClientNavResult
{
public:
    NavResult(AVUI::ObjectPtr pObject) : m_pResult(pObject) { }
    virtual AVUI::ObjectPtr GetNavResult() { return m_pResult; }
    virtual AVUI::NavigationStatus::Enum GetNavStatus() { return AVUI::NavigationStatus::Navigated; }
    virtual void Destroy()
    {
        delete this;
    }
private:
    ObjectPtr m_pResult;
};


AVUI::IClientNavResult* UIRenderer::NavigateTo(const WCHAR* wzNavURI)
{
    ObjectPtr navResult(LoadXaml(wzNavURI), false);
    return new NavResult(navResult);
}

ClientRenderGeometry::ClientRenderGeometry(UIRenderer* pUIRenderer) : 
                                                                      m_minX(0.0f), m_minY(0.0f), m_maxX(0.0f), 
                                                                      m_maxY(0.0f), m_pUIRenderer(pUIRenderer), m_cVertices(0), m_cIndices(0), 
                                                                      m_isEmpty(true), m_isSealed(false), m_pVertexBuffer(NULL), 
                                                                      m_pIndexBuffer(NULL), m_noTexCoordGen(false)
{
}

ClientRenderGeometry::~ClientRenderGeometry()
{
    if(m_pVertexBuffer != NULL)
    {
        m_pUIRenderer->ReleaseVertexBuffer(m_pVertexBuffer);
        m_pVertexBuffer = NULL;
    }

    if(m_pIndexBuffer != NULL)
    {
        m_pUIRenderer->ReleaseIndexBuffer(m_pIndexBuffer);
        m_pIndexBuffer = NULL;
    }           
}


void ClientRenderGeometry::AddVertex(float xPosition, float yPosition, bool isAAGeometry)
{
    if(isAAGeometry)
    {   
        AddVertexUV(xPosition, yPosition, 0.0f, 0.0f);    
    }    
    else    
    {    
        AddVertexUV(xPosition, yPosition, 1.0f, 1.0f);    
    }
}

void ClientRenderGeometry::AddVertexUV(float xPosition, float yPosition, float U, float V)
{

    if(m_cVertices == 0)    
    {
        m_pUIRenderer->m_clientVertices.Clear();
        m_minX = m_maxX = xPosition;
        m_minY = m_maxY = yPosition;
    }
    else   
    {    
        m_minX = min(m_minX, xPosition);    
        m_minY = min(m_minY, yPosition);             
        m_maxX = max(m_maxX, xPosition);    
        m_maxY = max(m_maxY, yPosition);    
    }

    m_pUIRenderer->m_clientVertices.AddEmptyItems(1);
    UIRenderer::GeometryVertex* pVertex = m_pUIRenderer->m_clientVertices.get_Ptr() + (m_pUIRenderer->m_clientVertices.get_Count() - 1);

    pVertex->x = xPosition - 0.5f; // Bias to ensure texels are correct
    pVertex->y = yPosition - 0.5f;
    pVertex->z = 0.0f;
    pVertex->tu2 = U;
    pVertex->tv2 = V;

    m_cVertices++;
}

void ClientRenderGeometry::AddVertex3D(float xPosition, float yPosition, float zPosition, float textureU, float textureV)
{
    if(m_cVertices == 0)
    {
        m_pUIRenderer->m_clientVertices.Clear();
    }

    m_pUIRenderer->m_clientVertices.AddEmptyItems(1);

    UIRenderer::GeometryVertex* pVertex = m_pUIRenderer->m_clientVertices.get_Ptr() + (m_pUIRenderer->m_clientVertices.get_Count() - 1);

    pVertex->x = xPosition;
    pVertex->y = yPosition;
    pVertex->z = zPosition;
    pVertex->tu1 = textureU;
    pVertex->tv1 = textureV;
    pVertex->tu2 = 1.0f;
    pVertex->tv2 = 1.0f;


    m_cVertices++;

    m_noTexCoordGen = true;
}


void ClientRenderGeometry::AddIndex(unsigned int index)
{
    if(m_cIndices == 0)
    {
        m_pUIRenderer->m_clientIndices.Clear();
    }

    m_pUIRenderer->m_clientIndices.AddItem(index);
    m_cIndices++;
}

void ClientRenderGeometry::AddIndexTriplet(unsigned int index1, unsigned int index2, unsigned int index3)
{
    if(m_isSealed)
    {    
        LibraryCriticalError();
        return;   
    } 



    AddIndex(index1);
    AddIndex(index2);
    AddIndex(index3);
}

void ClientRenderGeometry::BeginFigure()
{

}

void ClientRenderGeometry::EndFigure()
{

}

void ClientRenderGeometry::AllocateBuffers()
{
    if(m_cVertices == 0 || m_cIndices == 0)
    {
        return;
    }              

    float bboxXCenter = (m_maxX + m_minX) / 2.0f;
    float bboxYCenter = (m_maxY + m_minY) / 2.0f;

    if(!m_noTexCoordGen)
    {
        for(int i = 0; i < m_cVertices; i++)

        {    
            UIRenderer::GeometryVertex* pVertex = m_pUIRenderer->m_clientVertices.get_Ptr() + i;

            pVertex->tu1 = (((pVertex->x - bboxXCenter) / (m_maxX - m_minX)) + 0.5f);   
            pVertex->tv1 = (((pVertex->y - bboxYCenter) / (m_maxY - m_minY)) + 0.5f);    
        }
    }

    m_pVertexBuffer = m_pUIRenderer->CreateVertexBuffer(m_cVertices);

    UIRenderer::GeometryVertex* pVertexData;

    m_pVertexBuffer->Lock(0, 0, (void**) &pVertexData, D3DLOCK_DISCARD);
    memcpy(pVertexData, m_pUIRenderer->m_clientVertices.get_Ptr(), m_cVertices * sizeof(UIRenderer::GeometryVertex));

    m_pVertexBuffer->Unlock();

    m_pIndexBuffer = m_pUIRenderer->CreateIndexBuffer(m_cIndices);

    unsigned short* pIndexData;

    m_pIndexBuffer->Lock(0, 0, (void**) &pIndexData, D3DLOCK_DISCARD);

    memcpy(pIndexData, m_pUIRenderer->m_clientIndices.get_Ptr(), m_cIndices * sizeof(short));

    m_pIndexBuffer->Unlock();
    m_isEmpty = false;
}

void ClientRenderGeometry::Seal()
{
    if(m_isSealed)
    {
        return;
    }

    AllocateBuffers();

    m_isSealed = true;
}

void UIRenderer::DeviceLost()
{
    for( int idx = 0; idx < m_activeVideos.get_Count(); idx++)
    {
        m_activeVideos[idx]->DeviceLost();
    }

    Application::get_Current()->ReleaseRenderData();

    CleanupDeviceData();
    ClearCachedRenderInterfaces();

    m_pFontManager->DeviceLost();
    m_pTextureManager->DeviceLost();

    for(int idx = 0; idx < m_activeBrushes.get_Count(); idx++)
    {
        m_activeBrushes[idx]->DeviceLost();
    }
}

void UIRenderer::DeviceReset(IDirect3DDevice9* pDevice)
{
    m_pDevice = pDevice;

    for(int idx = 0; idx < m_activeVideos.get_Count(); idx++)
    {
        m_activeVideos[idx]->DeviceReset();
    }

    InitDeviceData();

    m_pFontManager->DeviceReset();
    m_pTextureManager->DeviceReset();

    for(int idx = 0; idx < m_activeBrushes.get_Count(); idx++)
    {
        m_activeBrushes[idx]->DeviceReset();
    }

}

void UIRenderer::AddActiveVideo(UIVideo* pVideo)
{
    m_activeVideos.AddItem(pVideo);
}

void UIRenderer::RemoveActiveVideo(UIVideo* pVideo)
{
    m_activeVideos.Remove(pVideo);
}


IDirect3DTexture9* UIRenderer::GetRenderTarget(unsigned int width, unsigned int height)
{
    int idx = 0;

    for(idx = 0; idx < m_cachedRenderTargets.get_Count(); idx++)
    {
        if(m_cachedRenderTargets[idx].height == height &&
           m_cachedRenderTargets[idx].width == width)
        {
            break;
        }
    }

    IDirect3DTexture9* pRetTarget = NULL;
    if(idx == m_cachedRenderTargets.get_Count())
    {
        m_pDevice->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pRetTarget, NULL);       
        return pRetTarget;
    }

    pRetTarget = m_cachedRenderTargets[idx].pRenderTargetTexture;

    m_cachedRenderTargets.RemoveAt(idx);

    return pRetTarget;
}

void UIRenderer::FreeRenderTarget(IDirect3DTexture9* pRenderTarget)
{
    D3DSURFACE_DESC desc;

    pRenderTarget->GetLevelDesc(0, &desc);

    m_cachedRenderTargets.AddEmptyItems(1);
    CachedRenderTarget* pCachedRenderTarget = m_cachedRenderTargets.get_Ptr() + (m_cachedRenderTargets.get_Count() - 1);
    pCachedRenderTarget->height = desc.Height;
    pCachedRenderTarget->width = desc.Width;
    pCachedRenderTarget->lastFrameUsed = m_frameCount;
    pCachedRenderTarget->pRenderTargetTexture = pRenderTarget;

}

void UIRenderer::ReleaseVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer)
{
    D3DVERTEXBUFFER_DESC desc;
    pVertexBuffer->GetDesc(&desc);

    CachedVertexBuffer cachedBuffer;

    cachedBuffer.pVertexBuffer = pVertexBuffer;
    cachedBuffer.cVerts = desc.Size / sizeof(GeometryVertex);

    m_cachedVertexBuffers.AddItem(cachedBuffer);
}

void UIRenderer::ReleaseIndexBuffer(IDirect3DIndexBuffer9* pIndexBuffer)
{
    D3DINDEXBUFFER_DESC desc;
    pIndexBuffer->GetDesc(&desc);

    CachedIndexBuffer cachedBuffer;

    cachedBuffer.pIndexBuffer = pIndexBuffer;
    cachedBuffer.cIndices = desc.Size / sizeof(WORD);

    m_cachedIndexBuffers.AddItem(cachedBuffer);
}

IDirect3DVertexBuffer9* UIRenderer::CreateVertexBuffer(int cVerts)
{
    IDirect3DVertexBuffer9* pVertexBuffer = NULL;
    int cAlignedVerts = NextPow2(cVerts, -1);

    int index = -1;

    for(int i = m_cachedVertexBuffers.get_Count() - 1; i >= 0; i--)
    {
        if(m_cachedVertexBuffers[i].cVerts == cAlignedVerts)
        {
            index = i;
            break;
        }
    }

    if(index != -1)
    {
        pVertexBuffer = m_cachedVertexBuffers[index].pVertexBuffer;
        m_cachedVertexBuffers.RemoveAt(index);

        return pVertexBuffer;
    }
   
    m_pDevice->CreateVertexBuffer(sizeof(UIRenderer::GeometryVertex) * cAlignedVerts, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, NULL, D3DPOOL_DEFAULT, &pVertexBuffer, NULL);

    return pVertexBuffer;
}

IDirect3DIndexBuffer9* UIRenderer::CreateIndexBuffer(int cIndices)
{
    IDirect3DIndexBuffer9* pIndexBuffer = NULL;
    int cAlignedIndices = NextPow2(cIndices, -1);

    int index = -1;

    for(int i = m_cachedIndexBuffers.get_Count() - 1; i >= 0; i--)
    {
        if(m_cachedIndexBuffers[i].cIndices == cAlignedIndices)
        {
            index = i;
            break;
        }
    }

    if(index != -1)
    {
        pIndexBuffer = m_cachedIndexBuffers[index].pIndexBuffer;
        m_cachedIndexBuffers.RemoveAt(index);

        return pIndexBuffer;
    }


    m_pDevice->CreateIndexBuffer(cAlignedIndices * sizeof(unsigned short), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIndexBuffer, NULL);

    return pIndexBuffer;
}

void UIRenderer::RemoveActiveBrush(ClientBrush* pBrush)
{
    m_activeBrushes.Remove(pBrush);
}

int UIRenderer::NextPow2(int value, int maxValue)
{
    int pow2 = 1;

    while(pow2 < value)
    {
        pow2 = pow2 << 1;
    }

    if(maxValue == -1)
    {
        return pow2;
    }

    return min(pow2, maxValue);
}


