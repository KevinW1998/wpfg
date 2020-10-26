#pragma once

#include <AVUIDependencyObject.h>
#include <AVUIMatrix.h>

namespace AVUI {

class Brush;
class RenderData;
class GeometryBuilder;
class Renderer;
class Transform;
class Viewport2DVisual3D;

class HitTestFilterBehavior
{
public:
    enum Enum
    {
        ContinueSkipSelfAndChildren = 0,
        ContinueSkipChildren = 2,
        ContinueSkipSelf = 4,
        Continue = 6,
        Stop = 8
    };
};

class HitTestResultBehavior
{
public:
    enum Enum
    {
        Stop,
        Continue
    };
};


class HitTestResultArgs : public Object
{
public:
    DECLARE_ELEMENT(HitTestResultArgs, Object);

    HitTestResultBehavior::Enum ResultBehavior;    
};

class HitTestFilterArgs : public Object
{
public:
    DECLARE_ELEMENT(HitTestFilterArgs, Object);

    HitTestFilterBehavior::Enum FilterBehavior;
};

typedef AVUI::TDelegate<HitTestResultArgs> HitTestResultCallback;
typedef AVUI::TDelegate<HitTestFilterArgs> HitTestFilterCallback;


class HitTestResult : public Object
{
public:
    HitTestResult(DependencyObject* pVisualHit) { m_pVisualHit = pVisualHit; }

    DECLARE_ELEMENT(HitTestResult, Object);

    DependencyObject* get_VisualHit() { return m_pVisualHit; }

private:

    DependencyObject* m_pVisualHit;
};

class PointHitTestResult : public HitTestResult
{
public:
    PointHitTestResult(DependencyObject* pVisualHit, const Point& ptHit) : HitTestResult(pVisualHit), m_pointHit(ptHit) { }

    DECLARE_ELEMENT(PointHitTestResult, HitTestResult);

    Point get_PointHit() { return m_pointHit; }

private:
    Point m_pointHit;
};


class TopMostHitResult
{
public:

    // Methods
    TopMostHitResult() { }

    void HitTestResultCheck(Object* pResult, HitTestResultArgs* pArgs)
    {
        m_pHitResult = (HitTestResult*) pResult;
        pArgs->ResultBehavior = HitTestResultBehavior::Stop;
    }

    void NoNested2DFilter(Object* pPotentialHitTestTarget, HitTestFilterArgs* pArgs)
    {
        if(is_type<Viewport2DVisual3D>(pPotentialHitTestTarget))
        {
            pArgs->FilterBehavior = HitTestFilterBehavior::ContinueSkipChildren;            
        }
        else
        {
            pArgs->FilterBehavior = HitTestFilterBehavior::Continue;
        }
    }

    HitTestResult* get_Result() { return m_pHitResult; }

private:

    TRefCountedPtr<HitTestResult> m_pHitResult;
};

class HitTestParameters : public Object
{
public:
    DECLARE_ELEMENT(HitTestParameters, Object);
};

class PointHitTestParameters : public HitTestParameters
{
    friend class Visual;
public:
    DECLARE_ELEMENT(PointHitTestParameters, HitTestParameters);

    PointHitTestParameters(const Point& point) { m_hitPoint = point; }

    Point get_HitPoint() { return m_hitPoint; }

private:
    void SetHitPoint(const Point& hitPoint) { m_hitPoint = hitPoint; }

    Point m_hitPoint;
};


class Visual : public DependencyObject
{
    friend class VisualDrawingContext;
    friend class VisualBrush;
    friend class UIElement;
    friend class Window;
    friend class PopupRoot;
    friend class Application;
    friend class WindowManager;
    friend class LayoutManager;
    friend class UIElementCollection;
    friend class XamlParser;
public:
    Visual();
    virtual ~Visual();

    DECLARE_ELEMENT(Visual, DependencyObject);

    virtual unsigned int get_VisualChildCount() { return 0; }
    virtual Visual* get_VisualChild(unsigned int) { LibraryCriticalError(); return NULL; };

    void BuildGeometry(GeometryBuilder* pGeometryBuilder);

    void RenderDependencies(Renderer* pRenderer, const Matrix& parentMatrix, const Rect& clipRect);

