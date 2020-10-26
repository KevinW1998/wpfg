#include <AVUICommon.h>
#include <AVUIPopup.h>
#include <AVUIPopupRoot.h>
#include <AVUIDispatcherTimer.h>
#include <AVUIMatrixTransform.h>
#include <AVUIApplication.h>
#include <AVUIDispatcher.h>
#include <AVUIInputManager.h>
#include <AVUIBinding.h>
#include <AVUIMenuBase.h>

REGISTER_ELEMENT_BEGIN(AVUI::Popup)
    REGISTER_STATIC_CONSTRUCTOR(StaticConstructor)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_PROPERTY(AllowsTransparency)
    REGISTER_PROPERTY(Child)
    REGISTER_PROPERTY(HasDropShadow)
    REGISTER_PROPERTY(HorizontalOffset)
    REGISTER_PROPERTY(IsOpen)
    REGISTER_PROPERTY(Placement)
    REGISTER_PROPERTY(PlacementRectangle)
    REGISTER_PROPERTY(PlacementTarget)
    REGISTER_PROPERTY(PopupAnimation)
    REGISTER_PROPERTY(StaysOpen)
    REGISTER_PROPERTY(VerticalOffset)
REGISTER_ELEMENT_END()

namespace AVUI {

UncommonField<ItemStructList<TRefCountedPtr<Popup> > > Popup::RegisteredPopupsField;

void Popup::StaticConstructor()
{
    EventManager::RegisterClassHandler(GET_TYPE(Popup), Mouse::get_LostMouseCaptureEvent(), MouseEventHandler::Create(OnLostMouseCapture), false);
}


Popup::Popup() : m_isCaptureEngaged(false), m_isTransparent(false), m_onClosedHandlerReopen(false), m_dropOppositeSet(false), m_dropOpposite(false),
                 m_animateFromRight(false), m_animateFromBottom(false), m_notHitTestable(false)

{
    m_positionInfo.X = 0;
    m_positionInfo.Y = 0;
}


Popup::~Popup()
{
}


void Popup::OnChildChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    Popup* pPopup = (Popup*) pDO;

    if(e.pOldValue != NULL)
    {
        pPopup->RemoveLogicalChild(e.pOldValue);
    }

    if(pPopup->m_pPopupRoot != NULL && pPopup->get_IsOpen())
    {
        pPopup->m_pPopupRoot->set_Child((UIElement*)e.pNewValue);
    }

    pPopup->RemoveLogicalChild(e.pOldValue);
    pPopup->AddLogicalChild(e.pNewValue);

    pPopup->Reposition();
}

void Popup::Reposition()
{
    if (get_IsOpen() && m_pPopupRoot != NULL && m_pPopupRoot->IsWindowAlive())
    {
        UpdatePosition();
    }
}

void Popup::OnIsOpenChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
    Popup* pPopup = (Popup*) pDO;
    bool flag = (pPopup->m_pPopupRoot != NULL && pPopup->m_pPopupRoot->IsWindowAlive() && pPopup->m_pAsyncDestroy == NULL) || (pPopup->m_pAsyncCreate != NULL);
    bool newValue = UnboxValue<bool>(e.pNewValue);
    if (newValue != flag)
    {
        if (newValue)
        {
            if (pPopup->m_onClosedHandlerReopen)
            {
                LibraryCriticalError();
            }
            pPopup->CancelAsyncDestroy();
            pPopup->CancelAsyncCreate();
            pPopup->CreateWindow(false);

            pPopup->add_Unloaded(RoutedEventHandler::Create(&Popup::CloseOnUnloaded));
        }
        else
        {
            pPopup->CancelAsyncCreate();
            if (pPopup->m_pPopupRoot->IsWindowAlive() && pPopup->m_pAsyncDestroy == NULL)
            {
                pPopup->HideWindow();

                pPopup->remove_Unloaded(RoutedEventHandler::Create(&Popup::CloseOnUnloaded));
            }
        }
    }
}


void Popup::CancelAsyncDestroy()
{
    if (m_pAsyncDestroy != NULL)
    {
        m_pAsyncDestroy->Stop();
        m_pAsyncDestroy = NULL;
    }
}

void Popup::CancelAsyncCreate()
{
    if (m_pAsyncCreate != NULL)
    {
        m_pAsyncCreate->Stop();
        m_pAsyncCreate = NULL;
    }
}
 
