#include <Windows.h>
#include <AVUICommon.h>
#include <AVUISolidColorBrush.h>
#include <AVUILinearGradientBrush.h>
#include <AVUIRadialGradientBrush.h>
#include <AVUIImageBrush.h>
#include <AVUIVisualBrush.h>
#include <AVUIMatrix.h>
#include <AVUITransform.h>
#include <D3DUIBrushes.h>
#include <D3DUIRenderer.h>
#include <D3DUITextureManager.h>


void ClientBrush::StaticInitBrushes(IDirect3DDevice9* pDevice)
{
    ClientSolidColorBrush::StaticInitBrush(pDevice);
    ClientImageBrush::StaticInitBrush(pDevice);
    ClientLinearGradientBrush::StaticInitBrush(pDevice);
    ClientRadialGradientBrush::StaticInitBrush(pDevice);
    ClientVisualBrush::StaticInitBrush(pDevice);
}

void ClientBrush::StaticCleanupBrushes(IDirect3DDevice9* pDevice)
{
    ClientSolidColorBrush::StaticCleanupBrush(pDevice);
    ClientImageBrush::StaticCleanupBrush(pDevice);
    ClientLinearGradientBrush::StaticCleanupBrush(pDevice);
    ClientRadialGradientBrush::StaticCleanupBrush(pDevice);
    ClientVisualBrush::StaticCleanupBrush(pDevice);
}


void ClientSolidColorBrush::StaticInitBrush(IDirect3DDevice9* pDevice)
{
    ShaderHelper::CompileShaders(pDevice, m_szVertexShaderProgram, m_szPixelShaderProgram, &m_pVertexShader, &m_pPixelShader);
}

void ClientSolidColorBrush::StaticCleanupBrush(IDirect3DDevice9* pDevice)
{
    if(m_pVertexShader != NULL)
    {
        m_pVertexShader->Release();
    }

    if(m_pPixelShader != NULL)
    {
        m_pPixelShader->Release();
    }
}


void ClientImageBrush::StaticInitBrush(IDirect3DDevice9* pDevice)
{
    ShaderHelper::CompileShaders(pDevice, m_szVertexShaderProgram, m_szPixelShaderProgram, &m_pVertexShader, &m_pPixelShader);
}

void ClientImageBrush::StaticCleanupBrush(IDirect3DDevice9* pDevice)
{
    if(m_pVertexShader != NULL)
    {
        m_pVertexShader->Release();
    }

    if(m_pPixelShader != NULL)
    {
        m_pPixelShader->Release();
    }
}

void ClientLinearGradientBrush::StaticInitBrush(IDirect3DDevice9* pDevice)
{
    ShaderHelper::CompileShaders(pDevice, m_szVertexShaderProgram, m_szPixelShaderProgram, &m_pVertexShader, &m_pPixelShader);
}

void ClientLinearGradientBrush::StaticCleanupBrush(IDirect3DDevice9* pDevice)
{
    if(m_pVertexShader != NULL)
    {
        m_pVertexShader->Release();
    }

    if(m_pPixelShader != NULL)
    {
        m_pPixelShader->Release();
    }
}

ClientLinearGradientBrush::ClientLinearGradientBrush(UIRenderer* pRenderer) : ClientGradientBrush(pRenderer)
{
}

void ClientRadialGradientBrush::StaticInitBrush(IDirect3DDevice9* pDevice)
{
    ShaderHelper::CompileShaders(pDevice, m_szVertexShaderProgram, m_szPixelShaderProgram, &m_pVertexShader, &m_pPixelShader);
}

void ClientRadialGradientBrush::StaticCleanupBrush(IDirect3DDevice9* pDevice)
{
    if(m_pVertexShader != NULL)
    {
        m_pVertexShader->Release();
    }

    if(m_pPixelShader != NULL)
    {
        m_pPixelShader->Release();
    }
}

ClientRadialGradientBrush::ClientRadialGradientBrush(UIRenderer* pRenderer) : ClientGradientBrush(pRenderer)
{
}

ClientGradientBrush::~ClientGradientBrush()
{
    if(m_rgGradientStops != NULL)
    {
        delete[] m_rgGradientStops;
    }

    if(m_pTexture != NULL)
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }

    m_pRenderer->RemoveActiveBrush(this);
}

void ClientGradientBrush::DeviceLost()
{
    if(m_pTexture != NULL)
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }
}

void ClientGradientBrush::DeviceReset()
{
    RecalculateGradientImage();
}


void ClientVisualBrush::StaticInitBrush(IDirect3DDevice9* pDevice)
{
    ShaderHelper::CompileShaders(pDevice, m_szVertexShaderProgram, m_szPixelShaderProgram, &m_pVertexShader, &m_pPixelShader);
}

