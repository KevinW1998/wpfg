#pragma once

#include <AVUIFrameworkElement.h>

namespace AVUI {

class PopupRoot;
class DispatcherTimer;

class PopupPrimaryAxis
{
public:

    enum Enum
    {
        None,
        Horizontal,
        Vertical
    };
};

class Popup : public FrameworkElement
{
    friend class PopupRoot;
    friend class ToolTip;
    friend class ContextMenu;
public:

    Popup();
    ~Popup();

    DECLARE_ELEMENT(Popup, FrameworkElement);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(AllowsTransparency, bool, true, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Child, UIElement, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure | PropertyFlag::IsContentProperty, Popup::OnChildChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(HasDropShadow, bool, false, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(HorizontalOffset, float, 0.0f, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsOpen, bool, false, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, Popup::OnIsOpenChanged));
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Placement, PlacementMode, PlacementMode::Bottom, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(PlacementRectangle, Rect, Rect(), PropertyFlag::AffectsMeasure);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(PlacementTarget, UIElement, NULL, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(PopupAnimation, PopupAnimation, PopupAnimation::None, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(StaysOpen, bool, false, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(VerticalOffset, float, 0.0f, PropertyFlag::AffectsMeasure);

    IMPLEMENT_EVENT(Opened, EventArgs);
    IMPLEMENT_EVENT(PopupCouldClose, EventArgs);
    IMPLEMENT_EVENT(Closed, EventArgs);

    static TimeSpan get_AnimationDelayTime() { return TimeSpan::FromMilliseconds(150); }

    void ForceClose();

    static void CreateRootPopup(Popup* pPopup, UIElement* pChild);
    
protected:

    // CLR event?
    virtual void OnOpened(EventArgs* pArgs) { invoke_Opened(this, NULL); }
    virtual void OnClosed(EventArgs* pArgs) { invoke_Closed(this, NULL); }


private:

    static void StaticConstructor();

    class InterestPoint
    {
    public:
    
        enum Enum
        {
            TopLeft,
            TopRight,
            BottomLeft,
            BottomRight,
            Center
        };
    };

    struct PointCombination
    {
        PointCombination() { };
        PointCombination(InterestPoint::Enum target, InterestPoint::Enum child) : TargetInterestPoint(target), ChildInterestPoint(child) { };

        InterestPoint::Enum TargetInterestPoint;
        InterestPoint::Enum ChildInterestPoint;
    };

    void Reposition();


    bool get_IsTransparent() { return m_isTransparent; }
    void set_IsTransparent(bool isTransparent) { m_isTransparent = isTransparent; }

    bool get_DropOpposite() { return m_dropOpposite; }
    void set_DropOpposite(bool dropOpposite) { m_dropOpposite = dropOpposite; }

    bool get_AnimateFromRight() { return m_animateFromRight; }
    void set_AnimateFromRight(bool animateFromRight) { m_animateFromRight = animateFromRight; }

    bool get_AnimateFromBottom() { return m_animateFromBottom; }
    void set_AnimateFromBottom(bool animateFromBottom) { m_animateFromBottom = animateFromBottom; }

    bool get_HitTestable() { return !m_notHitTestable; }
    void set_HitTestable(bool isHitTestable) { m_notHitTestable = !isHitTestable; }

    Visual* GetTarget();
    Rect GetScreenBounds(Rect& boundingBox);
    Matrix TransformToClient(Visual* pVisual, Visual* pRootVisual);

    static bool IsAbsolutePlacementMode(PlacementMode::Enum placement);
    static int GetNumberOfCombinations(PlacementMode::Enum placement);
    static void InterestPointsFromRect(const Rect& rect, ItemStructList<Point>* pInterestPoints);

    Rect GetMouseRect(PlacementMode::Enum placement);

    void CreateNewPopupRoot();
    void BuildWindow(Visual* pTargetVisual);
    void DestroyWindow();
    static bool IsVisualPresentationSourceNull(Visual* pVisual);
    static void RegisterPopupWithPlacementTarget(Popup* pPopup, UIElement* pPlacementTarget);
    static void UnregisterPopupFromPlacementTarget(Popup* pPopup, UIElement* pPlacementTarget);

    static void OnLostMouseCapture(Object* pSender, MouseEventArgs* pArgs); 

    void UpdatePlacementTargetRegistration(UIElement* pOldValue, UIElement* pNewValue);
    Rect GetBounds(ItemStructList<Point> *pInterestPoints);
    PointCombination GetPointCombination(PlacementMode::Enum placement, int i, PopupPrimaryAxis::Enum *pAxis);

 

    struct PositionInfo
    {
        Size ChildSize;
        Rect MouseRect;
        int X;
        int Y;
    };

    void CreateWindow(bool isAsyncCall);
    bool SetupAnimations(bool visible);

    void EstablishPopupCapture();
    void ReleasePopupCapture();

    void CancelAsyncCreate();
    void CancelAsyncDestroy();

    void ClearDropOpposite() { m_dropOpposite = false; }

    void HideWindow();
    void ShowWindow();
    void SetHitTestable(bool hitTestable);
    void UpdateTransform();

    void SetRootVisualToPopupRoot();
    void UpdatePosition();

    void AsyncCreateWindow(Object* pSender, EventArgs* pArgs);
    void AsyncDestroyWindow(Object* pSender, EventArgs* pArgs);

    void GetChildInterestPoints(PlacementMode::Enum placement, ItemStructList<Point>* pInterestPoints);
    void GetPlacementTargetInterestPoints(PlacementMode::Enum placement, ItemStructList<Point>* pInterestPoints);

    static Visual* GetRootVisual(Visual* pChild);
    static void OnChildChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnIsOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void CloseOnUnloaded(Object* pSender, RoutedEventArgs* pArgs)
    {
        ((Popup*)pSender)->set_IsOpen(false);
    }

    void OnWindowResize(const Size& size);

    bool m_isCaptureEngaged : 1;
    bool m_isTransparent : 1;
    bool m_onClosedHandlerReopen : 1;
    bool m_dropOppositeSet : 1;
    bool m_dropOpposite : 1;
    bool m_animateFromRight : 1;
    bool m_animateFromBottom : 1;
    bool m_notHitTestable : 1;

    PositionInfo m_positionInfo;


    TRefCountedPtr<DispatcherTimer> m_pAsyncCreate;
    TRefCountedPtr<DispatcherTimer> m_pAsyncDestroy;

    TRefCountedPtr<PopupRoot> m_pPopupRoot;

    static UncommonField<ItemStructList<TRefCountedPtr<Popup> > > RegisteredPopupsField;  
};



}; // namespace AVUI
