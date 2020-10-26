#include <AVUICommon.h>
#include <AVUIVisual.h>
#include <AVUIViewport3D.h>
#include <AVUIMediaElement.h>
#include <AVUIRenderData.h>
#include <AVUIGeometryBuilder.h>
#include <AVUITransform.h>
#include <AVUIRenderer.h>
#include <AVUIApplication.h>
#include <AVUIBrush.h>
#include <AVUIIClientRendererInterface.h>
#include <AVUIVisualTreeHelper.h>

REGISTER_ELEMENT(AVUI::HitTestResultArgs);
REGISTER_ELEMENT(AVUI::HitTestFilterArgs);
REGISTER_ELEMENT(AVUI::HitTestResult);
REGISTER_ELEMENT(AVUI::PointHitTestResult);
REGISTER_ELEMENT(AVUI::HitTestParameters);
REGISTER_ELEMENT(AVUI::PointHitTestParameters);

REGISTER_ELEMENT_BEGIN(AVUI::Visual)
    REGISTER_DEFAULT_FACTORY()
REGISTER_ELEMENT_END()

namespace AVUI {

UncommonField<Object> Visual::VisualOpacityField;  
UncommonField<Transform>      Visual::VisualTransformField;  
UncommonField<Object> Visual::VisualClipField;  
UncommonField<Brush>  Visual::OpacityMaskField;  


Visual::Visual() : m_pVisualParent(NULL), m_visualFlags(VisualFlags::None), m_treeLevel(-1)                   
{
}

Visual::~Visual()
{
    DependencyObject::ClearEffectiveValues();
}

void Visual::AddVisualChild(Visual* pChild)
{
    if (pChild != NULL)
    {
        if (pChild->m_pVisualParent != NULL)
        {
            LibraryCriticalErrorMessage("Child has visual parent.");
        }
        SetFlags(true, VisualFlags::HasChildren);

        pChild->m_pVisualParent = this;

        pChild->SetFlags(false, VisualFlags::IsRenderValid);

        PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
        pChild->SetFlags(false, VisualFlags::IsRenderValid);
        PropagateFlags(pChild, VisualFlags::IsSubtreeDirtyForPrecompute);

        pChild->UpdateIsAttachedToPresentationSource(get_IsAttachedToPresentationSource());
        pChild->InvalidateGeometry(true);

        UIElement::PropagateResumeLayout(this, pChild);
        OnVisualChildrenChanged(pChild, NULL);

        pChild->FireOnVisualParentChanged(NULL);
    }
}

void Visual::RemoveVisualChild(Visual* pChild)
{
    if (pChild != NULL && pChild->m_pVisualParent != NULL)
    {
        if (pChild->m_pVisualParent != this)
        {
            LibraryCriticalError();
        }
        if (get_VisualChildCount() == 0)
        {
            SetFlags(false, VisualFlags::HasChildren);
        }

        pChild->InvalidateBbox();

        pChild->m_pVisualParent = NULL;
        PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);

        pChild->UpdateIsAttachedToPresentationSource(false);

        UIElement::PropagateSuspendLayout(pChild);
        pChild->FireOnVisualParentChanged(this);
        OnVisualChildrenChanged(NULL, pChild);
    }
}

void Visual::FireOnVisualParentChanged(DependencyObject* pOldParent)
{
    OnVisualParentChanged(pOldParent);
    /*
    if (pOldParent == NULL)
    {
        if (this.CheckFlagsAnd(VisualFlags.None | VisualFlags.SubTreeHoldsAncestorChanged))
        {
            SetTreeBits(this._parent, VisualFlags.None | VisualFlags.SubTreeHoldsAncestorChanged, VisualFlags.None | VisualFlags.RegisteredForAncestorChanged);
        }
    }
    else if (this.CheckFlagsAnd(VisualFlags.None | VisualFlags.SubTreeHoldsAncestorChanged))
    {
        ClearTreeBits(oldParent, VisualFlags.None | VisualFlags.SubTreeHoldsAncestorChanged, VisualFlags.None | VisualFlags.RegisteredForAncestorChanged);
    }
    AncestorChangedEventArgs args = new AncestorChangedEventArgs(this, oldParent);
    ProcessAncestorChangedNotificationRecursive(this, args);
    */
}

 

 