void ClientVisualBrush::StaticCleanupBrush(IDirect3DDevice9* pDevice)
{
    if(m_pVertexShader != NULL)
    {
        m_pVertexShader->Release();
    }

    if(m_pPixelShader != NULL)
    {
        m_pPixelShader->Release();
    }
}


void ClientGradientBrush::SetSpreadMethodRenderState(AVUI::GradientSpreadMethod::Enum spreadMethod)
{
    if(m_pRenderer->get_Device() == NULL)
    {
        return;
    }

    D3DTEXTUREADDRESS textureAddress;

    switch(spreadMethod)
    {
    case AVUI::GradientSpreadMethod::Pad:
        textureAddress = D3DTADDRESS_CLAMP;
        break;

    case AVUI::GradientSpreadMethod::Reflect:
        textureAddress = D3DTADDRESS_MIRROR;
        break;

    default:
        textureAddress = D3DTADDRESS_WRAP;
    }

    m_pRenderer->get_Device()->SetSamplerState(0, D3DSAMP_ADDRESSU, textureAddress);
    m_pRenderer->get_Device()->SetSamplerState(0, D3DSAMP_ADDRESSV, textureAddress);
}

void ClientGradientBrush::BeginGradientUpdate(unsigned int cGradientStops)
{
    if(m_cMaxGradientStops < (int)cGradientStops)
    {
        delete[] m_rgGradientStops;

        m_cMaxGradientStops = cGradientStops;

        m_rgGradientStops = new GradientStop[cGradientStops];
    }

    m_cGradientStops = 0;
}



void ClientGradientBrush::AddGradientStop(float offset, unsigned int argb)
{
    GradientStop* pStop = m_rgGradientStops + m_cGradientStops;
    pStop->argb = argb;
    pStop->offset = offset;

    m_cGradientStops++;
}

void ClientGradientBrush::EndGradientUpdate()
{
    if(m_cGradientStops > 1)
    {
        bool didSwap = true;

        for(int idxOuter = 0; idxOuter < m_cGradientStops - 1 && didSwap; idxOuter++)
        {
            didSwap = false;

            for(int idxInner = 0; idxInner < m_cGradientStops - 1; idxInner++)
            {
               
                if(m_rgGradientStops[idxInner].offset > m_rgGradientStops[idxInner + 1].offset)
                {
                    didSwap = true;
                    GradientStop temp = m_rgGradientStops[idxInner + 1];
                    m_rgGradientStops[idxInner + 1] = m_rgGradientStops[idxInner];
                    m_rgGradientStops[idxInner] = temp;
                }
            }
        }
    }

    RecalculateGradientImage();
}

void ClientGradientBrush::RecalculateGradientImage()
{

    if(m_pTexture != NULL)
    {
        m_pTexture->Release();
        m_pTexture = NULL;
    }

    if(m_rgGradientStops == NULL || m_cGradientStops == 0)
    {
        return;
    }

    int resolutionRequired = CalculateTextureResolution();

    m_pRenderer->get_Device()->CreateTexture(resolutionRequired, 1, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture, NULL);

    D3DLOCKED_RECT lockedRect;
    m_pTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_DISCARD);

    FillBufferWithGradient(lockedRect.pBits, resolutionRequired);

    m_pTexture->UnlockRect(0);
}

void ClientGradientBrush::FillBufferWithGradient(void* pvOut, int resolution)
{
    unsigned int* pData = (unsigned int*) pvOut;

    float position = 0.0f;
    float adder = 1.0f / (resolution - 1);
    for(int idx = 0; idx < resolution; idx++)
    {
        pData[idx] = CalculateGradient(position);

        position += adder;
    }
}

unsigned int ClientGradientBrush::CalculateGradient(float position)
{
    if(position >= m_rgGradientStops[m_cGradientStops - 1].offset)
    {
        return m_rgGradientStops[m_cGradientStops - 1].argb;
    }

    if(position <= m_rgGradientStops[0].offset)
    {
        return m_rgGradientStops[0].argb;
    }

    unsigned int color = 0;

    for(int idx = 0; idx < m_cGradientStops - 1; idx++)
    {
        if(position > m_rgGradientStops[idx].offset && position < m_rgGradientStops[idx + 1].offset)
        {
            color = InterpolateGradient(idx, position);
        }
    }

    return color;
}

unsigned char ClientGradientBrush::InterpolateColor(unsigned int colorFrom, unsigned int colorTo, 
                                                          float interpValue, unsigned int shift)
{
    int curColorFrom = ((colorFrom >> shift) & 0xFF);
    int curColorTo = ((colorTo >> shift) & 0xFF);


     float colorValue = (1.0f * curColorFrom) + (curColorTo - curColorFrom) * interpValue;

     colorValue = max(min(colorValue, 255), 0);

     return (unsigned char) colorValue;
}

