#include <AVUIGradientBrush.h>

namespace AVUI {

class IClientLinearGradientBrush;

class LinearGradientBrush : public GradientBrush
{
public:
    LinearGradientBrush();
    ~LinearGradientBrush();


    DECLARE_ELEMENT(LinearGradientBrush, GradientBrush);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(StartPoint, Point, Point(0.0f, 0.0f), DependencyPropertyMetadata(PropertyFlag::None, OnStartPointChanged));
    const Point &get_StartPoint() const { return m_startPoint; }
    void set_StartPoint(const Point& value) { SetValue(get_StartPointProperty(), AVUI::BoxValue(value)); }

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY_DEFINITION(EndPoint, Point, Point(1.0f, 1.0f), DependencyPropertyMetadata(PropertyFlag::None, OnEndPointChanged));
    const Point &get_EndPoint() const { return m_endPoint; }
    void set_EndPoint(const Point& value) { SetValue(get_EndPointProperty(), AVUI::BoxValue(value)); }

    virtual void set_ActiveBrush(Renderer* pRenderer);

protected:

    virtual void OnGradientStopsChanged();
    virtual void CloneCore(Animatable* pSource);

private:
    static void OnStartPointChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnEndPointChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);


    Point m_startPoint;
    Point m_endPoint;

    IClientLinearGradientBrush* m_pClientLinearGradientBrush;
    mutable bool m_needsGradientUpdate;
};

typedef TRefCountedPtr<LinearGradientBrush> LinearGradientBrushPtr;

}; // namespace AVUI