void Visual::UpdateIsAttachedToPresentationSource(bool value)
{
    set_IsAttachedToPresentationSource(value);

    if(value == false)
    {
        ClearRenderDataClientGeometry();
        m_treeLevel = -1;
    }
    else
    {
        int newTreeLevel = 0;

        if(m_pVisualParent != NULL)
        {
            newTreeLevel = m_pVisualParent->GetTreeLevel() + 1;
        }
        SetTreeLevel(newTreeLevel);
    }

    unsigned int cChildren = get_VisualChildCount();

    for(unsigned int idx = 0; idx < cChildren; idx++)
    {
        get_VisualChild(idx)->UpdateIsAttachedToPresentationSource(value);
    }
}

void Visual::Precompute()
{
    if(!CheckFlagsAnd(VisualFlags::IsSubtreeDirtyForPrecompute))
    {
        return;
    }

    unsigned int cChildren = get_VisualChildCount();

    for(unsigned int idx = 0; idx < cChildren; idx++)
    {
        get_VisualChild(idx)->Precompute();
    }

    SetFlags(false, VisualFlags::IsSubtreeDirtyForPrecompute);

    RecalculateBboxSubgraph();
}

void Visual::SetTreeLevel(unsigned short treeLevel)
{
    if(m_treeLevel != treeLevel)
    {
        m_treeLevel = treeLevel;

        unsigned int cChildren = get_VisualChildCount();

        for(unsigned int idx = 0; idx < cChildren; idx++)
        {
            get_VisualChild(idx)->SetTreeLevel(treeLevel + 1);
        }
    }
}


void Visual::InvalidateGeometry(bool isSubtreeInvalid)
{
    if(isSubtreeInvalid)
    {
        InvalidateSubtreeGeometry();
    }
    else
    {
        set_IsGeometryValid(false);
    }

    Visual* pCurrent = get_VisualParent();

    while(pCurrent != NULL)
    {
        pCurrent->set_IsGeometryValid(false);
        pCurrent = pCurrent->get_VisualParent();
    }
}

void Visual::InvalidateSubtreeGeometry()
{
    set_IsGeometryValid(false);

    unsigned int cChildren = get_VisualChildCount();
    for(unsigned int idx = 0; idx < cChildren; idx++)
    {
        get_VisualChild(idx)->InvalidateSubtreeGeometry();
    }
}

void Visual::set_RenderData(RenderData* pRenderData)
{ 
    if(m_pRenderData == NULL && pRenderData == NULL)
    {
        return;
    }

    m_pRenderData = pRenderData; 

    InvalidateGeometry(false);
    InvalidateBbox();

    PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
}

void Visual::BuildGeometry(GeometryBuilder* pGeometryBuilder)
{
    if(!get_IsGeometryValid())
    {
        float originalScale = pGeometryBuilder->get_GeometryScale();
        float newScale = originalScale;

        if(get_HasVisualTransform())
        {
            newScale = CalculateGeometryScale(get_VisualTransform()->get_Value()) * originalScale;
            pGeometryBuilder->set_GeometryScale(newScale);
        } 

        if(m_pRenderData != NULL)
        {
            m_pRenderData->BuildGeometry(pGeometryBuilder);
        }

        set_IsGeometryValid(true);

        int childCount = get_VisualChildCount();
        for(int idx = 0; idx < childCount; idx++)
        {
            get_VisualChild(idx)->BuildGeometry(pGeometryBuilder);
        }

        if(newScale != originalScale)
        {
            pGeometryBuilder->set_GeometryScale(originalScale);
        }
    }
}

float Visual::CalculateGeometryScale(const Matrix& mat)
{
    float dist1 = mat.GetM11() * mat.GetM11() + mat.GetM12() * mat.GetM12();
    float dist2 = mat.GetM21() * mat.GetM21() + mat.GetM22() * mat.GetM22();

    return sqrtf(Float::Max(dist1, dist2));
}


