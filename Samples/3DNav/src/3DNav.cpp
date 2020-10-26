#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUIUIElement.h>
#include <AVUIViewport3D.h>
#include <AVUIPoint3DAnimation.h>
#include <AVUIVector3DAnimation.h>
#include <AVUIStoryboard.h>
#include <AVUIControl.h>
#include <AVUIProjectionCamera.h>
#include <AVUIDispatcherTimer.h>
#include <AVUIWindow.h>
#include <AVUIButton.h>

using namespace AVUI;


ObjectPtr LoadXaml(const WCHAR* wzXamlFile);

class Nav : public DependencyObject
{
public:
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(NavTarget, String, NULL, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CameraLocation, Point3D, Point3D(), PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CameraOffset, Vector3D, Vector3D(), PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(CameraLookDirection, Vector3D, Vector3D(0, 0, 1), PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(IsNavPanel, bool, false, PropertyFlag::IsAttachedProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(TransitionDelay, TimeSpan, TimeSpan(), PropertyFlag::IsAttachedProperty);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(TransitionAnimation, Storyboard, NULL, PropertyFlag::IsAttachedProperty);
    DECLARE_ELEMENT(Nav, DependencyObject);
};

REGISTER_ELEMENT_BEGIN(Nav)

    REGISTER_PROPERTY(CameraLocation);
    REGISTER_PROPERTY(CameraOffset);
    REGISTER_PROPERTY(CameraLookDirection);
    REGISTER_PROPERTY(IsNavPanel);
    REGISTER_PROPERTY(NavTarget);
    REGISTER_PROPERTY(TransitionDelay);
    REGISTER_PROPERTY(TransitionAnimation);
REGISTER_ELEMENT_END()

class AnimatingScrollContentPresenter;

void RegisterLocalTypes()
{
    Application::RegisterType(GET_TYPE(Nav));
    Application::RegisterType(GET_TYPE(AnimatingScrollContentPresenter));
}



UIElement* FindNavPanel(UIElement* pElement)
{
    while(pElement != NULL)
    {
        bool isNavPanel = UnboxValue<bool>(pElement->GetValue(Nav::get_IsNavPanelProperty()));

        if(isNavPanel)
        {
            return pElement;
        }
        pElement = object_cast<UIElement>(pElement->get_VisualParent());
    }
    return NULL;
}

Vector3D g_lastLookDirection;
ItemStructList<TRefCountedPtr<Control> > g_navStack;

void OnKeyDown(Object* pSender, KeyEventArgs* pArgs)
{
    if(pArgs->get_Key() == Key::Escape || pArgs->get_Key() == Key::BrowserBack) 
    {
        if(g_navStack.get_Count() > 1)
        {
            g_navStack[g_navStack.get_Count() - 2]->Focus();
            g_navStack.RemoveAt(g_navStack.get_Count() - 1);
        }

        pArgs->set_IsHandled(true);
    }
}


void OnGotFocus(Object* pSender, RoutedEventArgs* pArgs)
{
    Viewport3D* pScene = object_cast<Viewport3D>(((UIElement*)pSender)->FindName(L"3DScene"));
    Control* pControl = object_cast<Control>(pArgs->get_Source());
    UIElement* pNavPanel = FindNavPanel(pControl);

    if(pNavPanel == NULL)
    {
        return;
    }

    Point3D location = UnboxValue<Point3D>(pNavPanel->GetValue(Nav::get_CameraLocationProperty()));
    Vector3D offset = UnboxValue<Vector3D>(pControl->GetValue(Nav::get_CameraOffsetProperty()));
    Vector3D lookDirection = UnboxValue<Vector3D>(pNavPanel->GetValue(Nav::get_CameraLookDirectionProperty()));

    Point3D newLocation(location.get_X() + offset.get_X(), location.get_Y() + offset.get_Y(), location.get_Z() + offset.get_Z());
    TRefCountedPtr<Point3DAnimation> pAnim = object_allocate<Point3DAnimation>();

    pAnim->set_To(newLocation);
    pAnim->set_AccelerationRatio(0.1f);
    pAnim->set_DecelerationRatio(0.6f);
    pAnim->set_Duration(TimeSpan::FromMilliseconds(600));

    pScene->get_Camera()->BeginAnimation(ProjectionCamera::get_PositionProperty(), pAnim, HandoffBehavior::SnapshotAndReplace);

    if(lookDirection != g_lastLookDirection)
    {
        TRefCountedPtr<Vector3DAnimation> pLookAnim = object_allocate<Vector3DAnimation>();

        pLookAnim->set_To(lookDirection);
        pLookAnim->set_AccelerationRatio(0.1f);
        pLookAnim->set_DecelerationRatio(0.6f);
        pLookAnim->set_Duration(TimeSpan::FromMilliseconds(600));

        pScene->get_Camera()->BeginAnimation(ProjectionCamera::get_LookDirectionProperty(), pLookAnim, HandoffBehavior::SnapshotAndReplace);
        pScene->get_Camera()->BeginAnimation(ProjectionCamera::get_LookDirectionProperty(), (AnimationTimeline*) pControl->FindResource(object_allocate<String>("LookDirectionIdle")), HandoffBehavior::Compose);

        g_lastLookDirection = lookDirection;
    }
}

TRefCountedPtr<DispatcherTimer> g_pTransitionTimer;

void OnTransitionDelayFinished(Object* pSender, EventArgs* pArgs)
{
    g_lastLookDirection = Vector3D();
    g_pTransitionTimer->Stop();
    g_pTransitionTimer = NULL;
    g_navStack[g_navStack.get_Count() - 1]->Focus();
}


void OnButtonClicked(Object* pSender, RoutedEventArgs* pArgs)
{    
    Viewport3D* pScene = object_cast<Viewport3D>(((UIElement*)pSender)->FindName(L"3DScene"));
    UIElement* pSource = object_cast<UIElement>(pArgs->get_Source());
    String* pNavTarget = object_cast<String>(pSource->GetValue(Nav::get_NavTargetProperty()));
    g_pTransitionTimer = NULL;

    if(pNavTarget != NULL)
    {
        Control* pControl = (Control*) ((UIElement*)pSender)->FindName(*pNavTarget);

        TimeSpan timeSpan = UnboxValue<TimeSpan>(pSource->GetValue(Nav::get_TransitionDelayProperty()));
        Storyboard* pStoryboard = object_cast<Storyboard>(pSource->GetValue(Nav::get_TransitionAnimationProperty()));

        if(pStoryboard != NULL)
        {
            pStoryboard->Begin(pScene, NULL);
        }

        g_pTransitionTimer = object_allocate<DispatcherTimer>(timeSpan, EventHandler::Create(&OnTransitionDelayFinished));
        g_pTransitionTimer->Start();
        g_navStack.AddItem(pControl);
    }
}


void SampleInit()
{
    RegisterLocalTypes();

    Application::get_Current()->set_MainWindow(object_allocate<Window>());
    Application::get_Current()->get_MainWindow()->set_WindowState(WindowState::Maximized);

    Application::get_Current()->get_MainWindow()->set_Content(LoadXaml(L"Media\\Xaml\\Scene.xaml"));

    UIElement* pRoot = object_cast<UIElement>(Application::get_Current()->get_MainWindow()->get_Content());

    pRoot->add_GotFocus(RoutedEventHandler::Create(&OnGotFocus));
    pRoot->add_KeyDown(KeyEventHandler::Create(&OnKeyDown));

    Control* pInitialFocus = object_cast<Control>(pRoot->FindName(L"Start Demo"));
    pInitialFocus->Focus();
    g_navStack.AddItem(pInitialFocus);

    pRoot->AddHandler(&Button::get_ClickEvent(), RoutedEventHandler::Create(&OnButtonClicked));

    Application::get_Current()->get_MainWindow()->Show();

}

void SampleTick()
{

}

void SampleShutdown()
{
    g_navStack.EraseData();
}