void Popup::CreateWindow(bool isAsyncCall)
{
    ClearDropOpposite();
    Visual* pTarget = GetTarget();
    if (pTarget != NULL && IsVisualPresentationSourceNull(pTarget))
    {
        if (!isAsyncCall)
        {
            m_pAsyncCreate = object_allocate<DispatcherTimer>(TimeSpan::get_Zero(), EventHandler::Create(this, &Popup::AsyncCreateWindow));
            m_pAsyncCreate->Start();
        }
    }
    else
    {
        m_positionInfo.MouseRect = Rect();
        m_positionInfo.ChildSize = Size();

        bool flag = m_pPopupRoot == NULL || !m_pPopupRoot->IsWindowAlive();
        if (flag)
        {
            // Reversed order of this -- our popup root is required to build a window
            CreateNewPopupRoot();
            BuildWindow(pTarget);
        }
        UIElement* pChild = get_Child();
        if (m_pPopupRoot->get_Child() != pChild)
        {
            m_pPopupRoot->set_Child(pChild);
        }
        UpdatePlacementTargetRegistration(NULL, get_PlacementTarget());
        UpdateTransform();
        if (flag)
        {
            SetRootVisualToPopupRoot();
        }
        else
        {
            UpdatePosition();
        }

        ShowWindow();
        OnOpened(NULL);
    }
}

void Popup::AsyncCreateWindow(Object* pSender, EventArgs* pArgs)
{
    if(m_pAsyncCreate != NULL)
    {
        m_pAsyncCreate->Stop();
    }

    m_pAsyncCreate = NULL;
    CreateWindow(true);
}

void Popup::AsyncDestroyWindow(Object* pSender, EventArgs* pArgs)
{
    if(m_pAsyncDestroy != NULL)
    {
        m_pAsyncDestroy->Stop();
    }

    m_pAsyncDestroy = NULL;
    DestroyWindow();
}

 
void Popup::HideWindow()
{
    bool flag = SetupAnimations(false);
    SetHitTestable(false);
    ReleasePopupCapture();

    m_positionInfo = PositionInfo();
    m_positionInfo.X = 0;
    m_positionInfo.Y = 0;



    TimeSpan delayTime = flag ? get_AnimationDelayTime() : TimeSpan::get_Zero();

    m_pAsyncDestroy = object_allocate<DispatcherTimer>(delayTime, EventHandler::Create(this, &Popup::AsyncDestroyWindow));
    m_pAsyncDestroy->Start();

    if(!flag)
    {
        m_pPopupRoot->HideWindow();
    }
}

void Popup::ShowWindow()
{
    if (m_pPopupRoot != NULL && m_pPopupRoot->IsWindowAlive())
    {
        m_pPopupRoot->set_Opacity(1.0f);
        SetupAnimations(true);
        SetHitTestable(get_HitTestable() || !get_IsTransparent());
        EstablishPopupCapture();
        m_pPopupRoot->ShowWindow();
    }
}

 
void Popup::SetHitTestable(bool hitTestable)
{
    m_pPopupRoot->set_IsHitTestVisible(hitTestable);
}

Visual* Popup::GetTarget()
{
    Visual* pPlacementTarget = get_PlacementTarget();
    if (pPlacementTarget == NULL)
    {
        pPlacementTarget = get_VisualParent();
    }
    return pPlacementTarget;
}
 
void Popup::CreateNewPopupRoot()
{
    if (m_pPopupRoot == NULL)
    {
        m_pPopupRoot = object_allocate<PopupRoot>();
        AddLogicalChild(m_pPopupRoot);
        m_pPopupRoot->SetupLayoutBindings(this);
    }
}

void Popup::BuildWindow(Visual* pTargetVisual)
{
    CoerceValue(get_AllowsTransparencyProperty());
    CoerceValue(get_HasDropShadowProperty());
    set_IsTransparent(get_AllowsTransparency());
    int x = m_positionInfo.X;
    int y = m_positionInfo.Y;
    m_pPopupRoot->BuildWindow(x, y, pTargetVisual, get_IsTransparent());
}

void Popup::DestroyWindow()
{
    if (m_pPopupRoot->IsWindowAlive())
    {
        m_pPopupRoot->DestroyWindow();
        ReleasePopupCapture();
        OnClosed(NULL);
        UpdatePlacementTargetRegistration(get_PlacementTarget(), NULL);
    }
}