void Visual::RenderDependencies(Renderer* pRenderer, const Matrix& parentMatrix, const Rect& clipRect)
{
    Matrix localMatrix = parentMatrix;

    Rect localClipRect = ParentToChild(clipRect);

    if(get_HasVisualTransform())
    {
        localMatrix = get_VisualTransform()->get_Value() * localMatrix;
    } 

    if(m_offset.get_X() != 0.0f || m_offset.get_Y() != 0.0f)
    {   
        localMatrix.set_OffsetX(localMatrix.get_OffsetX() + m_offset.get_X());
        localMatrix.set_OffsetY(localMatrix.get_OffsetY() + m_offset.get_Y());
    }

    if(!get_HasBboxSubgraph())
    {
        return;
    }

    if(!localClipRect.Intersects(m_bboxSubgraph))
    {
        return;
    }

    Rect bboxWorld = localMatrix.TransformBounds(m_bboxSubgraph);
    if(!Application::get_Current()->IsRectDirty(bboxWorld))
    {
        return;
    }

    Matrix opacityCachedMatrix;

    if(m_pRenderData != NULL && m_pRenderData->ContainsRenderDependencies())
    {
        m_pRenderData->RenderDependencies(pRenderer);
    }

    if(get_IsViewport3D())
    {
        ((Viewport3D*)this)->RenderDependencies3D(pRenderer);
    }
    else
    {
        int childCount = get_VisualChildCount();
        for(int idx = 0; idx < childCount; idx++)
        {
            get_VisualChild(idx)->RenderDependencies(pRenderer, localMatrix, localClipRect);
        }
    }

    if(get_HasOpacityMask() && !pRenderer->GetSubgraphCapture(this))
    {
        pRenderer->set_CurrentOpacity(1.0f);
        pRenderer->get_ClientRendererInterface()->BeginSubgraphCapture(m_bboxSubgraph);

        localMatrix = Matrix::CreateTranslation(-m_bboxSubgraph.get_Left(), -m_bboxSubgraph.get_Top());

        localClipRect = Rect(Point(0.0f, 0.0f), Size(m_bboxSubgraph.get_Width(), m_bboxSubgraph.get_Height()));

        RenderCore(pRenderer, localMatrix, localClipRect);

        IClientSubgraphCapture* pCapture = pRenderer->get_ClientRendererInterface()->EndSubgraphCapture();

        pRenderer->RegisterSubgraphCapture(this, pCapture);
    }
}



void Visual::Render(Renderer* pRenderer, const Matrix& parentMatrix, const Rect& clipRect)
{
    Matrix localMatrix = parentMatrix;

    Rect localClipRect = ParentToChild(clipRect);

    if(get_HasVisualTransform())
    {
        localMatrix = get_VisualTransform()->get_Value() * localMatrix;
    } 

    if(m_offset.get_X() != 0.0f || m_offset.get_Y() != 0.0f)
    {   
        localMatrix.set_OffsetX(localMatrix.get_OffsetX() + m_offset.get_X());
        localMatrix.set_OffsetY(localMatrix.get_OffsetY() + m_offset.get_Y());
    }

    if(!get_HasBboxSubgraph())
    {
        return;
    }

    if(!localClipRect.Intersects(m_bboxSubgraph))
    {
        return;
    }

    Rect bboxWorld = localMatrix.TransformBounds(m_bboxSubgraph);
    if(!Application::get_Current()->IsRectDirty(bboxWorld))
    {
        return;
    }

    float oldOpacity = 0.0f;
    
    if(get_HasVisualOpacity())
    {
        oldOpacity = pRenderer->get_CurrentOpacity();
        pRenderer->set_CurrentOpacity(get_VisualOpacity() * oldOpacity);
    }

    Matrix opacityCachedMatrix;

    if(get_HasOpacityMask())
    {
        IClientSubgraphCapture* pCapture = pRenderer->GetSubgraphCapture(this);

        get_VisualOpacityMask()->set_ActiveBrush(pRenderer);

        localMatrix.set_OffsetX(localMatrix.get_OffsetX() + m_bboxSubgraph.get_Left());
        localMatrix.set_OffsetY(localMatrix.get_OffsetY() + m_bboxSubgraph.get_Top());

        pRenderer->get_ClientRendererInterface()->SetCurrentTransform(localMatrix);
        pRenderer->get_ClientRendererInterface()->OpacityMask(m_bboxSubgraph, pCapture);
    }
    else
    {
        RenderCore(pRenderer, localMatrix, localClipRect);
    }

    if(get_HasVisualOpacity())
    {
        pRenderer->set_CurrentOpacity(oldOpacity);
    }

    SetFlags(true, VisualFlags::IsRenderValid);

}

