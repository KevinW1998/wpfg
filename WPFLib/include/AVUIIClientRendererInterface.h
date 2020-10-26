#pragma once

#include <AVUICommon.h>

// Base class for all brushes.

namespace AVUI {

class SolidColorBrush;
class LinearGradientBrush;
class RadialGradientBrush;
class ImageBrush;
class VisualBrush;
class Brush;

class IClientResource
{
public:
    virtual void Destroy() = 0;

protected:
    virtual ~IClientResource() { }
};

class IClientGradientBrush : public IClientResource
{
public:
    virtual void BeginGradientUpdate(unsigned int cGradientStops) = 0;
    virtual void AddGradientStop(float offset, unsigned int argb) = 0;
    virtual void EndGradientUpdate() = 0;
};


class IClientLinearGradientBrush : public IClientGradientBrush
{
protected:
    virtual ~IClientLinearGradientBrush() { };
};

class IClientSubgraphCapture : public IClientResource
{
protected:
    virtual ~IClientSubgraphCapture() { };
    virtual const Rect& get_CapturedRect() = 0;
};


class IClientRadialGradientBrush : public IClientGradientBrush
{
public:
    virtual void set_Center(float x, float y) = 0;
    virtual void set_RadiusX(float radiusX) = 0;
    virtual void set_RadiusY(float radiusY) = 0;
    virtual void set_GradientOrigin(float x, float y) = 0;

    virtual void BeginGradientUpdate(unsigned int cGradientStops) = 0;
    virtual void AddGradientStop(float offset, unsigned int argb) = 0;
    virtual void EndGradientUpdate() = 0;

protected:
    virtual ~IClientRadialGradientBrush() { };
};


class IClientRenderGeometry : public IClientResource
{
public:
    virtual void BeginFigure() = 0;
    virtual void EndFigure() = 0;
    virtual void AddVertex(float xPosition, float yPosition, bool isAAVertex) = 0;
    virtual void AddVertex3D(float xPosition, float yPosition, float zPosition, float textureU, float textureV) = 0;
    virtual void AddIndexTriplet(unsigned int index1, unsigned int index2, unsigned int index3) = 0;
    virtual void Seal() = 0;

protected:
    virtual ~IClientRenderGeometry() { };

};

class IClientFont;
class IClientFormattedText : public IClientResource
{
public:

    virtual bool get_HasEndOfText() const = 0;
    virtual float get_Width() const = 0;
    virtual float get_Height() const = 0;

    virtual float GetXLocationBeforeIndex(int index) const = 0;
    virtual unsigned int get_Cch() const = 0;

    virtual void Trim(float trimWidth) = 0;

protected:
    virtual ~IClientFormattedText() { };
};

class IClientTextGeometry : public IClientResource
{
public:

    virtual void AddFormattedText(IClientFormattedText* pText, float originX, float originY) = 0;
    virtual void Seal() = 0;

protected:
    virtual ~IClientTextGeometry() { }
};

class IClientFont : public IClientResource
{
public:

    virtual IClientFormattedText* CreateFormattedText(const WCHAR* pwchText, unsigned int cchText, float availableWidth, bool wrapText) = 0;
    virtual float GetCharacterWidth(WCHAR wch) = 0;

protected:
    virtual ~IClientFont() { };
};

class IClientNavResult : public IClientResource
{
public:
    
    virtual ObjectPtr GetNavResult() = 0;
    virtual NavigationStatus::Enum GetNavStatus() = 0;
};

class IClientMediaElement : public IClientResource
{
public:
    virtual void set_Volume(float volume) = 0;
    /*
    virtual Duration get_NaturalDuration() = 0;
    virtual float get_NaturalVideoHeight() = 0;
    virtual float get_NaturalVideoWidth() = 0;
    virtual void Pause() = 0;
    virtual void Play() = 0;
    */
};

class IClientImageSource : public IClientResource
{
public:

    virtual float get_Height() = 0;
    virtual float get_Width() = 0;
};


class IClientCursor : public IClientResource
{
};


class IClientRendererInterface
{
public:
    virtual void BeginUIRender() = 0;

    virtual void BeginMainSurfaceRender() = 0;
    virtual void AddDirtyRects(const Rect* pRects, unsigned int cRects) = 0;
    virtual void EndMainSurfaceRender() = 0;

    virtual void EndUIRender() = 0;

    virtual void BeginSubgraphCapture(const Rect& rectVisibleSubgraph) = 0;
    virtual IClientSubgraphCapture* EndSubgraphCapture() = 0;

    virtual void Begin3DRender() = 0;
    virtual void SetCurrent3DTransform(const Matrix3D& wvpMatrix) = 0;
    virtual void End3DRender() = 0;

    virtual void OpacityMask(const Rect& rectSubgraph, IClientSubgraphCapture* pCapture) = 0;

    virtual IClientRenderGeometry* CreateGeometry() = 0;
    virtual IClientTextGeometry*  CreateTextGeometry() = 0;

    virtual IClientLinearGradientBrush* CreateLinearGradientBrush() = 0;
    virtual IClientImageSource* CreateImageSource(const WCHAR* wzImageName) = 0;
    virtual IClientRadialGradientBrush* CreateRadialGradientBrush() = 0;

    virtual IClientFont* CreateClientFont(const WCHAR* wzFontName, float pixelSize) = 0;

    virtual IClientMediaElement* CreateMediaElement(const WCHAR* wzElementName) = 0;

    virtual void set_Opacity(float opacity) = 0;

    virtual void PushClipRect(const AVUI::Rect& rect) = 0;
    virtual void PopClipRect() = 0;

    virtual void set_CurrentBrush(const SolidColorBrush* pBrush) = 0;
    virtual void set_CurrentBrush(const LinearGradientBrush* pBrush, IClientLinearGradientBrush* pClientBrush) = 0;
    virtual void set_CurrentBrush(const RadialGradientBrush* pBrush, IClientRadialGradientBrush* pClientBrush) = 0;
    virtual void set_CurrentBrush(const ImageBrush* pBrush, IClientImageSource* pClientSource) = 0;
    virtual void set_CurrentBrush(const VisualBrush* pBrush, IClientSubgraphCapture* pSubgraphCapture) = 0;

    virtual void RenderGeometry(IClientRenderGeometry* pGeometry, bool isStrokeGeometry) = 0;
    virtual void RenderGeometry3D(IClientRenderGeometry* pGeometry) = 0;
    virtual void RenderTextGeometry(IClientTextGeometry* pTextGeometry) = 0;
    virtual void RenderMediaElement(IClientMediaElement* pMediaElement, const Rect& rectArea) = 0;

    virtual void SetCurrentTransform(const Matrix& matrix) = 0;

    virtual void PlaySound(const WCHAR* wzSoundName) = 0;
    virtual void set_Cursor(unsigned int cursorType) = 0;

    virtual IClientNavResult* NavigateTo(const WCHAR* wzNavURI) = 0;

protected:
    virtual ~IClientRendererInterface() { };
};

};