bool Popup::IsVisualPresentationSourceNull(Visual* pVisual)
{
    if(pVisual == NULL)
    {
        return true;
    }

    while(pVisual->get_VisualParent() != NULL)
    {
        pVisual = pVisual->get_VisualParent();
    }

    return !pVisual->IsType(GET_TYPE(Window)) && !pVisual->IsType(GET_TYPE(PopupRoot));
}
 
 
void Popup::UpdatePlacementTargetRegistration(UIElement* pOldValue, UIElement* pNewValue)
{
    if (pOldValue != NULL)
    {
        UnregisterPopupFromPlacementTarget(this, pOldValue);
        // TODO -- fix when we're doing inherited properties
        /*
        if ((newValue == null) && (VisualTreeHelper.GetParent(this) == null))
        {
            TreeWalkHelper.InvalidateOnTreeChange(this, null, oldValue, false);
        }
        */
    }
    if (pNewValue != NULL && get_VisualParent() == NULL)
    {
        RegisterPopupWithPlacementTarget(this, pNewValue);
        //TreeWalkHelper.InvalidateOnTreeChange(this, null, newValue, true);
    }
}

 
void Popup::RegisterPopupWithPlacementTarget(Popup* pPopup, UIElement* pPlacementTarget)
{
    TRefCountedPtr<ItemStructList<TRefCountedPtr<Popup> >  >pList = RegisteredPopupsField.GetValue(pPlacementTarget);
    if (pList == NULL)
    {
        pList = object_allocate<ItemStructList<TRefCountedPtr<Popup> > >();
        RegisteredPopupsField.SetValue(pPlacementTarget, pList);
    }
    if (!pList->Contains(pPopup))
    {
        pList->AddItem(pPopup);
    }
}

void Popup::UnregisterPopupFromPlacementTarget(Popup* pPopup, UIElement* pPlacementTarget)
{
    TRefCountedPtr<ItemStructList<TRefCountedPtr<Popup> > > pList = RegisteredPopupsField.GetValue(pPlacementTarget);
    if (pList != NULL)
    {
        pList->Remove(pPopup);
        if(pList->get_Count() == 0)
        {
            RegisteredPopupsField.SetValue(pPlacementTarget, NULL);
        }
    }

}

void Popup::UpdateTransform()
{
    Matrix matrix;

    if(get_LayoutTransform() != NULL)
    {
        matrix = matrix * get_LayoutTransform()->get_Value();
    }

    if(get_RenderTransform() != NULL)
    {
        matrix = matrix * get_RenderTransform()->get_Value();
    }

    DependencyObject* pParent = get_VisualParent();
    Visual* pAncestor = (pParent == NULL) ? NULL : GetRootVisual(this);
    if (pAncestor != NULL)
    {
        matrix = (matrix * TransformToAncestor(pAncestor)) * PointUtil::GetVisualTransform(pAncestor);
    }

    Vector vector = matrix.Transform(Vector(1.0, 0.0));
    Vector vector2 = matrix.Transform(Vector(0.0, 1.0));
    matrix = Matrix();
    matrix.Scale(vector.get_Length(), vector2.get_Length());

    m_pPopupRoot->set_Transform(object_allocate<MatrixTransform>(matrix));
}

 
void Popup::SetRootVisualToPopupRoot()
{
    if ((get_PopupAnimation() != PopupAnimation::None) && get_IsTransparent())
    {
        m_pPopupRoot->set_Opacity(0.0f);
    }

    /* Deviation -- Ensures is laid out before setting up animations */
    m_pPopupRoot->Measure(Application::get_Current()->get_PresentationSourceSize());
    m_pPopupRoot->Arrange(Rect(Point(), m_pPopupRoot->get_DesiredSize()));
    Reposition();

    // this._secHelper.SetWindowRootVisual(this._popupRoot.Value);    
}

