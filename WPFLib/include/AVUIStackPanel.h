#include <AVUIPanel.h>

namespace AVUI {


class StackPanel : public Panel
{
public:
    StackPanel();
    ~StackPanel();


    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);

    DECLARE_ELEMENT(StackPanel, Panel);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Orientation, Orientation, Orientation::Vertical, PropertyFlag::AffectsMeasure);

protected:

    virtual bool get_HasLogicalOrientation() { return true; }
    virtual Orientation::Enum get_LogicalOrientation() { return get_Orientation(); }

private:
};

}; // namespace AVUI
