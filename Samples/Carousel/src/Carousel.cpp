#include <AVUICommon.h>
#include <AVUIApplication.h>
#include <AVUIControl.h>
#include <AVUIWindow.h>

using namespace AVUI;

ObjectPtr LoadXaml(const WCHAR* wzXamlFile);

class ListBox3D;
class ListBox3DItem;

void RegisterLocalTypes()
{
    using namespace AVUI;

    Application::RegisterType(GET_TYPE(ListBox3D));
    Application::RegisterType(GET_TYPE(ListBox3DItem));
}

void SampleInit()
{
    RegisterLocalTypes();

    Application::get_Current()->set_MainWindow(object_allocate<Window>());
    Application::get_Current()->get_MainWindow()->set_WindowState(WindowState::Maximized);

    Application::get_Current()->get_MainWindow()->set_Content(LoadXaml(L"Media\\Xaml\\CarouselScene.xaml"));

    UIElement* pRoot = object_cast<UIElement>(Application::get_Current()->get_MainWindow()->get_Content());

    Control* pInitialFocus = object_cast<Control>(pRoot->FindName(L"Carousel"));
    pInitialFocus->Focus();

    Application::get_Current()->get_MainWindow()->Show();

}

void SampleTick()
{

}

void SampleShutdown()
{

}