void Popup::UpdatePosition()
{
    if (m_pPopupRoot != NULL)
    {
        int numberOfCombinations;
        PlacementMode::Enum placement = get_Placement();
        ItemStructList<Point> placementTargetInterestPoints;
        ItemStructList<Point> childInterestPoints;

        GetPlacementTargetInterestPoints(placement, &placementTargetInterestPoints);
        GetChildInterestPoints(placement, &childInterestPoints);

        Rect bounds = GetBounds(&placementTargetInterestPoints);
        Rect screenBounds = GetScreenBounds(bounds);
        Rect rect = GetBounds(&childInterestPoints);
        float num = rect.get_Width() * rect.get_Height();
        int num2 = -1;
        Vector offsetVector((float)m_positionInfo.X, (float)m_positionInfo.Y);
        float num3 = -1.0f;

        /*
        CustomPopupPlacement[] placementArray = null;
        if (placement == PlacementMode.Custom)
        {
            CustomPopupPlacementCallback customPopupPlacementCallback = this.CustomPopupPlacementCallback;
            if (customPopupPlacementCallback != null)
            {
                placementArray = customPopupPlacementCallback(rect.Size, bounds.Size, new Point(this.HorizontalOffset, this.VerticalOffset));
            }
            numberOfCombinations = (placementArray == null) ? 0 : placementArray.Length;
            if (!this.IsOpen)
            {
                return;
            }
        }
        else
        */
        {
            numberOfCombinations = GetNumberOfCombinations(placement);
        }
        for (int i = 0; i < numberOfCombinations; i++)
        {
            Vector vector2;
            PopupPrimaryAxis::Enum primaryAxis;
            bool flag = false;
            bool flag2 = false;

            /*
            if (placement == PlacementMode.Custom)
            {
                vector2 = ((Vector) placementTargetInterestPoints[0]) + ((Vector) placementArray[i].Point);
                primaryAxis = placementArray[i].PrimaryAxis;
            }
            else
            */
            {
                PointCombination combination = GetPointCombination(placement, i, &primaryAxis);
                InterestPoint::Enum targetInterestPoint = combination.TargetInterestPoint;
                InterestPoint::Enum childInterestPoint = combination.ChildInterestPoint;
                vector2 = placementTargetInterestPoints[(int) targetInterestPoint] - childInterestPoints[(int) childInterestPoint];
                flag = (childInterestPoint == InterestPoint::TopRight) || (childInterestPoint == InterestPoint::BottomRight);
                flag2 = (childInterestPoint == InterestPoint::BottomLeft) || (childInterestPoint == InterestPoint::BottomRight);
            }
            Rect rect4 = Rect::Offset(rect, vector2);
            Rect rect5;

            float num6 = 0.0f;
            if(Rect::Intersect(screenBounds, rect4, &rect5))
            {
                num6 = rect5.get_Width() * rect5.get_Height();
            }
            if ((num6 - num3) > 0.01f)
            {
                num2 = i;
                offsetVector = vector2;
                num3 = num6;
                set_AnimateFromRight(flag);
                set_AnimateFromBottom(flag2);
                if (Float::Abs((float) (num6 - num)) < 0.01f)
                {
                    break;
                }
            }
        }
        if ((num2 >= 2) && ((placement == PlacementMode::Right) || (placement == PlacementMode::Left)))
        {
            set_DropOpposite(!get_DropOpposite());
        }

        Size renderSize = m_pPopupRoot->get_RenderSize();
        Point transformPoint(renderSize.get_Width(), renderSize.get_Height());

        transformPoint = m_pPopupRoot->GetTransformToDevice().Transform(transformPoint);

        rect = Rect(Size(transformPoint.get_X(), transformPoint.get_Y()));

        rect.Offset(offsetVector);

        Rect rect6;

        Rect::Intersect(screenBounds, rect, &rect6);
        if ((Float::Abs((float) (rect6.get_Width() - rect.get_Width())) > 0.01f) || (Float::Abs((float) (rect6.get_Height() - rect.get_Height())) > 0.01f))
        {
            Point point3 = placementTargetInterestPoints[0];
            Point point4 = placementTargetInterestPoints[1];
            Vector vector3 = (Vector) (point4 - point3);
            vector3.Normalize();
            if ((!get_IsTransparent() || Float::IsNaN(vector3.get_Y())) || (Float::Abs(vector3.get_Y()) < 0.01f))
            {
                if (rect.get_Right() > screenBounds.get_Right())
                {
                    offsetVector.set_X(screenBounds.get_Right() - rect.get_Width());
                }
                else if (rect.get_Left() < screenBounds.get_Left())
                {
                    offsetVector.set_X(screenBounds.get_Left());
                }
            }
            else if (get_IsTransparent() && (Float::Abs(vector3.get_X()) < 0.01f))
            {
                if (rect.get_Bottom() > screenBounds.get_Bottom())
                {
                    offsetVector.set_Y(screenBounds.get_Bottom() - rect.get_Height());
                }
                else if (rect.get_Top() < screenBounds.get_Top())
                {
                    offsetVector.set_Y(screenBounds.get_Top());
                }
            }
            Point point5 = placementTargetInterestPoints[2];
            Vector vector4 = (Vector) (point3 - point5);
            vector4.Normalize();
            if ((!get_IsTransparent() || Float::IsNaN(vector4.get_X())) || (Float::Abs(vector4.get_X()) < 0.01f))
            {
                if (rect.get_Bottom() > screenBounds.get_Bottom())
                {
                    offsetVector.set_Y(screenBounds.get_Bottom() - rect.get_Height());
                }
                else if (rect.get_Top() < screenBounds.get_Top())
                {
                    offsetVector.set_Y(0.0f);
                }
            }
            else if (get_IsTransparent() && (Float::Abs(vector4.get_Y()) < 0.01f))
            {
                if (rect.get_Right() > screenBounds.get_Right())
                {
                    offsetVector.set_X(screenBounds.get_Right() - rect.get_Width());
                }
                else if (rect.get_Left() < screenBounds.get_Left())
                {
                    offsetVector.set_X(0.0f);
                }
            }
        }
        int x = Float::FloatToInt(offsetVector.get_X());
        int y = Float::FloatToInt(offsetVector.get_Y());
        if ((x != m_positionInfo.X) || (y != m_positionInfo.Y))
        {
            m_positionInfo.X = x;
            m_positionInfo.Y = y;
            m_pPopupRoot->SetPopupPos(true, x, y, false, 0, 0);
        }
    }
}

 
Rect Popup::GetBounds(ItemStructList<Point> *pInterestPoints)
{
    float num2;
    float num4;
    float x = num2 = (*pInterestPoints)[0].get_X();
    float y = num4 = (*pInterestPoints)[0].get_Y();
    for (int i = 1; i < pInterestPoints->get_Count(); i++)
    {
        float num6 = (*pInterestPoints)[i].get_X();
        float num7 = (*pInterestPoints)[i].get_Y();
        if (num6 < x)
        {
            x = num6;
        }
        if (num6 > num2)
        {
            num2 = num6;
        }
        if (num7 < y)
        {
            y = num7;
        }
        if (num7 > num4)
        {
            num4 = num7;
        }
    }
    return Rect(x, y, num2 - x, num4 - y);
}

 
Popup::PointCombination Popup::GetPointCombination(PlacementMode::Enum placement, int i, PopupPrimaryAxis::Enum *pAxis)
{
    bool menuDropAlignment = false; //SystemParameters.MenuDropAlignment;
    switch (placement)
    {
    case PlacementMode::Relative:
    case PlacementMode::AbsolutePoint:
    case PlacementMode::RelativePoint:
    case PlacementMode::MousePoint:
    {
        *pAxis = PopupPrimaryAxis::Horizontal;
        if (!menuDropAlignment)
        {
            if (i == 0)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::TopLeft);
            }
            if (i == 1)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::TopRight);
            }
            if (i == 2)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::BottomLeft);
            }
            if (i == 3)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::BottomRight);
            }
            break;
        }
        if (i != 0)
        {
            if (i == 1)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::TopLeft);
            }
            if (i == 2)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::BottomRight);
            }
            if (i == 3)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::BottomLeft);
            }
            break;
        }
        return PointCombination(InterestPoint::TopLeft, InterestPoint::TopRight);
    }

    case PlacementMode::Bottom:
    case PlacementMode::Mouse:
    {
        *pAxis = PopupPrimaryAxis::Horizontal;
        if (!menuDropAlignment)
        {
            if (i == 0)
            {
                return PointCombination(InterestPoint::BottomLeft, InterestPoint::TopLeft);
            }
            if (i == 1)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::BottomLeft);
            }
            break;
        }
        if (i != 0)
        {
            if (i == 1)
            {
                return PointCombination(InterestPoint::TopRight, InterestPoint::BottomRight);
            }
            break;
        }
        return PointCombination(InterestPoint::BottomRight, InterestPoint::TopRight);
    }

    case PlacementMode::Center:
    {
        *pAxis = PopupPrimaryAxis::None;
        return PointCombination(InterestPoint::Center, InterestPoint::Center);
    }

    case PlacementMode::Right:
    case PlacementMode::Left:
    {
        *pAxis = PopupPrimaryAxis::Vertical;
        menuDropAlignment |= get_DropOpposite();
        if ((!menuDropAlignment || (placement != PlacementMode::Right)) && (menuDropAlignment || (placement != PlacementMode::Left)))
        {
            if (i == 0)
            {
                return PointCombination(InterestPoint::TopRight, InterestPoint::TopLeft);
            }
            if (i == 1)
            {
                return PointCombination(InterestPoint::BottomRight, InterestPoint::BottomLeft);
            }
            if (i == 2)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::TopRight);
            }
            if (i == 3)
            {
                return PointCombination(InterestPoint::BottomLeft, InterestPoint::BottomRight);
            }
            break;
        }
        if (i == 0)
        {
            return PointCombination(InterestPoint::TopLeft, InterestPoint::TopRight);
        }
        if (i == 1)
        {
            return PointCombination(InterestPoint::BottomLeft, InterestPoint::BottomRight);
        }
        if (i == 2)
        {
            return PointCombination(InterestPoint::TopRight, InterestPoint::TopLeft);
        }
        if (i != 3)
        {
            break;
        }
        return PointCombination(InterestPoint::BottomRight, InterestPoint::BottomLeft);
    }

    case PlacementMode::Top:
    {
        *pAxis = PopupPrimaryAxis::Horizontal;
        if (!menuDropAlignment)
        {
            if (i == 0)
            {
                return PointCombination(InterestPoint::TopLeft, InterestPoint::BottomLeft);
            }
            if (i == 1)
            {
                return PointCombination(InterestPoint::BottomLeft, InterestPoint::TopLeft);
            }
            break;
        }
        if (i != 0)
        {
            if (i == 1)
            {
                return PointCombination(InterestPoint::BottomRight, InterestPoint::TopRight);
            }
            break;
        }
        return PointCombination(InterestPoint::TopRight, InterestPoint::BottomRight);
    }

    default:
    {
        *pAxis = PopupPrimaryAxis::None;
        return PointCombination(InterestPoint::TopLeft, InterestPoint::TopLeft);
    }

    }

    return PointCombination(InterestPoint::TopLeft, InterestPoint::TopRight);
}
 
