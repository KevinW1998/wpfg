#include <AVUICommon.h>
#include <AVUIPopupRoot.h>
#include <AVUIWindowManager.h>
#include <AVUIApplication.h>
#include <AVUIDispatcher.h>
#include <AVUILayoutManager.h>
#include <AVUIPopup.h>
#include <AVUIDecorator.h>
#include <AVUITranslateTransform.h>
#include <AVUIDoubleAnimation.h>
#include <AVUIWindow.h>

REGISTER_ELEMENT_BEGIN(AVUI::PopupRoot)
REGISTER_ELEMENT_END()

namespace AVUI {

PopupRoot::~PopupRoot()
{
    if(m_isWindowRegistered)
    {
        if(Application::get_Current() != NULL)
        {
            Application::get_Current()->get_WindowManager()->RemovePopupWindow(this);
        }
        m_isWindowRegistered = false;
    }
    m_pTransformDecorator->set_Child(NULL);
}

void PopupRoot::set_Child(UIElement* pChild)
{
    m_pTransformDecorator->set_Child(pChild);
}

UIElement* PopupRoot::get_Child()
{
    return m_pTransformDecorator->get_Child();
}

Visual* PopupRoot::get_VisualChild(unsigned int idx)
{
    return (Visual*)m_pTransformDecorator;
}

bool PopupRoot::IsWindowAlive()
{
    return !m_isWindowDisposed;
}

void PopupRoot::ShowWindow()
{
    if(!IsWindowAlive())
    {
        return;
    }
    m_isWindowShowing = true;
    set_Visibility(Visibility::Visible);
}
void PopupRoot::HideWindow()
{
    if(!IsWindowAlive())
    {
        return;
    }
    m_isWindowShowing = false;
    set_Visibility(Visibility::Visible);
}

void PopupRoot::set_Transform(Transform* pTransform)
{
    // no implementation -- transform decorator?
}
Matrix PopupRoot::GetTransformToDevice()
{
    // device transform should take  window offsets/position into consideration
    return Matrix();
}

Matrix PopupRoot::GetTransformToDevice(Visual* pVisual)
{
    if(pVisual->IsType(GET_TYPE(Window)))
    {
        Window* pWindow = (Window*) pVisual;

        return Matrix::CreateTranslation(pWindow->get_WindowLocation().get_X(), pWindow->get_WindowLocation().get_Y());
    }

    // device transform should take  window offsets/position into consideration
    return Matrix();
}

void PopupRoot::SetupLayoutBindings(Popup* pPopup)
{

}

void PopupRoot::BuildWindow(int x, int y, Visual* pPlacementTarget, bool transparent)
{
    if(m_isWindowRegistered)
    {
        LibraryCriticalError();
    }

    Application::get_Current()->get_WindowManager()->AddPopupWindow(this);
    m_isWindowDisposed = false;
    m_isWindowRegistered = true;

    UpdateIsAttachedToPresentationSource(true);

    SetPopupPos(true, x, y, false, 0, 0);
}

void PopupRoot::DestroyWindow()
{
    if(!IsWindowAlive())
    {
        return;
    }

    m_isWindowDisposed = true;
    Application::get_Current()->get_WindowManager()->RemovePopupWindow(this);
    m_isWindowRegistered = false;

    UpdateIsAttachedToPresentationSource(false);

    SetPopupPos(true, 0, 0, false, 0, 0);
}

void PopupRoot::SetPopupPos(bool position, int x, int y, bool size, int width, int height)
{
    if(position)
    {
        m_popupOffset = Vector((float)x, (float)y);
        set_VisualOffset(m_popupOffset);
    }
}

void PopupRoot::StopAnimations()
{
    BeginAnimation(UIElement::get_OpacityProperty(), NULL);
    TRefCountedPtr<TranslateTransform> pRenderTransform = object_cast<TranslateTransform>(m_pTransformDecorator->get_RenderTransform());
    if (pRenderTransform != NULL)
    {
        pRenderTransform->BeginAnimation(TranslateTransform::get_XProperty(), NULL);
        pRenderTransform->BeginAnimation(TranslateTransform::get_YProperty(), NULL);
    }
}

void PopupRoot::SetupFadeAnimation(const Duration& duration, bool isVisible)
{
    TRefCountedPtr<DoubleAnimation> pAnimation = object_allocate<DoubleAnimation>(isVisible ? 0.0f : 1.0f, isVisible ? 1.0f : 0.0f, duration, FillBehavior::HoldEnd);

    BeginAnimation(UIElement::get_OpacityProperty(), pAnimation);
}

void PopupRoot::SetupTranslateAnimations(PopupAnimation::Enum animationType, const Duration& duration, bool animateFromRight, bool animateFromBottom)
{
    UIElement* pChild = get_Child();
    if (pChild != NULL)
    {
        TRefCountedPtr<TranslateTransform> pRenderTransform = object_cast<TranslateTransform>(m_pTransformDecorator->get_RenderTransform());
        if (pRenderTransform == NULL)
        {
            pRenderTransform = object_allocate<TranslateTransform>();
            m_pTransformDecorator->set_RenderTransform(pRenderTransform);
        }
        if (animationType == PopupAnimation::Scroll)
        {
            float width = m_pTransformDecorator->get_RenderSize().get_Width();
            TRefCountedPtr<DoubleAnimation> pAnimation = object_allocate<DoubleAnimation>(animateFromRight ? width : -width, 0.0f, duration, FillBehavior::Stop);
            pRenderTransform->BeginAnimation(TranslateTransform::get_XProperty(), pAnimation);
        }
        float height = m_pTransformDecorator->get_RenderSize().get_Height();
        TRefCountedPtr<DoubleAnimation> pAnimation2 = object_allocate<DoubleAnimation>(animateFromBottom ? height : -height, 0.0f, duration, FillBehavior::Stop);
        pRenderTransform->BeginAnimation(TranslateTransform::get_YProperty(), pAnimation2);
    }
}


Size PopupRoot::MeasureOverride(const Size& constraintSize)
{
    m_pTransformDecorator->Measure(Size(Float::get_PositiveInfinity(), Float::get_PositiveInfinity()));
    Size desiredSize = m_pTransformDecorator->get_DesiredSize();

    Popup* pParent = object_cast<Popup>(get_Parent());
    if (pParent != NULL)
    {
        /*
        Size size2 = parent.RestrictSize(desiredSize);
        if ((Math.Abs((double) (size2.Width - desiredSize.Width)) <= 0.01) && (Math.Abs((double) (size2.Height - desiredSize.Height)) <= 0.01))
        {
            return desiredSize;
        }
        desiredSize = size2;
        this._transformDecorator.Measure(desiredSize);
        */
    }

    return desiredSize;
}

Size PopupRoot::ArrangeOverride(const Size& arrangeSize)
{
    m_pTransformDecorator->Arrange(Rect(arrangeSize));

    return arrangeSize;
}


void PopupRoot::ClearLayoutUpdatedHandler()
{
    if(m_isRegisteredForLayoutUpdated)
    {
        Dispatcher::get_CurrentDispatcher()->get_LayoutManager()->remove_LayoutUpdated(EventHandler::Create(this, &PopupRoot::OnLayoutUpdated));
        m_isRegisteredForLayoutUpdated = false;
    }
}

void PopupRoot::EnsureLayoutUpdatedHandler()
{
    if(!m_isRegisteredForLayoutUpdated)
    {
        m_isRegisteredForLayoutUpdated = true;
        Dispatcher::get_CurrentDispatcher()->get_LayoutManager()->add_LayoutUpdated(EventHandler::Create(this, &PopupRoot::OnLayoutUpdated));

        InvalidateArrange();
    }
}

void PopupRoot::OnLayoutUpdated(Object* pSender, EventArgs* pArgs)
{
    Popup* pParent = object_cast<Popup>(get_Parent());
    if (pParent != NULL)
    {
        pParent->OnWindowResize(get_RenderSize());
    }
    
}

void PopupRoot::Initialize()
{
    SetTreeLevel(0);

    m_pTransformDecorator = object_allocate<Decorator>();
    m_pTransformDecorator->set_IsNotLogical(true);
    set_ClipToBounds(true);
    AddVisualChild(m_pTransformDecorator);

    set_Visibility(Visibility::Collapsed);
}

Vector PopupRoot::get_AnimationOffset()
{
    TRefCountedPtr<TranslateTransform> pRenderTransform = object_cast<TranslateTransform>(m_pTransformDecorator->get_RenderTransform());
    if (pRenderTransform != NULL)
    {
        return Vector(pRenderTransform->get_X(), pRenderTransform->get_Y());
    }
    return Vector();

}

bool PopupRoot::GetLayoutClip(const Size& layoutSlotSize, Rect* pRect)
{
    *pRect = Rect(get_RenderSize());
    return true;
}


}; // namespace AVUI


