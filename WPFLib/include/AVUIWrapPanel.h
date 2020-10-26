#include <AVUIPanel.h>

namespace AVUI {


class WrapPanel : public Panel
{
public:
    WrapPanel();
    ~WrapPanel();


    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);

    DECLARE_ELEMENT(WrapPanel, Panel);
    IMPLEMENT_ENUM_DEPENDENCY_PROPERTY(Orientation, Orientation, Orientation::Horizontal, PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ItemWidth, float, Float::get_NaN(), PropertyFlag::AffectsMeasure);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(ItemHeight, float, Float::get_NaN(), PropertyFlag::AffectsMeasure);

   
private:


    struct UVSize
    {
        UVSize(Orientation::Enum orientation, float width = 0.0f, float height = 0.0f) : Orientation(orientation) 
        { 
            set_Width(width);
            set_Height(height);
        }

        float get_Width() const { if(Orientation == Orientation::Horizontal) return U; return V; }
        void set_Width(float width) { if(Orientation == Orientation::Horizontal) U = width; else V = width; }

        float get_Height() const { if(Orientation == Orientation::Horizontal) return V; return U; }
        void set_Height(float height) { if(Orientation == Orientation::Horizontal) V = height; else U = height; }

        float U;
        float V;
        Orientation::Enum Orientation;
    };

    void ArrangeLine(float v, float lineV, int start, int end, bool useItemU, float itemU);

};

}; // namespace AVUI