bool Popup::SetupAnimations(bool visible)
{
    PopupAnimation::Enum popupAnimation = get_PopupAnimation();
    m_pPopupRoot->StopAnimations();
    if (popupAnimation != PopupAnimation::None && get_IsTransparent())
    {
        if (popupAnimation == PopupAnimation::Fade)
        {
            m_pPopupRoot->SetupFadeAnimation(get_AnimationDelayTime(), visible);
            return true;
        }
        if (visible)
        {
            m_pPopupRoot->SetupTranslateAnimations(popupAnimation, get_AnimationDelayTime(), get_AnimateFromRight(), get_AnimateFromBottom());
            return true;
        }
    }
    return false;
}

 
void Popup::EstablishPopupCapture()
{
    if (!m_isCaptureEngaged  && m_pPopupRoot != NULL && !get_StaysOpen() && Mouse::get_Captured() == NULL)
    {
        Mouse::Capture(m_pPopupRoot, CaptureMode::SubTree);
        m_isCaptureEngaged = true;
    }
}

void Popup::ReleasePopupCapture()
{
    if (m_isCaptureEngaged)
    {
        if (Mouse::get_Captured() == m_pPopupRoot)
        {
            Mouse::Capture(NULL);
        }
        m_isCaptureEngaged = false;
    }
}