unsigned int ClientGradientBrush::InterpolateGradient(int idx, float position)
{
    float interpValue = (position - m_rgGradientStops[idx].offset) / (m_rgGradientStops[idx + 1].offset - m_rgGradientStops[idx].offset);

    unsigned int colorFrom = m_rgGradientStops[idx].argb;
    unsigned int colorTo = m_rgGradientStops[idx + 1].argb;


    return InterpolateColor(colorFrom, colorTo, interpValue, 0) + (InterpolateColor(colorFrom, colorTo, interpValue, 8) << 8) +
           (InterpolateColor(colorFrom, colorTo, interpValue, 16) << 16) + (InterpolateColor(colorFrom, colorTo, interpValue, 24) << 24);
}

void ClientLinearGradientBrush::GetGradientVector(float* pVecX, float* pVecY)
{
    *pVecX = m_xEnd - m_xStart;
    *pVecY = m_yEnd - m_yStart;
}

D3DXMATRIX ClientLinearGradientBrush::GetTextureMatrix(float bboxWidth, float bboxHeight)
{
    float vectorX, vectorY;

    GetGradientVector(&vectorX, &vectorY);

    vectorX *= bboxWidth;
    vectorY *= bboxHeight;

    float lenSq = vectorX * vectorX + vectorY * vectorY;

    D3DXMATRIX rotMatrix(1.0f / lenSq, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f);



    D3DXMATRIX transformBackMatrix(1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f,
                                   -bboxWidth * m_xStart, -bboxHeight * m_yStart, 0.0f, 1.0f);

    D3DXMATRIX unitProjectionMatrix(vectorX, 0.0f, 0.0f, 0.0f,
                                    vectorY, 0.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f,
                                    0, 0, 0.0f, 1.0f);

    D3DXMATRIX textureToBboxMatrix(bboxWidth, 0.0f, 0.0f, 0.0f,
                                   0.0f, bboxHeight, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 1.0f);

    return (textureToBboxMatrix * transformBackMatrix * unitProjectionMatrix * rotMatrix);
}



int ClientGradientBrush::CalculateTextureResolution()
{
    if(m_cGradientStops <= 1)
    {
        return 1; 
    }

    int totalColorSpace = 0;

    for(int idx = 0; idx < m_cGradientStops - 1; idx++)
    {
        totalColorSpace += CalculateTotalColorSpaceSquared(idx, idx + 1);
    }

    return UIRenderer::NextPow2((int)sqrtf((float)totalColorSpace), 1024);
}

int ClientGradientBrush::CalculateTotalColorSpaceSquared(int idx, int idxT)
{
    GradientStop* pOne = m_rgGradientStops + idx;
    GradientStop* pTwo = m_rgGradientStops + idxT;

    unsigned int aDiff = ((pOne->argb >> 24) & 0xFF) - ((pTwo->argb >> 24) & 0xFF);
    unsigned int rDiff = ((pOne->argb >> 16) & 0xFF) - ((pTwo->argb >> 16) & 0xFF);
    unsigned int gDiff = ((pOne->argb >> 8) & 0xFF) - ((pTwo->argb >> 8) & 0xFF);
    unsigned int bDiff = ((pOne->argb >> 0) & 0xFF) - ((pTwo->argb >> 0) & 0xFF);

    return max(max(max(aDiff * aDiff, rDiff * rDiff), gDiff * gDiff), bDiff * bDiff);
}


void ShaderHelper::CompileShaders(IDirect3DDevice9* pDevice, const char* szVertexShader, const char* szPixelShader,
                    IDirect3DVertexShader9** ppVertexShader, IDirect3DPixelShader9** ppPixelShader)
{
    ID3DXBuffer* pVertexShaderCode;
    ID3DXBuffer* pVertexErrorMsg;

    HRESULT hr = D3DXCompileShader(szVertexShader, 
                                    (UINT)strlen(szVertexShader),
                                    NULL, 
                                    NULL, 
                                    "main", 
                                    "vs_1_1", 
                                    0, 
                                    &pVertexShaderCode, 
                                    &pVertexErrorMsg, 
                                    NULL );

    if( FAILED(hr) )
    {
        if( pVertexErrorMsg )
            OutputDebugStringA( (char*)pVertexErrorMsg->GetBufferPointer() );

        if(pVertexShaderCode != NULL)
        {
            pVertexShaderCode->Release();
        }

        if(pVertexErrorMsg != NULL)
        {
            pVertexErrorMsg->Release();
        }

        return;
    }    

    // Create vertex shader.
    pDevice->CreateVertexShader( (DWORD*)pVertexShaderCode->GetBufferPointer(), 
                                      ppVertexShader );

    if(pVertexShaderCode != NULL)
    {
        pVertexShaderCode->Release();
    }

    if(pVertexErrorMsg != NULL)
    {
        pVertexErrorMsg->Release();
    }

    // Compile pixel shader.
    ID3DXBuffer* pPixelShaderCode;
    ID3DXBuffer* pPixelErrorMsg;
    hr = D3DXCompileShader( szPixelShader, 
                            (UINT)strlen( szPixelShader ),
                            NULL, 
                            NULL, 
                            "main", 
                            "ps_2_0", 
                            0, 
                            &pPixelShaderCode, 
                            &pPixelErrorMsg,
                            NULL );
    if( FAILED(hr) )
    {
        if( pPixelErrorMsg )
            OutputDebugStringA( (char*)pPixelErrorMsg->GetBufferPointer() );

        if(pPixelShaderCode != NULL)
        {
            pPixelShaderCode->Release();
        }

        if(pPixelErrorMsg != NULL)
        {            
            pPixelErrorMsg->Release();
        }

        return;
    }
 
    // Create pixel shader.
    pDevice->CreatePixelShader( (DWORD*)pPixelShaderCode->GetBufferPointer(), 
                                  ppPixelShader );


    if(pPixelShaderCode != NULL)
    {
        pPixelShaderCode->Release();
    }

    if(pPixelErrorMsg != NULL)
    {            
        pPixelErrorMsg->Release();
    }
}