void Visual::RenderCore(Renderer* pRenderer, const Matrix& localMatrix, Rect localClipRect)
{
    if(get_HasVisualClip())
    {
        localClipRect = get_VisualClip();
        Rect globalClipRect = localMatrix.TransformBounds(localClipRect);

        pRenderer->get_ClientRendererInterface()->PushClipRect(globalClipRect);
    }

    if(m_pRenderData != NULL)
    {
        pRenderer->get_ClientRendererInterface()->SetCurrentTransform(localMatrix);
        m_pRenderData->Render(pRenderer);
    }

    if(get_IsViewport3D())
    {
        pRenderer->get_ClientRendererInterface()->SetCurrentTransform(localMatrix);
        ((Viewport3D*)this)->Render3D(pRenderer, localMatrix, localClipRect);
    }
    else if(get_IsMediaElement())
    {
        pRenderer->get_ClientRendererInterface()->SetCurrentTransform(localMatrix);
        ((MediaElement*)this)->RenderMedia(pRenderer);
    }
    else
    {
        int childCount = get_VisualChildCount();
        for(int idx = 0; idx < childCount; idx++)
        {
            get_VisualChild(idx)->Render(pRenderer, localMatrix, localClipRect);
        }
    }

    if(get_HasVisualClip())
    {
        pRenderer->get_ClientRendererInterface()->PopClipRect();
    }
}

TRefCountedPtr<HitTestResult> Visual::HitTest(const Point& point, bool include2DOn3D)
{
    TopMostHitResult result;
    PointHitTestParameters params(point);

    VisualTreeHelper::HitTest(this, 
                              include2DOn3D ? NULL : HitTestFilterCallback::Create(&result, &TopMostHitResult::NoNested2DFilter), 
                              HitTestResultCallback::Create(&result, &TopMostHitResult::HitTestResultCheck), 
                              &params);

    return result.get_Result();
}

void Visual::HitTest(HitTestFilterCallback* pFilterCallback, HitTestResultCallback* pResultCallback, HitTestParameters* pHitTestParameters)
{
    Precompute();

    PointHitTestParameters* pPointParams = object_cast<PointHitTestParameters>(pHitTestParameters);
    if (pPointParams != NULL)
    {
        Point hitPoint = pPointParams->get_HitPoint();
        HitTestPoint(pFilterCallback, pResultCallback, pPointParams);
        return;
    }
    /*
    GeometryHitTestParameters geometryParams = hitTestParameters as GeometryHitTestParameters;
    if (geometryParams != null)
    {
        try
        {
            this.HitTestGeometry(filterCallback, resultCallback, geometryParams);
            return;
        }
        catch
        {
            geometryParams.EmergencyRestoreOriginalTransform();
            throw;
        }
    }
    */
    LibraryCriticalError();
}