Visual* Popup::GetRootVisual(Visual* pChild)
{
    if(pChild == NULL)
    {
        return NULL;
    }

    while(pChild->get_VisualParent() != NULL)
    {
        pChild = pChild->get_VisualParent();
    }

    return pChild;
}

void Popup::GetChildInterestPoints(PlacementMode::Enum placement, ItemStructList<Point>* pInterestPoints)
{
    UIElement* pChild = get_Child();
    if (pChild == NULL)
    {
        InterestPointsFromRect(Rect(), pInterestPoints);
        return;
    }

    InterestPointsFromRect(Rect(Point(), pChild->get_RenderSize()), pInterestPoints);
    UIElement* pTarget = object_cast<UIElement>(GetTarget());

    Vector animationOffset = m_pPopupRoot->get_AnimationOffset();
    Matrix mtx = TransformToClient(pChild, m_pPopupRoot);
    for (int i = 0; i < 5; i++)
    {
        Point ptTransform = (*pInterestPoints)[i] - animationOffset;
        ptTransform = mtx.Transform(ptTransform);

        (*pInterestPoints)[i] = ptTransform;
    }
}

 
void Popup::GetPlacementTargetInterestPoints(PlacementMode::Enum placement, ItemStructList<Point>* pInterestPoints)
{
    Rect placementRectangle = get_PlacementRectangle();
    UIElement* pTarget = object_cast<UIElement>(GetTarget());
    Vector vector(get_HorizontalOffset(), get_VerticalOffset());
    if (pTarget == NULL || IsAbsolutePlacementMode(placement))
    {
        if ((placement == PlacementMode::Mouse) || (placement == PlacementMode::MousePoint))
        {
            if (m_positionInfo.MouseRect.get_IsEmpty())
            {
                m_positionInfo.MouseRect = GetMouseRect(placement);
            }
            placementRectangle = m_positionInfo.MouseRect;
        }
        else if (placementRectangle.get_IsEmpty())
        {
            placementRectangle = Rect();
        }
        vector = m_pPopupRoot->GetTransformToDevice().Transform(vector);
        placementRectangle.Offset(vector);
        InterestPointsFromRect(placementRectangle, pInterestPoints);
        return;
    }
    if (placementRectangle.get_IsEmpty())
    {
        if ((placement != PlacementMode::Relative) && (placement != PlacementMode::RelativePoint))
        {
            placementRectangle = Rect(0.0f, 0.0f, pTarget->get_RenderSize().get_Width(), pTarget->get_RenderSize().get_Height());
        }
        else
        {
            placementRectangle = Rect();
        }
    }
    placementRectangle.Offset(vector);
    InterestPointsFromRect(placementRectangle, pInterestPoints);

    Visual* pRootVisual = GetRootVisual(pTarget);

    Matrix mtx = TransformToClient(pTarget, pRootVisual);

    for (int i = 0; i < 5; i++)
    {
        Point ptTransform = (*pInterestPoints)[i];
        ptTransform = mtx.Transform(ptTransform);

        (*pInterestPoints)[i] = ptTransform;
    }
}
 