const char* ClientSolidColorBrush::m_szVertexShaderProgram = 
" float4x4 matWVP : register(c0);              "  
"                                              "  
" struct VS_IN                                 "  
" {                                            " 
"     float4 ObjPos   : POSITION;              "  // Object space position 
"     float2 TexCoord1: TEXCOORD1;             "  
" };                                           " 
"                                              " 
" struct VS_OUT                                " 
" {                                            " 
"     float4 ProjPos  : POSITION;              "  // Projected space position 
"     float4 TexCoord1: TEXCOORD1;             "  
" };                                           "  
"                                              "  
" VS_OUT main( VS_IN In )                      "  
" {                                            "  
"     VS_OUT Out;                              "  
"     Out.ProjPos = mul( matWVP, In.ObjPos );  "  // Transform vertex into
"     Out.TexCoord1.xy = In.TexCoord1;          "
"     Out.TexCoord1.zw = 0;                     "
"     return Out;                              "  // Transfer color
" }                                            ";
 
const char* ClientSolidColorBrush::m_szPixelShaderProgram = 
" sampler   baseTexture : register(s0);        "
" sampler   modTexture  : register(s1);        "
" float4    color : register(c0);              "  
" struct PS_IN                                 "
" {                                            "
"     float4 TexCoord1 : TEXCOORD1;            "  
" };                                           "  // the vertex shader
"                                              "  
" float4 main( PS_IN In ) : COLOR              "  
" {                                            "
"   float4 texel = tex2D( modTexture, In.TexCoord1.xy ); "
"                                                       "
"   float4 outputColor = color * texel;        "  // Output color
"   return outputColor * float4(outputColor.w, outputColor.w, outputColor.w, 1.0);"
" }                                            "; 

IDirect3DPixelShader9* ClientSolidColorBrush::m_pPixelShader;
IDirect3DVertexShader9* ClientSolidColorBrush::m_pVertexShader;

void ClientSolidColorBrush::SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::SolidColorBrush* pBrush, float opacityCurrent)
{
    pDevice->SetVertexShader( m_pVertexShader );
    pDevice->SetPixelShader( m_pPixelShader );

    D3DXCOLOR color(pBrush->get_Color().get_ARGB());

    color.a *= opacityCurrent;

    pDevice->SetPixelShaderConstantF(0, (FLOAT*)&color, 1);
}



const char* ClientLinearGradientBrush::m_szVertexShaderProgram = 
" float4x4 matWVP : register(c0);              "  
" float4x4 matTexture : register(c4);          "  
"                                              "  
" struct VS_IN                                 "  
" {                                            " 
"     float4 ObjPos   : POSITION;              "  // Object space position 
"     float2 TexCoord0: TEXCOORD0;             "  
"     float2 TexCoord1: TEXCOORD1;             "  
" };                                           " 
"                                              " 
" struct VS_OUT                                " 
" {                                            " 
"     float4 ProjPos  : POSITION;              "  // Projected space position 
"     float4 TexCoord: TEXCOORD0;             "  
"     float4 TexCoord1: TEXCOORD1;             "  
" };                                           "  
"                                              "  
" VS_OUT main( VS_IN In )                      "  
" {                                            "  
"     VS_OUT Out;                              "  
"     float4 texMul = float4( In.TexCoord0.x, In.TexCoord0.y, 0, 1 ); "
"     Out.ProjPos = mul( matWVP, In.ObjPos );  "  // Transform vertex into"     
"     Out.TexCoord.xy = mul(texMul, matTexture).xy;  "
"     Out.TexCoord.zw = 0; "
"     Out.TexCoord1.xy = In.TexCoord1;          "
"     Out.TexCoord1.zw = 0;          "
"     return Out;                              "  // Transfer color
" }                                            ";
 