HitTestResultBehavior::Enum Visual::HitTestPoint(HitTestFilterCallback* pFilterCallback, HitTestResultCallback* pResultCallback, PointHitTestParameters* pPointParams)
{
    if (get_HasBboxSubgraph() && m_bboxSubgraph.Contains(pPointParams->get_HitPoint()) && ((!get_HasVisualClip()) || get_VisualClip().Contains(pPointParams->get_HitPoint())))
    {
        HitTestFilterBehavior::Enum behavior = HitTestFilterBehavior::Continue;
        if (pFilterCallback != NULL)
        {
            HitTestFilterArgs filterArgs;

            pFilterCallback->Invoke(this, &filterArgs);

            behavior = filterArgs.FilterBehavior;

            switch (behavior)
            {
            case HitTestFilterBehavior::ContinueSkipSelfAndChildren:
                return HitTestResultBehavior::Continue;

            case HitTestFilterBehavior::Stop:
                return HitTestResultBehavior::Stop;
            }
        }
        Point hitPoint = pPointParams->get_HitPoint();
        Point outPoint = hitPoint;
        /*
        if (this.CheckFlagsAnd(VisualFlags.NodeHasBitmapEffect) && !BitmapEffectStateField.GetValue(this).TransformHitPoint(this._bboxSubgraph, hitPoint, out outPoint))
        {
            return HitTestResultBehavior.Continue;
        }
        */
        if (behavior != HitTestFilterBehavior::ContinueSkipChildren)
        {
            for (int i = get_VisualChildCount() - 1; i >= 0; i--)
            {
                Visual* pVisualChild = get_VisualChild(i);
                if (pVisualChild != NULL)
                {
                    Point point = outPoint;
                    point = point - pVisualChild->m_offset;
                    Transform* pTransform = VisualTransformField.GetValue(pVisualChild);
                    if (pTransform != NULL)
                    {
                        Matrix matrix = pTransform->get_Value();
                        if (!matrix.HasInverse())
                        {
                            continue;
                        }
                        matrix.Invert();
                        point = matrix.Transform(point);
                    }

                    pPointParams->SetHitPoint(point);
                    HitTestResultBehavior::Enum behavior3 = pVisualChild->HitTestPoint(pFilterCallback, pResultCallback, pPointParams);
                    pPointParams->SetHitPoint(hitPoint);
                    if (behavior3 == HitTestResultBehavior::Stop)
                    {
                        return HitTestResultBehavior::Stop;
                    }
                }
            }
        }
        if (behavior != HitTestFilterBehavior::ContinueSkipSelf)
        {
            pPointParams->SetHitPoint(outPoint);
            HitTestResultBehavior::Enum behavior2 = HitTestPointInternal(pFilterCallback, pResultCallback, pPointParams);
            pPointParams->SetHitPoint(hitPoint);
            if (behavior2 == HitTestResultBehavior::Stop)
            {
                return HitTestResultBehavior::Stop;
            }
        }
    }
    return HitTestResultBehavior::Continue;
}

HitTestResultBehavior::Enum Visual::HitTestPointInternal(HitTestFilterCallback* pFilterCallback, HitTestResultCallback* pResultCallback, PointHitTestParameters* pHitTestParameters)
{
    TRefCountedPtr<HitTestResult> pResult = HitTestCore(pHitTestParameters);
    if (pResult != NULL)
    {
        HitTestResultArgs resultArgs;

        pResultCallback->Invoke(pResult, &resultArgs);

        return resultArgs.ResultBehavior;
    }
    return HitTestResultBehavior::Continue;
}
 
TRefCountedPtr<HitTestResult> Visual::HitTestCore(PointHitTestParameters* pHitTestParameters)
{
    if(!get_HasBboxSubgraph() || get_VisualOpacity() == 0.0f)
    {
        return NULL;
    }

    if (m_pRenderData != NULL && m_pRenderData->HitTestPoint(pHitTestParameters->get_HitPoint()))
    {
        return (HitTestResult*) object_allocate<PointHitTestResult>(this, pHitTestParameters->get_HitPoint());
    }
    return NULL;
}

bool Visual::IsAncestorOf(Visual* pVisualChild)
{
    return pVisualChild->IsDescendantOf(this);
}


bool Visual::IsDescendantOf(Visual* pVisualParent)
{
    Visual* pVisual = this;

    while(pVisual != NULL)
    {
        if(pVisual == pVisualParent)
        {
            return true;
        }
        pVisual = pVisual->get_VisualParent();
    }

    return false;
}

Visual* Visual::get_RootVisual()
{
    Visual* pVisual = this;

    while(pVisual->get_VisualParent() != NULL)
    {
        pVisual = pVisual->get_VisualParent();
    }
    return pVisual;
}