Rect Popup::GetScreenBounds(Rect& boundingBox)
{
    return Rect(Point(), Application::get_Current()->get_PresentationSourceSize());
}

int Popup::GetNumberOfCombinations(PlacementMode::Enum placement)
{
    switch (placement)
    {
    case PlacementMode::Bottom:
    case PlacementMode::Mouse:
    case PlacementMode::Top:
        return 2;

    case PlacementMode::Right:
    case PlacementMode::AbsolutePoint:
    case PlacementMode::RelativePoint:
    case PlacementMode::MousePoint:
    case PlacementMode::Left:
        return 4;

    case PlacementMode::Custom:
        return 0;
    }
    return 1;
}

void Popup::InterestPointsFromRect(const Rect& rect, ItemStructList<Point>* pInterestPoints)
{
    pInterestPoints->AddItem(rect.get_TopLeft()); 
    pInterestPoints->AddItem(rect.get_TopRight()); 
    pInterestPoints->AddItem(rect.get_BottomLeft()); 
    pInterestPoints->AddItem(rect.get_BottomRight()); 
    pInterestPoints->AddItem(Point(rect.get_X() + rect.get_Width() / 2.0f, rect.get_Y() + rect.get_Height() / 2.0f)); 
}
 
Matrix Popup::TransformToClient(Visual* pVisual, Visual* pRootVisual)
{
    Matrix mtx;

    mtx = mtx * pVisual->TransformToAncestor(pRootVisual);
    mtx = mtx * PointUtil::GetVisualTransform(pRootVisual);
    mtx = mtx * m_pPopupRoot->GetTransformToDevice(pRootVisual);

    return mtx;
}

bool Popup::IsAbsolutePlacementMode(PlacementMode::Enum placement)
{
    switch (placement)
    {
    case PlacementMode::AbsolutePoint:
    case PlacementMode::Mouse:
    case PlacementMode::MousePoint:
    case PlacementMode::Absolute:
        return true;
    }
    return false;
}