const char* ClientLinearGradientBrush::m_szPixelShaderProgram = 
" sampler   baseTexture : register(s0);        "
" sampler   modTexture  : register(s1);        "
" float4    color : register(c0);              "  
" struct PS_IN                                 "
" {                                            "
"     float4 TexCoord : TEXCOORD0;            "  
"     float4 TexCoord1 : TEXCOORD1;            "  
" };                                           "  // the vertex shader
"                                              "  
" float4 main( PS_IN In ) : COLOR              "  
" {                                            "
"   float4 firstTexel = tex2D( baseTexture, In.TexCoord.xy ); "
"   float4 secondTexel = tex2D( modTexture, In.TexCoord1.xy ); "
"                                                             "
"   float4 outputColor = color * firstTexel * secondTexel;    "  // Output color
"   return outputColor * float4(outputColor.w, outputColor.w, outputColor.w, 1.0); "
" }                                            "; 

IDirect3DPixelShader9* ClientLinearGradientBrush::m_pPixelShader;
IDirect3DVertexShader9* ClientLinearGradientBrush::m_pVertexShader;

void ClientLinearGradientBrush::SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::LinearGradientBrush* pBrush, float opacityCurrent)
{
    m_xStart = pBrush->get_StartPoint().get_X();
    m_yStart = pBrush->get_StartPoint().get_Y();

    m_xEnd = pBrush->get_EndPoint().get_X();
    m_yEnd = pBrush->get_EndPoint().get_Y();

    pDevice->SetVertexShader( m_pVertexShader );
    pDevice->SetPixelShader( m_pPixelShader );

    D3DXCOLOR colorOpacity;
    colorOpacity.r = colorOpacity.g = colorOpacity.b = 1.0f;
    colorOpacity.a = opacityCurrent;

    SetSpreadMethodRenderState(pBrush->get_SpreadMethod());

    pDevice->SetPixelShaderConstantF(0, (FLOAT*)&colorOpacity, 1);
    pDevice->SetTexture(0, m_pTexture);

}

void ClientLinearGradientBrush::SetGeometryRenderStates(IDirect3DDevice9* pDevice, const AVUI::Rect& rect)
{
    D3DXMATRIX textureMatrix = GetTextureMatrix(rect.get_Width(), rect.get_Height());
    D3DXMATRIX textureMatrixT;

    D3DXMatrixTranspose(&textureMatrixT, &textureMatrix);
    pDevice->SetVertexShaderConstantF( 4, (FLOAT*)&textureMatrixT, 4 );
}


const char* ClientRadialGradientBrush::m_szVertexShaderProgram = 
" float4x4 matWVP : register(c0);              "  
" float4x4 matTexture : register(c4);          "  
"                                              "  
" struct VS_IN                                 "  
" {                                            " 
"     float4 ObjPos   : POSITION;              "  // Object space position 
"     float2 TexCoord0: TEXCOORD0;             "  
"     float2 TexCoord1: TEXCOORD1;             "  
" };                                           " 
"                                              " 
" struct VS_OUT                                " 
" {                                            " 
"     float4 ProjPos  : POSITION;              "  // Projected space position 
"     float4 TexCoord: TEXCOORD0;             "  
"     float4 TexCoord1: TEXCOORD1;             "  
" };                                           "  
"                                              "  
" VS_OUT main( VS_IN In )                      "  
" {                                            "  
"     VS_OUT Out;                              "  
"     Out.ProjPos = mul( matWVP, In.ObjPos );  "  // Transform vertex into"     
"     float4 texMul = float4( In.TexCoord0.x, In.TexCoord0.y, 0, 1 ); "
"     Out.TexCoord.xy = mul(texMul, matTexture).xy;  "
"     Out.TexCoord.zw = 0;  "
"     Out.TexCoord1.xy = In.TexCoord1;          "
"     Out.TexCoord1.zw = 0;  "
"     return Out;                              "  // Transfer color
" }                                            ";


