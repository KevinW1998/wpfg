#pragma once

#include <AVUIFrameworkElement.h>
#include <AVUISolidColorBrush.h>

namespace AVUI {

class InputManager;
class ControlTemplate;
class Control : public FrameworkElement
{
public:

    DECLARE_ELEMENT(Control, FrameworkElement);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Background, Brush, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(BorderBrush, Brush, NULL, PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(BorderThickness,   Thickness, Thickness(0), PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Template, ControlTemplate, NULL, DependencyPropertyMetadata(PropertyFlag::AffectsMeasure, OnTemplateChangedStatic));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Foreground, Brush, object_allocate<SolidColorBrush>(KnownColor::Black), PropertyFlag::AffectsRender | PropertyFlag::IgnoreSubPropertyInvalidation);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(FontFamily, String, object_allocate<String>("Tahoma"), PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(FontSize, float, 11.0f, PropertyFlag::AffectsMeasure);

    // Included for compatibility, not implemented
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(FontStyle, String, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(FontWeight, String, NULL, PropertyFlag::None);

    // TODO -- Affectsparentmeasure?
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Padding, Thickness, Thickness(0), PropertyFlag::AffectsMeasure);

    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(HorizontalContentAlignment, HorizontalAlignment, HorizontalAlignment::Left, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(VerticalContentAlignment, VerticalAlignment, VerticalAlignment::Top, PropertyFlag::AffectsMeasure);

    IMPLEMENT_ROUTED_EVENT(PreviewMouseDoubleClick, MouseButtonEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(MouseDoubleClick, MouseButtonEventArgs, RoutingStrategy::Direct);


    Object* FindTemplateChild(const WCHAR* wzChildName);

    bool Focus();

protected:

    virtual ~Control() { }
    virtual void OnPropertyChanged(const DependencyPropertyChangedEventArgs&);
    virtual void OnPreviewMouseDoubleClick(MouseButtonEventArgs* pArgs);
    virtual void OnMouseDoubleClick(MouseButtonEventArgs* pArgs);
 

    InputManager* get_InputManager();
    void CaptureMouse();
    bool get_IsMouseCaptured();
    void ReleaseMouseCapture();   


    virtual Size MeasureOverride(const Size& constraintSize);
    virtual Size ArrangeOverride(const Size& constraintSize);

    virtual FrameworkTemplate* get_TemplateInternal() { return (FrameworkTemplate*) get_Template(); }
    virtual void OnIsMouseCapturedChanged() { };

    virtual bool get_HandlesScrolling() { return false; } 

private:
    static void HandleDoubleClick(Object* pSender, MouseButtonEventArgs* pArgs);

    static void StaticConstructor();
    void OnTemplateChanged();

    // Update template cache?
    static void OnTemplateChangedStatic(Object* pDO, const DependencyPropertyChangedEventArgs& e) { }
};


}; // namespace AVUI