    void Render(Renderer* pRenderer, const Matrix& parentMatrix, const Rect& clipRect);
    void RenderCore(Renderer* pRenderer, const Matrix& localMatrix, Rect localClipRect);

    Visual* get_VisualParent() { return m_pVisualParent; }

    const Vector& get_VisualOffset() const { return m_offset; }
    void set_VisualOffset(const Vector& offset);

    Transform* get_VisualTransform();
    void set_VisualTransform(Transform* pTransform);

    float get_VisualOpacity();
    void set_VisualOpacity(float opacity);

    Rect get_VisualClip();
    void set_VisualClip(const Rect& visualClip);

    Brush* get_VisualOpacityMask();
    void set_VisualOpacityMask(Brush* pOpacityMask);

    void set_HasVisualClip(bool hasVisualClip) { WriteFlag(VisualFlags::HasVisualClip, hasVisualClip); }
    bool get_HasVisualClip() const { return ReadFlag(VisualFlags::HasVisualClip); }

    void set_IsViewport3D(bool isViewport3D) { WriteFlag(VisualFlags::IsViewport3D, isViewport3D); }
    bool get_IsViewport3D() const { return ReadFlag(VisualFlags::IsViewport3D); }

    void set_IsMediaElement(bool isMediaElement) { WriteFlag(VisualFlags::IsMediaElement, isMediaElement); }
    bool get_IsMediaElement() const { return ReadFlag(VisualFlags::IsMediaElement); }

    Rect get_SubgraphBounds() const { return m_bboxSubgraph; };

    void ClearVisualClip();

    virtual TRefCountedPtr<HitTestResult> HitTest(const Point& point, bool include2DOn3D = true);
    void HitTest(HitTestFilterCallback* pFilterCallback, HitTestResultCallback* pResultCallback, HitTestParameters* pHitTestParameters);
    HitTestResultBehavior::Enum HitTestPoint(HitTestFilterCallback* pFilterCallback, HitTestResultCallback* pResultCallback, PointHitTestParameters* pPointParams);

    virtual HitTestResultBehavior::Enum HitTestPointInternal(HitTestFilterCallback* pFilterCallback, HitTestResultCallback* pResultCallback, PointHitTestParameters* pHitTestParameters);
 
    bool IsDescendantOf(Visual* pVisualParent);
    bool IsAncestorOf(Visual* pVisualParent);
    Visual* get_RootVisual();

    Matrix TransformToAncestor(Visual* pAncestor) { return InternalTransformToAncestor(pAncestor, false); }
    Matrix TransformToDescendant(Visual* pDescendant) { return pDescendant->InternalTransformToAncestor(this, true); }

    void InvalidateBbox();
    void InvalidateGeometry(bool isSubtreeInvalid);
    void InvalidateSubtreeGeometry();
    void InvalidateRect(const Rect& rect);
    static float CalculateGeometryScale(const Matrix& mat);

    unsigned short GetTreeLevel() const { return m_treeLevel; }

protected:

    void AddVisualChild(Visual* pChild);
    void RemoveVisualChild(Visual* pChild);
 
    virtual void OnVisualParentChanged(DependencyObject* pOldParent) { }
    virtual void OnVisualChildrenChanged(DependencyObject* pChildAdded, DependencyObject* pChildRemoved) { }

    void ReleaseRenderData();

    virtual TRefCountedPtr<HitTestResult> HitTestCore(PointHitTestParameters* pHitTestParameters);

private:

    // Should this just be a bitfield struct?
    class VisualFlags
    {
    public:
        enum Enum
        {
            None =               0x0,
            HasVisualTransform = 0x1,
            HasVisualClip  =     0x2,
            HasBboxSubgraph =  0x4,
            HasVisualOpacity = 0x8,
            IsGeometryValid =       0x10,
            IsOpacitySuppressed =  0x20,
            IsCollapsed =       0x40,
            IsViewport3D =      0x80,
            HasOpacityMask    = 0x100,
            IsMediaElement    = 0x200,
            IsVisualBrushRoot = 0x400,
            IsLayoutSuspended= 0x800,
            IsAttachedToPresentationSource = 0x1000,
            Unused1 = 0x2000,
            IsSubtreeDirtyForPrecompute = 0x4000,
            IsRenderValid = 0x8000,
            HasChildren   = 0x10000
        };
    };