// Solving a quadratic equation
//  [-b +/- sqrt(b^2 - 4ac)]
//   ----------------------
//          2a
// Where A = 1- S^2 - V^2
// and   B = 2RS + 2UV
// and   C = R^2 + U^2
// 
// Where S = (CenterX - GradientOriginX) / RadiusX
// and   V = (CenterY - GradientOriginY) / RadiusY
// and   R = (X - GradientOriginX) / RadiusX
// and   U = (Y - GradientOriginY) / RadiusY
//
// and   X and Y are current texture location.
// Complicated, I know.
// TODO - Optimization. SV can be precalculated. A can be precalculated.
// 1 / 2A can be precalculated. Constants can be packed.
// Goes from 36 down to about ~20 instructions. TBD.
const char* ClientRadialGradientBrush::m_szPixelShaderProgram = 
" sampler   baseTexture : register(s0);                              "
" sampler   modTexture  : register(s1);                              "
" float4    color : register(c0);                                    "
" float2    gradientOrigin : register(c1);                           "
" float2    center : register(c2);                                  "
" float2    radius : register(c3);                                  "
"                                                                    "
" struct PS_IN                                                       "
" {                                                                  "
"     float4 TexCoord : TEXCOORD0;                                   "
"     float4 TexCoord1 : TEXCOORD1;                                   "
" };                                                                 "
"                                                                    "
" float4 main( PS_IN In ) : COLOR                                    "
" {                                                                  "
"   float2 SV = (center - gradientOrigin) / radius;                  "
"   float2 RU = (In.TexCoord.xy - gradientOrigin) / radius;          "
"   float  a = 1 - SV.x * SV.x - SV.y * SV.y;                        "
"   float  b = 2 * ( RU.x * SV.x + RU.y * SV.y);                     "
"   float  c = -1 * (RU.x * RU.x + RU.y * RU.y);                     "
"   float dist = 1;                                                  "
"   float  denom = b * b - 4 * a * c;                                "
"   if(denom > 0 && a != 0)                                          "
"   {                                                                "
"    float distPos = (-b + sqrt(denom)) / (2 * a);                   "
"     float distNeg = (-b - sqrt(denom)) / (2 * a);                  "
"                                                                    "
"     dist = max(max(distPos, distNeg), 0);                          "
"   }                                                                "
"                                                                    "
"   float4 firstTexel = tex2D( baseTexture, float2(dist, 0) );       "
"   float4 secondTexel = tex2D( modTexture, In.TexCoord1.xy );        "
"                                                                    "
"   float4 outputColor = color * firstTexel * secondTexel;    "  // Output color
"   return outputColor * float4(outputColor.w, outputColor.w, outputColor.w, 1.0); "
" };  ";

/*
" sampler   baseTexture : register(s0);        "
" sampler   modTexture  : register(s1);        "
" float4    color : register(c8);              "  
" float2    gradientOrigin : register(c9);     "  
" float2    center : register(c10);            "  
" float2    radius : register(c11);            "  
"                                              "
" struct PS_IN                                 "
" {                                            "
"     float4 TexCoord : TEXCOORD0;             "  
" };                                           "  // the vertex shader
"                                              "  
" float4 main( PS_IN In ) : COLOR              "  
" {                                            "
"   float2 SV = (center - gradientOrigin) / radius; "
"   float2 RU = (In.TexCoord.xy - gradientOrigin) / radius; "
"   float  a = 1 - SV.x * SV.x - SV.y * SV.y;     "
"   float  b = 2 * ( RU.x * SV.x + RU.y * SV.y);  "
"   float  c = -1 * (RU.x * RU.x + RU.y * RU.y);  "
"   float  denom = b * b - 4 * a * c;             "
"   float distPos = (-b + sqrt(denom)) / (2 * a); "
"   float distNeg = (-b - sqrt(denom)) / (2 * a); "
"                                                 "
"   float4 firstTexel = tex2D( baseTexture, float2(distPos, 0.5) ); "
"   float4 secondTexel = tex2D( modTexture, In.TexCoord.zw ); "

"   return color * firstTexel * secondTexel; "
" }                                            "; 
*/

IDirect3DPixelShader9* ClientRadialGradientBrush::m_pPixelShader;
IDirect3DVertexShader9* ClientRadialGradientBrush::m_pVertexShader;

void ClientRadialGradientBrush::SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::RadialGradientBrush* pBrush, float opacityCurrent)
{
    pDevice->SetVertexShader( m_pVertexShader );
    pDevice->SetPixelShader( m_pPixelShader );


    D3DXVECTOR4 gradientOrigin(m_xGradientOrigin, m_yGradientOrigin, 0.0f, 0.0f);
    D3DXVECTOR4 center(m_xCenter, m_yCenter, 0.0f, 0.0f);
    D3DXVECTOR4 radius(m_radiusX, m_radiusY, 0.0f, 0.0f);


    D3DXCOLOR colorOpacity;
    colorOpacity.r = colorOpacity.g = colorOpacity.b = 1.0f;
    colorOpacity.a = opacityCurrent;

    SetSpreadMethodRenderState(pBrush->get_SpreadMethod());


    D3DXMATRIX textureMatrix;
    AVUI::Transform* pTransform = pBrush->get_RelativeTransform();

    if(pTransform != NULL)
    {
        AVUI::Matrix mat = pTransform->get_Value();


        D3DXMATRIX value = D3DXMATRIX(mat.GetM11(), mat.GetM12(), 0.0f, 0.0f,
                             mat.GetM21(), mat.GetM22(), 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f,
                             mat.get_OffsetX(), mat.get_OffsetY(), 0.0f, 1.0f);

        D3DXMatrixInverse(&textureMatrix, NULL, &value);
    }
    else
    {
        D3DXMatrixIdentity(&textureMatrix);
    }

    D3DXMATRIX textureMatrixT;

    D3DXMatrixTranspose(&textureMatrixT, &textureMatrix);
    pDevice->SetVertexShaderConstantF(4, (FLOAT*)&textureMatrixT, 4 );



    pDevice->SetPixelShaderConstantF(0, (FLOAT*)&colorOpacity, 1);
    pDevice->SetPixelShaderConstantF(1, (FLOAT*)&gradientOrigin, 1);
    pDevice->SetPixelShaderConstantF(2, (FLOAT*)&center, 1);
    pDevice->SetPixelShaderConstantF(3, (FLOAT*)&radius, 1);

    pDevice->SetTexture(0, m_pTexture);

}