Rect Popup::GetMouseRect(PlacementMode::Enum placement)
{
    Point mousePosition = Dispatcher::get_CurrentDispatcher()->get_InputManager()->get_MousePosition(NULL);
    if (placement == PlacementMode::Mouse)
    {
        return Rect(mousePosition.get_X(), mousePosition.get_Y(), 32.0f, 32.0f);
//        GetMouseCursorSize(out num, out num2, out num3, out num4);
//        return Rect((mouseCursorPos.x, (double) (mouseCursorPos.y - 1), (double) Math.Max(0, num - num3), (double) Math.Max(0, (num2 - num4) + 2));
    }
    return Rect(mousePosition.get_X(), mousePosition.get_Y(), 0.0f, 0.0f);
}

 
void Popup::OnWindowResize(const Size& size)
{
    if(size != m_positionInfo.ChildSize)
    {
        m_positionInfo.ChildSize = size;
        Reposition();
    }
}

void Popup::OnLostMouseCapture(Object* pSender, MouseEventArgs* pArgs)
{
    Popup* pPopup = (Popup*) pSender;
    if (!pPopup->get_StaysOpen())
    {
        PopupRoot* pRoot = pPopup->m_pPopupRoot;
        if (Mouse::get_Captured() != pRoot)
        {
            if (pArgs->get_OriginalSource() == pRoot)
            {
                if (Mouse::get_Captured() == NULL || !MenuBase::IsDescendant(pRoot, Mouse::get_Captured()))
                {
                    pPopup->set_IsOpen(false);
                }
                else
                {
                    pPopup->m_isCaptureEngaged = false;
                }
            }
            else if (MenuBase::IsDescendant(pRoot, object_cast<UIElement>(pArgs->get_OriginalSource())))
            {
                if (pPopup->get_IsOpen() && Mouse::get_Captured() == NULL)
                {
                    pPopup->EstablishPopupCapture();
                    pArgs->set_IsHandled(true);
                }
            }
            else
            {
                pPopup->set_IsOpen(false);
            }
        }
    }
}
 
void Popup::ForceClose()
{
    if (m_pAsyncDestroy != NULL)
    {
        CancelAsyncDestroy();
        DestroyWindow();
    }
}

 
void Popup::CreateRootPopup(Popup* pPopup, UIElement* pChild)
{
    TRefCountedPtr<Binding> pBinding;

    pBinding = object_allocate<Binding>("PlacementTarget");
    pBinding->set_Mode(BindingMode::OneWay);
    pBinding->set_Source(pChild);
    pPopup->SetBinding(get_PlacementTargetProperty(), pBinding);

    pPopup->set_Child(pChild);


    pBinding = object_allocate<Binding>("VerticalOffset");
    pBinding->set_Mode(BindingMode::OneWay);
    pBinding->set_Source(pChild);
    pPopup->SetBinding(get_VerticalOffsetProperty(), pBinding);

    pBinding = object_allocate<Binding>("HorizontalOffset");
    pBinding->set_Mode(BindingMode::OneWay);
    pBinding->set_Source(pChild);
    pPopup->SetBinding(get_HorizontalOffsetProperty(), pBinding);

    pBinding = object_allocate<Binding>("PlacementRectangle");
    pBinding->set_Mode(BindingMode::OneWay);
    pBinding->set_Source(pChild);
    pPopup->SetBinding(get_PlacementRectangleProperty(), pBinding);

    pBinding = object_allocate<Binding>("Placement");
    pBinding->set_Mode(BindingMode::OneWay);
    pBinding->set_Source(pChild);
    pPopup->SetBinding(get_PlacementProperty(), pBinding);

    pBinding = object_allocate<Binding>("StaysOpen");
    pBinding->set_Mode(BindingMode::OneWay);
    pBinding->set_Source(pChild);
    pPopup->SetBinding(get_StaysOpenProperty(), pBinding);


    pBinding = object_allocate<Binding>("IsOpen");
    pBinding->set_Mode(BindingMode::OneWay);
    pBinding->set_Source(pChild);
    pPopup->SetBinding(get_IsOpenProperty(), pBinding);
}
 
 

 

 
 



 

 

 

 
 
 




 

 

 

 

 
 

 

 




}; // namespace AVUI


