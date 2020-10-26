#include <AVUICommon.h>
#include <AVUIWrapPanel.h>

REGISTER_ELEMENT_BEGIN(AVUI::WrapPanel)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Orientation);
    REGISTER_PROPERTY(ItemWidth);
    REGISTER_PROPERTY(ItemHeight);
REGISTER_ELEMENT_END()

namespace AVUI {

WrapPanel::WrapPanel()
{

}

WrapPanel::~WrapPanel()
{

}

Size WrapPanel::MeasureOverride(const Size& constraintSize)
{
    Orientation::Enum orientation = get_Orientation();
    UVSize size1(orientation);
    UVSize size2(orientation);
    UVSize size3(orientation, constraintSize.get_Width(), constraintSize.get_Height());
    
    float itemWidth = get_ItemWidth();
    float itemHeight = get_ItemHeight();
    bool hasWidth = !Float::IsNaN(itemWidth);
    bool hasHeight = !Float::IsNaN(itemHeight);
    Size childSize(hasWidth ? itemWidth : constraintSize.get_Width(), hasHeight ? itemHeight : constraintSize.get_Height());
    UIElementCollection& children = *(get_Children());
    int cElts = children.get_Count();

    for(int i = 0; i < cElts; i++)
    {
        UIElement* pUIElement = children[i];
        if (pUIElement != NULL)
        {
              pUIElement->Measure(childSize);
              UVSize size5(orientation, hasWidth ? itemWidth : pUIElement->get_DesiredSize().get_Width(),
                           hasHeight ? itemHeight : pUIElement->get_DesiredSize().get_Height());

              if ((size1.U + size5.U) > size3.U)
              {
                    size2.U = Float::Max(size1.U, size2.U);
                    size2.V += size1.V;
                    size1 = size5;
                    if (size5.U > size3.U)
                    {
                          size2.U = Float::Max(size5.U, size2.U);
                          size2.V += size5.V;
                          size1 = UVSize(orientation);
                    }
              }
              else
              {
                    size1.U += size5.U;
                    size1.V = Float::Max(size5.V, size1.V);
              }
        }
    }
    size2.U = Float::Max(size1.U, size2.U);
    size2.V += size1.V;
    
    return Size(size2.get_Width(), size2.get_Height());
}

Size WrapPanel::ArrangeOverride(const Size& arrangeSize)
{
    int num1 = 0;
    float itemWidth = get_ItemWidth();
    float itemHeight = get_ItemHeight();
    float num4 = 0;
    Orientation::Enum orientation = get_Orientation();

    float num5 = (orientation == Orientation::Horizontal) ? itemWidth : itemHeight;

    UVSize size1(orientation);
    UVSize size2(orientation, arrangeSize.get_Width(), arrangeSize.get_Height());
    bool hasWidth = !Float::IsNaN(itemWidth);
    bool hasHeight = !Float::IsNaN(itemHeight);
    bool flag3 = (orientation == Orientation::Horizontal) ? hasWidth : hasHeight;

    UIElementCollection& children = *(get_Children());
    int cElts = children.get_Count();

    for(int i = 0; i < cElts; i++)
    {
        UIElement* pElement = children[i];
        if (pElement != NULL)
        {
              UVSize size3(orientation, hasWidth ? itemWidth : pElement->get_DesiredSize().get_Width(), hasHeight ? itemHeight : pElement->get_DesiredSize().get_Height());
              if ((size1.U + size3.U) > size2.U)
              {
                    ArrangeLine(num4, size1.V, num1, i, flag3, num5);
                    num4 += size1.V;
                    size1 = size3;
                    if (size3.U > size2.U)
                    {
                          ArrangeLine(num4, size3.V, i, ++i, flag3, num5);
                          num4 += size3.V;
                          size1 = UVSize(orientation);
                    }
                    num1 = i;
              }
              else
              {
                    size1.U += size3.U;
                    size1.V = Float::Max(size3.V, size1.V);
              }
        }
    }
    if (num1 < children.get_Count())
    {
        ArrangeLine(num4, size1.V, num1, children.get_Count(), flag3, num5);
    }
    return arrangeSize;
}


void WrapPanel::ArrangeLine(float v, float lineV, int start, int end, bool useItemU, float itemU)
{
    float num1 = 0;
    Orientation::Enum orientation = get_Orientation();
    bool flag1 = orientation == Orientation::Horizontal;
    
    UIElementCollection& children = *(get_Children());
    for (int num2 = start; num2 < end; num2++)
    {
        UIElement* pElement = children[num2];
        if (pElement != NULL)
        {
              UVSize size1(orientation, pElement->get_DesiredSize().get_Width(), pElement->get_DesiredSize().get_Height());
              float num3 = useItemU ? itemU : size1.U;
              pElement->Arrange(Rect(flag1 ? num1 : v, flag1 ? v : num1, flag1 ? num3 : lineV, flag1 ? lineV : num3));
              num1 += num3;
        }
    }
}

 


}; // namespace AVUI