const char* ClientImageBrush::m_szVertexShaderProgram = 
" float4x4 matWVP : register(c0);              "  
" float4x4 matTexture : register(c4);          "  
"                                              "  
" struct VS_IN                                 "  
" {                                            " 
"     float4 ObjPos   : POSITION;              "  // Object space position 
"     float2 TexCoord0: TEXCOORD0;             "  
"     float2 TexCoord1: TEXCOORD1;             "  
" };                                           " 
"                                              " 
" struct VS_OUT                                " 
" {                                            " 
"     float4 ProjPos  : POSITION;              "  // Projected space position 
"     float4 TexCoord: TEXCOORD0;             "  
"     float4 TexCoord1: TEXCOORD1;             "  
" };                                           "  
"                                              "  
" VS_OUT main( VS_IN In )                      "  
" {                                            "  
"     VS_OUT Out;                              "  
"     Out.ProjPos = mul( matWVP, In.ObjPos );  "  // Transform vertex into
"     float4 texMul = float4( In.TexCoord0.x, In.TexCoord0.y, 0, 1 ); "
"     Out.TexCoord.xy = mul(texMul, matTexture).xy;  "
"     Out.TexCoord.zw = 0;           "
"     Out.TexCoord1.xy = In.TexCoord1;          "
"     Out.TexCoord1.zw = 0;          "
"     return Out;                              "  // Transfer color
" }                                            ";
 
const char* ClientImageBrush::m_szPixelShaderProgram = 
" sampler   baseTexture : register(s0);        "
" sampler   modTexture  : register(s1);        "
" float4    color : register(c0);              "
" struct PS_IN                                 "
" {                                            "
"     float4 TexCoord : TEXCOORD0;            "  
"     float4 TexCoord1 : TEXCOORD1;            "  
" };                                           "  // the vertex shader
"                                              "  
" float4 main( PS_IN In ) : COLOR              "  
" {                                            "
"   float4 firstTexel = tex2D( baseTexture, In.TexCoord.xy ); "
"   float4 secondTexel = tex2D( modTexture, In.TexCoord1.xy ); "
"   float4 outputColor = color * firstTexel * secondTexel;    "  // Output color
"   return outputColor * float4(outputColor.w, outputColor.w, outputColor.w, 1.0);"
" }                                            "; 

IDirect3DPixelShader9* ClientImageBrush::m_pPixelShader;
IDirect3DVertexShader9* ClientImageBrush::m_pVertexShader;

void ClientImageBrush::SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::ImageBrush* pBrush, float opacityCurrent, BitmapSourceData* pSourceData)
{
    pDevice->SetVertexShader( m_pVertexShader );
    pDevice->SetPixelShader( m_pPixelShader );

    D3DXCOLOR colorOpacity;
    colorOpacity.r = colorOpacity.g = colorOpacity.b = 1.0f;
    colorOpacity.a = opacityCurrent;

    D3DXMATRIX textureMatrix;
    AVUI::Transform* pTransform = pBrush->get_RelativeTransform();

    if(pTransform != NULL)
    {
        AVUI::Matrix mat = pTransform->get_Value();


        D3DXMATRIX value = D3DXMATRIX(mat.GetM11(), mat.GetM12(), 0.0f, 0.0f,
                             mat.GetM21(), mat.GetM22(), 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f,
                             mat.get_OffsetX(), mat.get_OffsetY(), 0.0f, 1.0f);

        D3DXMatrixInverse(&textureMatrix, NULL, &value);
    }
    else
    {
        D3DXMatrixIdentity(&textureMatrix);
    }

    D3DXMATRIX textureMatrixT;

    D3DXMatrixTranspose(&textureMatrixT, &textureMatrix);
    pDevice->SetVertexShaderConstantF(4, (FLOAT*)&textureMatrixT, 4 );

    pDevice->SetPixelShaderConstantF(0, (FLOAT*)&colorOpacity, 1);

    pDevice->SetTexture(0, pSourceData->get_Texture());

    SetTileModeRenderState(pDevice, pBrush->get_TileMode());
}

