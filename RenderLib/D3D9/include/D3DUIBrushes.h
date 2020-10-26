#pragma once

#include <AVUIIClientRendererInterface.h>
#include <D3DX9.h>


class ClientRenderGeometry;
class BitmapSourceData;

class ShaderHelper
{
public:
    static void CompileShaders(IDirect3DDevice9* pDevice, const char* szVertexShader, const char* szPixelShader,
                               IDirect3DVertexShader9** ppVertexShader, IDirect3DPixelShader9** ppPixelShader);
};

class ClientBrush
{
    friend class UIRenderer;

protected:
    virtual ~ClientBrush() { }

    virtual void SetGeometryRenderStates(IDirect3DDevice9* pDevice, const AVUI::Rect& rect) { };

    virtual void DeviceLost() { }
    virtual void DeviceReset() { };

private:
    static void StaticInitBrushes(IDirect3DDevice9* pDevice);
    static void StaticCleanupBrushes(IDirect3DDevice9* pDevice);
};

class ClientSolidColorBrush
{
public:

    static void SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::SolidColorBrush* pBrush, float opacityCurrent);

    static void StaticInitBrush(IDirect3DDevice9* pDevice);
    static void StaticCleanupBrush(IDirect3DDevice9* pDevice);

private:

    static IDirect3DPixelShader9* m_pPixelShader;
    static IDirect3DVertexShader9* m_pVertexShader;
    static const char* m_szVertexShaderProgram;
    static const char* m_szPixelShaderProgram;
};


class ClientTileBrush : public ClientBrush
{

protected:
    static void SetTileModeRenderState(IDirect3DDevice9* pDevice, AVUI::TileMode::Enum tileMode);
};

class ClientImageBrush : public ClientTileBrush
{
public:
    static void SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::ImageBrush* pImageBrush, float opacityCurrent, BitmapSourceData* pSourceData);
    static void StaticInitBrush(IDirect3DDevice9* pDevice);
    static void StaticCleanupBrush(IDirect3DDevice9* pDevice);
private:

    static IDirect3DPixelShader9* m_pPixelShader;
    static IDirect3DVertexShader9* m_pVertexShader;
    static const char* m_szVertexShaderProgram;
    static const char* m_szPixelShaderProgram;
};



class ClientGradientBrush : public ClientBrush
{
private:
public:

    ClientGradientBrush(UIRenderer* pRenderer) : m_pRenderer(pRenderer), m_rgGradientStops(NULL), m_cGradientStops(0), m_cMaxGradientStops(0), m_pTexture(NULL) { };
    ~ClientGradientBrush();

    virtual void BeginGradientUpdate(unsigned int cGradientStops);
    virtual void AddGradientStop(float offset, unsigned int argb);
    virtual void EndGradientUpdate();

    static void StaticInitBrush(IDirect3DDevice9* pDevice);
    static void StaticCleanupBrush(IDirect3DDevice9* pDevice);

    virtual void DeviceLost();
    virtual void DeviceReset();

private:
    void RecalculateGradientImage();
    int CalculateTextureResolution();
    void FillBufferWithGradient(void* pvOut, int resolution);
    unsigned int CalculateGradient(float position);
    unsigned int InterpolateGradient(int idx, float position);
    int CalculateTotalColorSpaceSquared(int idx, int idxT);
    unsigned char InterpolateColor(unsigned int colorFrom, unsigned int colorTo, 
                                   float interpValue, unsigned int shift);

    struct GradientStop
    {
        unsigned int argb;
        float offset;
    };

protected:

    void SetSpreadMethodRenderState(AVUI::GradientSpreadMethod::Enum spreadMethod);

    UIRenderer* m_pRenderer;
    IDirect3DTexture9* m_pTexture;

private:
    GradientStop* m_rgGradientStops;
    int m_cGradientStops;
    int m_cMaxGradientStops;
};



class ClientLinearGradientBrush : public ClientGradientBrush, public AVUI::IClientLinearGradientBrush
{
public:

    ClientLinearGradientBrush(UIRenderer* pRenderer);
    ~ClientLinearGradientBrush() { }

    void Destroy() { delete this; }

    void GetGradientVector(float* pVecX, float* pVecY);
    void GetGradientCenter(float* pxCenter, float* pyCenter);
    D3DXMATRIX GetTextureMatrix(float bboxWidth, float bboxHeight);

    void SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::LinearGradientBrush* pBrush, float opacityCurrent);

    virtual void BeginGradientUpdate(unsigned int cGradientStops) { ClientGradientBrush::BeginGradientUpdate(cGradientStops); }
    virtual void AddGradientStop(float offset, unsigned int argb) { ClientGradientBrush::AddGradientStop(offset, argb); }
    virtual void EndGradientUpdate() { ClientGradientBrush::EndGradientUpdate(); }

    virtual void SetGeometryRenderStates(IDirect3DDevice9* pDevice, const AVUI::Rect& rect);

    static void StaticInitBrush(IDirect3DDevice9* pDevice);
    static void StaticCleanupBrush(IDirect3DDevice9* pDevice);

private:

    float m_xStart, m_yStart;
    float m_xEnd, m_yEnd;

    static IDirect3DPixelShader9* m_pPixelShader;
    static IDirect3DVertexShader9* m_pVertexShader;
    static const char* m_szVertexShaderProgram;
    static const char* m_szPixelShaderProgram;
};


class ClientRadialGradientBrush : public ClientGradientBrush, public AVUI::IClientRadialGradientBrush
{
public:

    ClientRadialGradientBrush(UIRenderer* pRenderer);
    ~ClientRadialGradientBrush() { }

    void Destroy() { delete this; }

    virtual void set_Center(float x, float y) { m_xCenter = x; m_yCenter = y; }
    virtual void set_RadiusX(float x) { m_radiusX = x; }
    virtual void set_RadiusY(float y) { m_radiusY = y; }
    virtual void set_GradientOrigin(float x, float y) { m_xGradientOrigin = x; m_yGradientOrigin = y; }

    void SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::RadialGradientBrush* pBrush, float opacityCurrent);

    virtual void BeginGradientUpdate(unsigned int cGradientStops) { ClientGradientBrush::BeginGradientUpdate(cGradientStops); }
    virtual void AddGradientStop(float offset, unsigned int argb) { ClientGradientBrush::AddGradientStop(offset, argb); }
    virtual void EndGradientUpdate() { ClientGradientBrush::EndGradientUpdate(); }

    static void StaticInitBrush(IDirect3DDevice9* pDevice);
    static void StaticCleanupBrush(IDirect3DDevice9* pDevice);

private:

    float m_xCenter, m_yCenter;
    float m_radiusX;
    float m_radiusY;
    float m_xGradientOrigin, m_yGradientOrigin;

    static IDirect3DPixelShader9* m_pPixelShader;
    static IDirect3DVertexShader9* m_pVertexShader;
    static const char* m_szVertexShaderProgram;
    static const char* m_szPixelShaderProgram;
};


class ClientVisualBrush : public ClientTileBrush
{
public:

    ~ClientVisualBrush() { }

    void Destroy() { delete this; }

    static void SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::VisualBrush* pBrush, IDirect3DTexture9* pTexture, float opacityCurrent);

    static void StaticInitBrush(IDirect3DDevice9* pDevice);
    static void StaticCleanupBrush(IDirect3DDevice9* pDevice);

private:

    static IDirect3DPixelShader9* m_pPixelShader;
    static IDirect3DVertexShader9* m_pVertexShader;
    static const char* m_szVertexShaderProgram;
    static const char* m_szPixelShaderProgram;
};


