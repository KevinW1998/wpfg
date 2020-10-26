#include <AVUIGradientBrush.h>

namespace AVUI {

class IClientRadialGradientBrush;

class RadialGradientBrush : public GradientBrush
{
public:
    RadialGradientBrush();
    ~RadialGradientBrush();


    DECLARE_ELEMENT(RadialGradientBrush, GradientBrush);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(Center, Point, Point(0.5f, 0.5f), DependencyPropertyMetadata(PropertyFlag::None, OnCenterChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RadiusX, float, 0.5f, DependencyPropertyMetadata(PropertyFlag::None, OnRadiusXChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(RadiusY, float, 0.5f, DependencyPropertyMetadata(PropertyFlag::None, OnRadiusYChanged));
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(GradientOrigin, Point, Point(0.5f, 0.5f), DependencyPropertyMetadata(PropertyFlag::None, OnGradientOriginChanged));

    virtual void set_ActiveBrush(Renderer* pRenderer);

protected:

    virtual void OnGradientStopsChanged();
    virtual void CloneCore(Animatable* pSource);

private:
    static void OnCenterChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnRadiusXChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnRadiusYChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnGradientOriginChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

    IClientRadialGradientBrush* m_pClientRadialGradientBrush;
    mutable bool m_needsGradientUpdate;
};

typedef TRefCountedPtr<RadialGradientBrush> RadialGradientBrushPtr;

}; // namespace AVUI