void ClientTileBrush::SetTileModeRenderState(IDirect3DDevice9* pDevice, AVUI::TileMode::Enum tileMode)
{
    switch(tileMode)
    {
    case AVUI::TileMode::None:
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
        break;
    case AVUI::TileMode::FlipX:
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        break;
    case AVUI::TileMode::FlipY:
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
        break;
    case AVUI::TileMode::FlipXY:
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
        break;
    case AVUI::TileMode::Tile:
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        break;
    }
}


const char* ClientVisualBrush::m_szVertexShaderProgram = 
" float4x4 matWVP : register(c0);              "  
" float4x4 matTexture : register(c4);          "  
"                                              "  
" struct VS_IN                                 "  
" {                                            " 
"     float4 ObjPos   : POSITION;              "  // Object space position 
"     float2 TexCoord0: TEXCOORD0;             "  
"     float2 TexCoord1: TEXCOORD1;             "  
" };                                           " 
"                                              " 
" struct VS_OUT                                " 
" {                                            " 
"     float4 ProjPos  : POSITION;              "  // Projected space position 
"     float4 TexCoord: TEXCOORD0;             "  
"     float4 TexCoord1: TEXCOORD1;             "  
" };                                           "  
"                                              "  
" VS_OUT main( VS_IN In )                      "  
" {                                            "  
"     VS_OUT Out;                              "  
"     Out.ProjPos = mul( matWVP, In.ObjPos );  "  // Transform vertex into
"     float4 texMul = float4( In.TexCoord0.x, In.TexCoord0.y, 0, 1 ); "
"     Out.TexCoord.xy = mul(texMul, matTexture).xy;  "
"     Out.TexCoord.zw = 0;  "
"     Out.TexCoord1.xy = In.TexCoord1;          "
"     Out.TexCoord1.zw = 0;  "
"     return Out;                              "  // Transfer color
" }                                            ";
 
const char* ClientVisualBrush::m_szPixelShaderProgram = 
" sampler   baseTexture : register(s0);        "
" sampler   modTexture  : register(s1);        "
" float4    color : register(c0);              "  
" struct PS_IN                                 "
" {                                            "
"     float4 TexCoord : TEXCOORD0;            "  
"     float4 TexCoord1 : TEXCOORD1;            "  
" };                                           "  // the vertex shader
"                                              "  
" float4 main( PS_IN In ) : COLOR              "  
" {                                            "
"   float4 firstTexel = tex2D( baseTexture, In.TexCoord.xy ); "
"   float4 secondTexel = tex2D( modTexture, In.TexCoord1.xy ); "
"   float4 outputColor = color;  "  // Output color
"   outputColor = outputColor * float4(outputColor.w, outputColor.w, outputColor.w, 1.0); "
"   return outputColor * firstTexel * secondTexel;           "
" }                                            "; 


IDirect3DPixelShader9* ClientVisualBrush::m_pPixelShader;
IDirect3DVertexShader9* ClientVisualBrush::m_pVertexShader;

void ClientVisualBrush::SetRenderStates(IDirect3DDevice9* pDevice, const AVUI::VisualBrush* pBrush, IDirect3DTexture9* pTexture, float opacityCurrent)
{
    pDevice->SetVertexShader( m_pVertexShader );
    pDevice->SetPixelShader( m_pPixelShader );

    D3DXCOLOR colorOpacity;
    colorOpacity.r = colorOpacity.g = colorOpacity.b = 1.0f;
    colorOpacity.a = opacityCurrent;

    D3DXMATRIX textureMatrix;

    AVUI::Transform* pTransform = NULL;

    if(pBrush != NULL)
    {
        pTransform = pBrush->get_RelativeTransform();
    }

    if(pTransform != NULL)
    {
        AVUI::Matrix mat = pTransform->get_Value();


        D3DXMATRIX value = D3DXMATRIX(mat.GetM11(), mat.GetM12(), 0.0f, 0.0f,
                             mat.GetM21(), mat.GetM22(), 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f,
                             mat.get_OffsetX(), mat.get_OffsetY(), 0.0f, 1.0f);

        D3DXMatrixInverse(&textureMatrix, NULL, &value);
    }
    else
    {
        D3DXMatrixIdentity(&textureMatrix);
    }

    D3DXMATRIX textureMatrixT;

    D3DXMatrixTranspose(&textureMatrixT, &textureMatrix);
    pDevice->SetVertexShaderConstantF(4, (FLOAT*)&textureMatrixT, 4 );

    pDevice->SetPixelShaderConstantF(0, (FLOAT*)&colorOpacity, 1);

    pDevice->SetTexture(0, pTexture);

    if(pBrush != NULL)
    {
        SetTileModeRenderState(pDevice, pBrush->get_TileMode());
    }
}