    virtual void FireOnVisualParentChanged(DependencyObject* pOldParent);


    void SetTreeLevel(unsigned short treeLevel);
    void ClearRenderDataClientGeometry();


    void RecalculateBboxSubgraph();
    Rect ChildToParent(const Rect& rect);
    Point ParentToChild(const Point& point);
    Rect ParentToChild(const Rect& rect);


    Matrix InternalTransformToAncestor(Visual* pAncestor, bool invertMatrix);
    void set_RenderData(RenderData* pRenderData);
    RenderData* get_RenderData() { return m_pRenderData; }

    static UncommonField<Object> VisualOpacityField;  
    static UncommonField<Transform>      VisualTransformField;  
    static UncommonField<Object> VisualClipField;  
    static UncommonField<Brush> OpacityMaskField;  

    Visual* m_pVisualParent;
    TRefCountedPtr<RenderData> m_pRenderData;
    Vector m_offset;
    Rect m_bboxSubgraph;


    void set_HasVisualTransform(bool hasVisualTransform) { WriteFlag(VisualFlags::HasVisualTransform, hasVisualTransform); }
    bool get_HasVisualTransform() const { return ReadFlag(VisualFlags::HasVisualTransform); }

    void set_HasBboxSubgraph(bool hasBboxSubgraph) { WriteFlag(VisualFlags::HasBboxSubgraph, hasBboxSubgraph); }
    bool get_HasBboxSubgraph() const { return ReadFlag(VisualFlags::HasBboxSubgraph); }

    void set_HasVisualOpacity(bool hasVisualOpacity) { WriteFlag(VisualFlags::HasVisualOpacity, hasVisualOpacity); }
    bool get_HasVisualOpacity() const { return ReadFlag(VisualFlags::HasVisualOpacity); }

    void set_IsGeometryValid(bool isGeometryValid) { WriteFlag(VisualFlags::IsGeometryValid, isGeometryValid); }
    bool get_IsGeometryValid() const { return ReadFlag(VisualFlags::IsGeometryValid); }

    void set_HasOpacityMask(bool hasOpacityMask) { WriteFlag(VisualFlags::HasOpacityMask, hasOpacityMask); }
    bool get_HasOpacityMask() const { return ReadFlag(VisualFlags::HasOpacityMask); }

    void set_IsOpacitySuppressed(bool isOpacitySuppressed) { WriteFlag(VisualFlags::IsOpacitySuppressed, isOpacitySuppressed); }
    bool get_IsOpacitySuppressed() const { return ReadFlag(VisualFlags::IsOpacitySuppressed); }

    void set_IsCollapsed(bool isCollapsed) { WriteFlag(VisualFlags::IsCollapsed, isCollapsed); }
    bool get_IsCollapsed() const { return ReadFlag(VisualFlags::IsCollapsed); }

    void set_IsAttachedToPresentationSource(bool isAttachedToPresentationSource) { WriteFlag(VisualFlags::IsAttachedToPresentationSource, isAttachedToPresentationSource); }
    bool get_IsAttachedToPresentationSource() const { return ReadFlag(VisualFlags::IsAttachedToPresentationSource); }

    void set_IsVisualBrushRoot(bool isVisualBrushRoot) { WriteFlag(VisualFlags::IsVisualBrushRoot, isVisualBrushRoot); }
    bool get_IsVisualBrushRoot() const { return ReadFlag(VisualFlags::IsVisualBrushRoot); }

    void WriteFlag(unsigned int flag, bool value) { m_visualFlags = (unsigned short) (value ? m_visualFlags | flag : m_visualFlags & ~flag); }
    bool ReadFlag(unsigned int flag) const { return (m_visualFlags & flag) != 0; }

    bool CheckFlagsAnd(unsigned int flags)
    {
        return ((m_visualFlags & flags) == flags);
    }
    void SetFlags(bool value, unsigned int flags)
    {
        m_visualFlags = (unsigned short) (value ? (m_visualFlags | flags) : (m_visualFlags & ~flags));
    }

    static void PropagateFlags(Visual* pVisual, VisualFlags::Enum value);
    void UpdateIsAttachedToPresentationSource(bool value);
    void Precompute();

 


    unsigned short m_visualFlags;
    unsigned short m_treeLevel;    
};


}; // namespace AVUI
