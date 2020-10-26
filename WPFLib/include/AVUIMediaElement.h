#pragma once

#include <AVUIFrameworkElement.h>

namespace AVUI {

class Renderer;
class IClientMediaElement;

class MediaElement : public FrameworkElement
{
    friend class Visual;
public:
    DECLARE_ELEMENT(MediaElement, FrameworkElement);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Source, String, NULL, PropertyFlag::IsContentProperty);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Volume, float, 0.5f, DependencyPropertyMetadata(PropertyFlag::None, MediaElement::OnVolumeChanged));
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Stretch, Stretch, Stretch::Uniform, PropertyFlag::AffectsMeasure);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(StretchDirection, StretchDirection, StretchDirection::Both, PropertyFlag::AffectsMeasure);

    MediaElement();
    ~MediaElement();

    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);

protected:
    virtual void OnVisualParentChanged(DependencyObject* pOldParent);


private:
    static void OnVolumeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);


    void RenderMedia(Renderer* pRenderer);
    virtual void OnRender(DrawingContext& drawingContext);

    IClientMediaElement* m_pClientMediaElement;
};



}; // namespace AVUI