Matrix Visual::InternalTransformToAncestor(Visual* pAncestor, bool invertMatrix)
{
    Matrix matrixTransform = Matrix::get_Identity();
    Visual* pVisual = this;

    while(pVisual->get_VisualParent() != NULL && pVisual != pAncestor)
    {

        if(pVisual->get_HasVisualTransform())
        {
            matrixTransform = Matrix::MultiplyMatrix(matrixTransform, pVisual->get_VisualTransform()->get_Value());
        }

        matrixTransform.Translate(pVisual->m_offset.get_X(), pVisual->m_offset.get_Y());

        pVisual = pVisual->get_VisualParent();
    }

    if(invertMatrix)
    {
        if(!matrixTransform.HasInverse())
        {
            LibraryCriticalError();
        }
        matrixTransform.Invert();
    }

    return matrixTransform;
}

Transform* Visual::get_VisualTransform()
{
    if(get_HasVisualTransform())
    {
        return VisualTransformField.GetValue(this);
    }
    return NULL;
}

void Visual::set_VisualTransform(Transform* pTransform)
{
    bool isInvalid = false;

    if(get_HasVisualTransform() && !pTransform)
    {
        InvalidateBbox();
        set_HasVisualTransform(false);
        VisualTransformField.ClearValue(this);

        PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
        InvalidateGeometry(true);
    }
    else if(pTransform != NULL && pTransform != VisualTransformField.GetValue(this))
    {
        InvalidateBbox();

        set_HasVisualTransform(true);
        VisualTransformField.SetValue(this, pTransform);

        PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
        InvalidateGeometry(true);
    }
}

void Visual::set_VisualOpacityMask(Brush* pOpacityMask)
{
    if(get_HasOpacityMask() && !pOpacityMask)
    {
        set_HasOpacityMask(false);
        OpacityMaskField.ClearValue(this);
    }
    else if(pOpacityMask != NULL && pOpacityMask != OpacityMaskField.GetValue(this))
    {
        set_HasOpacityMask(true);
        OpacityMaskField.SetValue(this, pOpacityMask);
    }

    PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
}

Brush* Visual::get_VisualOpacityMask()
{
    if(get_HasOpacityMask())
    {
        return OpacityMaskField.GetValue(this);
    }

    return NULL;
}


float Visual::get_VisualOpacity()
{
    if(get_HasVisualOpacity())
    {
        return UnboxValue<float>(VisualOpacityField.GetValue(this));
    }
    return 1.0f;
}

void Visual::set_VisualOpacity(float opacity)
{
    InvalidateBbox();

    if(get_HasVisualOpacity() && opacity == 1.0f)
    {
        set_HasVisualOpacity(false);
        VisualOpacityField.ClearValue(this);
    }
    else if(opacity != 1.0f)
    {
        set_HasVisualOpacity(true);
        VisualOpacityField.SetValue(this, BoxValue(opacity));
    }

    PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
}

Rect Visual::get_VisualClip()
{
    if(get_HasVisualClip())
    {
        return UnboxValue<Rect>(VisualClipField.GetValue(this));
    }
    return Rect();
}

void Visual::set_VisualClip(const Rect& visualClip)
{
    InvalidateBbox();

    set_HasVisualClip(true);
    VisualClipField.SetValue(this, BoxValue(visualClip));

    PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
}

void Visual::ClearVisualClip()
{
    if(get_HasVisualClip())
    {
        InvalidateBbox();

        VisualClipField.ClearValue(this);
        set_HasVisualClip(false);

        PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
    }   
}

void Visual::RecalculateBboxSubgraph()
{
    bool hasBboxSubgraph = false;
    Rect bboxSubgraph;
    unsigned int childCount = get_VisualChildCount();
    

    for(unsigned int childIndex = 0; childIndex < childCount; childIndex++)
    {
        Visual* pChild = get_VisualChild(childIndex);

        if(!pChild->get_HasBboxSubgraph())
        {
            continue;
        }

        if(!hasBboxSubgraph)
        {
            hasBboxSubgraph = true;
            bboxSubgraph = pChild->ChildToParent(pChild->m_bboxSubgraph);
        }
        else
        {
            bboxSubgraph = Rect::Union(bboxSubgraph, pChild->ChildToParent(pChild->m_bboxSubgraph));
        }
    }

    if(m_pRenderData != NULL)
    {
        if(!hasBboxSubgraph)
        {
            bboxSubgraph = m_pRenderData->get_ContentBounds();
            hasBboxSubgraph = true;
        }
        else
        {
            bboxSubgraph = Rect::Union(bboxSubgraph, m_pRenderData->get_ContentBounds());
        }
    }

    if(get_HasBboxSubgraph() && get_HasVisualClip())
    {
        set_HasBboxSubgraph(Rect::Intersect(bboxSubgraph, get_VisualClip(), &bboxSubgraph));
    }

    if(get_HasBboxSubgraph() != hasBboxSubgraph)
    {
        set_HasBboxSubgraph(hasBboxSubgraph);
    }

    m_bboxSubgraph = bboxSubgraph;
}

Rect Visual::ChildToParent(const Rect& rect)
{
    if(get_HasVisualTransform())
    {
        Matrix mat = TransformToAncestor(m_pVisualParent);

        return mat.TransformBounds(rect);
    }

    Rect rectParent(rect);

    rectParent.set_X(rect.get_X() + m_offset.get_X());
    rectParent.set_Y(rect.get_Y() + m_offset.get_Y());

    return rectParent;
}

Point Visual::ParentToChild(const Point& pt)
{
    if(get_HasVisualTransform())
    {
        Matrix mat = m_pVisualParent->TransformToDescendant(this);

        return mat.Transform(pt);
    }

    return Point(pt.get_X() - m_offset.get_X(), pt.get_Y() - m_offset.get_Y());
}

Rect Visual::ParentToChild(const Rect& rect)
{
    if(get_HasVisualTransform())
    {
        Matrix mat = m_pVisualParent->TransformToDescendant(this);

        return mat.TransformBounds(rect);
    }
   
    return Rect(rect.get_X() - m_offset.get_X(), rect.get_Y() - m_offset.get_Y(), rect.get_Width(), rect.get_Height());
}

void Visual::InvalidateRect(const Rect& rect)
{
    Rect bboxInvalidate = rect;

    Visual* pVisual = this;
    while(pVisual != NULL)
    {
        if(!pVisual->CheckFlagsAnd(VisualFlags::IsRenderValid))
        {
            return;
        }

        if(pVisual->get_HasVisualClip())
        {
            Rect visualClip = pVisual->get_VisualClip();
            if(!Rect::Intersect(bboxInvalidate, pVisual->get_VisualClip(), &bboxInvalidate))
            {
                return;
            }
        }
        bboxInvalidate = pVisual->ChildToParent(bboxInvalidate);

        pVisual = pVisual->get_VisualParent();
    }

    if(Application::get_Current() != NULL)
    {
        Application::get_Current()->AddDirtyRect(bboxInvalidate);
    }
}

void Visual::InvalidateBbox()
{
    if(!get_HasBboxSubgraph())
    {
        return;
    }

    InvalidateRect(m_bboxSubgraph);

    SetFlags(false, VisualFlags::IsRenderValid);
}

void Visual::set_VisualOffset(const Vector& offset)
{
    if(!Float::AreClose(m_offset, offset)) 
    { 
        InvalidateBbox(); 

        m_offset = offset; 

        PropagateFlags(this, VisualFlags::IsSubtreeDirtyForPrecompute);
    }
}


void Visual::ClearRenderDataClientGeometry()
{
    set_IsGeometryValid(false);

    if(m_pRenderData != NULL)
    {
        m_pRenderData->ClearClientGeometry();
    }
}

void Visual::PropagateFlags(Visual* pVisual, VisualFlags::Enum value)
{
    while(pVisual != NULL && !pVisual->CheckFlagsAnd(value))
    {
        pVisual->SetFlags(true, value);

        pVisual = pVisual->m_pVisualParent;
    }
}

void Visual::ReleaseRenderData()
{
    ClearRenderDataClientGeometry();

    unsigned int cChildren = get_VisualChildCount();

    for(unsigned int idx = 0; idx < cChildren; idx++)
    {
        get_VisualChild(idx)->ReleaseRenderData();
    }
}


}; // namespace AVUI
