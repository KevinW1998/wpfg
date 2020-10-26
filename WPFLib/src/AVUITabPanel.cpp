#include <AVUICommon.h>
#include <AVUITabPanel.h>
#include <AVUITabControl.h>

REGISTER_ELEMENT_BEGIN(AVUI::TabPanel)
    REGISTER_DEFAULT_FACTORY()

REGISTER_ELEMENT_END()

namespace AVUI {

TabPanel::TabPanel()
{

}

TabPanel::~TabPanel()
{

}

Size TabPanel::MeasureOverride(const Size& constraintSize)
{
    Size size;
    Dock::Enum tabStripPlacement = get_TabStripPlacement();
    m_numRows = 1;
    m_rowHeight = 0;


    UIElementCollection* pChildren = get_Children();
    if (tabStripPlacement != Dock::Top && tabStripPlacement != Dock::Bottom)
    {
        if (tabStripPlacement == Dock::Left || tabStripPlacement == Dock::Right)
        {
            for(int idx = 0; idx < pChildren->get_Count(); idx++)
            {
                UIElement* pChild = (*pChildren)[idx];

                if (pChild->get_Visibility() == Visibility::Collapsed)
                {
                    continue;
                }
                pChild->Measure(constraintSize);
                Size desiredSizeWithoutMargin = GetDesiredSizeWithoutMargin(pChild);
                if (size.get_Width() < desiredSizeWithoutMargin.get_Width())
                {
                    size.set_Width(desiredSizeWithoutMargin.get_Width());
                }
                size.set_Height(size.get_Height() + desiredSizeWithoutMargin.get_Height());
            }
        }
        return size;
    }

    int num = 0;
    float width = 0;
    float num3 = 0;
    for(int idx = 0; idx < pChildren->get_Count(); idx++)
    {
        UIElement* pChild = (*pChildren)[idx];

        if (pChild->get_Visibility() == Visibility::Collapsed)
        {
            continue;
        }

        pChild->Measure(constraintSize);
        Size size2 = GetDesiredSizeWithoutMargin(pChild);
        if (m_rowHeight < size2.get_Height())
        {
            m_rowHeight = size2.get_Height();
        }
        if (((width + size2.get_Width()) > constraintSize.get_Width()) && (num > 0))
        {
            if (num3 < width)
            {
                num3 = width;
            }
            width = size2.get_Width();
            num = 1;
            m_numRows++;
            continue;
        }
        width += size2.get_Width();
        num++;
    }
    if (num3 < width)
    {
        num3 = width;
    }
    size.set_Height(m_rowHeight * m_numRows);
    if ((Float::IsInfinity(size.get_Width()) || Float::IsNaN(size.get_Width())) || (num3 < constraintSize.get_Width()))
    {
        size.set_Width(num3);
    }
    else
    {
        size.set_Width(constraintSize.get_Width());
    }

    return size;
}

Size TabPanel::ArrangeOverride(const Size& arrangeSize)
{
    switch (get_TabStripPlacement())
    {
    case Dock::Top:
    case Dock::Bottom:
        ArrangeHorizontal(arrangeSize);
        break;

    case Dock::Left:
    case Dock::Right:
        ArrangeVertical(arrangeSize);
        break;
    }
    return arrangeSize;
}

void TabPanel::ArrangeVertical(const Size& arrangeSize)
{
    float y = 0;
    UIElementCollection* pChildren = get_Children();

    for(int idx = 0; idx < pChildren->get_Count(); idx++)
    {
        UIElement* pChild = (*pChildren)[idx];

        if (pChild->get_Visibility() != Visibility::Collapsed)
        {
            Size desiredSizeWithoutMargin = GetDesiredSizeWithoutMargin(pChild);
            pChild->Arrange(Rect(0, y, arrangeSize.get_Width(), desiredSizeWithoutMargin.get_Height()));
            y += desiredSizeWithoutMargin.get_Height();
        }
    }
}

 

 


void TabPanel::ArrangeHorizontal(const Size& arrangeSize)
{
    Dock::Enum tabStripPlacement = get_TabStripPlacement();
    bool flag = m_numRows > 1;
    int activeRow = 0;
    ItemStructList<int> solution;
    ItemStructList<float> headerWidth;
    Vector vector;

    GetHeadersSize(&headerWidth);

    if (flag)
    {
        CalculateHeaderDistribution(arrangeSize.get_Width(), &headerWidth, &solution);
        activeRow = GetActiveRow(&solution);
        if (tabStripPlacement == Dock::Top)
        {
            vector.set_Y((m_numRows - 1 - activeRow) * m_rowHeight);
        }
        if (tabStripPlacement == Dock::Bottom && activeRow != 0)
        {
            vector.set_Y((m_numRows - activeRow) * m_rowHeight);
        }
    }
    int index = 0;
    int num3 = 0;

    UIElementCollection* pChildren = get_Children();

    for(int idx = 0; idx < pChildren->get_Count(); idx++)
    {
        UIElement* pChild = (*pChildren)[idx];

        Thickness thickness = pChild->get_Margin();
        float left = thickness.get_Left();
        float right = thickness.get_Right();
        float top = thickness.get_Top();
        float bottom = thickness.get_Bottom();
        bool flag2 = flag && (((num3 < solution.get_Count()) && solution[num3] == index) || (index == pChildren->get_Count() - 1));
        Size size(headerWidth[index], m_rowHeight);
        if (flag2)
        {
            size.set_Width(arrangeSize.get_Width() - vector.get_X());
        }

        pChild->Arrange(Rect(vector.get_X(), vector.get_Y(), size.get_Width(), size.get_Height()));

        Size size2 = size;

        size2.set_Height(Float::Max(0, (size2.get_Height() - top - bottom)));
        size2.set_Width(Float::Max(0, (size2.get_Width() - left - right)));
        vector.set_X(vector.get_X() + size.get_Width());
        if (flag2)
        {
            if ((num3 == activeRow && tabStripPlacement == Dock::Top) || (num3 == activeRow - 1 && tabStripPlacement == Dock::Bottom))
            {
                vector.set_Y(0);
            }
            else
            {
                vector.set_Y(vector.get_Y() + m_rowHeight);
            }
            vector.set_X(0);
            num3++;
        }
        index++;
    }
}

Dock::Enum TabPanel::get_TabStripPlacement()
{
    Dock::Enum top = Dock::Top;
    TabControl* pControl = object_cast<TabControl>(get_TemplatedParent());
    if (pControl != NULL)
    {
        top = pControl->get_TabStripPlacement();
    }
    return top;
}

Size TabPanel::GetDesiredSizeWithoutMargin(UIElement* pElement)
{
    Thickness thickness = pElement->get_Margin();
    Size size;
    size.set_Height(Float::Max(0, (pElement->get_DesiredSize().get_Height() - thickness.get_Top()) - thickness.get_Bottom()));
    size.set_Width(Float::Max(0, (pElement->get_DesiredSize().get_Width() - thickness.get_Left()) - thickness.get_Right()));

    return size;
}

void TabPanel::GetHeadersSize(ItemStructList<float>* pNumArray)
{
    UIElementCollection* pChildren = get_Children();

    pNumArray->Clear();
    pNumArray->AddEmptyItems(pChildren->get_Count());

    int index = 0;
    for(int idx = 0; idx < pChildren->get_Count(); idx++)
    {
        UIElement* pChild = (*pChildren)[idx];

        Size desiredSizeWithoutMargin = GetDesiredSizeWithoutMargin(pChild);
        (*pNumArray)[index] = (pChild->get_Visibility() == Visibility::Collapsed) ? 0 : desiredSizeWithoutMargin.get_Width();
        index++;
    }
}

int TabPanel::GetActiveRow(ItemStructList<int>* pSolution)
{
    int index = 0;
    int num2 = 0;
    if (pSolution->get_Count() > 0)
    {
        UIElementCollection* pChildren = get_Children();

        for(int idx = 0; idx < pChildren->get_Count(); idx++)
        {
            UIElement* pChild = (*pChildren)[idx];

            if (UnboxValue<bool>(pChild->GetValue(Selector::get_IsSelectedProperty())))
            {
                return index;
            }
            if (index < pSolution->get_Count() && (*pSolution)[index] == num2)
            {
                index++;
            }
            num2++;
        }
    }
    if (get_TabStripPlacement() == Dock::Top)
    {
        index = m_numRows - 1;
    }
    return index;
}

template <class T>
static void ArrayCopy(ItemStructList<T>* pListFrom, ItemStructList<T>* pListTo)
{
    for(int i = 0; i < pListFrom->get_Count(); i++)
    {
        (*pListTo)[i] = (*pListFrom)[i];
    }
}

void TabPanel::CalculateHeaderDistribution(float rowWidthLimit, ItemStructList<float>* pHeaderWidth, ItemStructList<int>* pSolution)
{
    float num = 0;
    int length = pHeaderWidth->get_Count();
    int num3 = m_numRows - 1;
    float num4 = 0;
    int num5 = 0;
    float num6 = 0;
    ItemStructList<int> numArray;
    numArray.AddEmptyItems(num3);

    pSolution->Clear();
    pSolution->AddEmptyItems(num3);

    ItemStructList<int> numArray3;
    numArray3.AddEmptyItems(m_numRows);

    ItemStructList<float> numArray4;
    numArray4.AddEmptyItems(m_numRows);

    ItemStructList<float> numArray5;
    numArray5.AddEmptyItems(m_numRows);

    ItemStructList<float> numArray6;
    numArray6.AddEmptyItems(m_numRows);

    int index = 0;
    for (int i = 0; i < length; i++)
    {
        if (((num4 + (*pHeaderWidth)[i]) > rowWidthLimit) && (num5 > 0))
        {
            numArray4[index] = num4;
            numArray3[index] = num5;
            num6 = Float::Max(0, ((rowWidthLimit - num4) / ((float) num5)));
            numArray5[index] = num6;
            numArray[index] = i - 1;
            if (num < num6)
            {
                num = num6;
            }
            index++;
            num4 = (*pHeaderWidth)[i];
            num5 = 1;
        }
        else
        {
            num4 += (*pHeaderWidth)[i];
            if ((*pHeaderWidth)[i] != 0)
            {
                num5++;
            }
        }
    }
    if (index == 0)
    {
        pSolution->Clear();
        return;
    }

    numArray4[index] = num4;
    numArray3[index] = num5;
    num6 = (rowWidthLimit - num4) / ((float) num5);
    numArray5[index] = num6;
    if (num < num6)
    {
        num = num6;
    }

    ArrayCopy(&numArray, pSolution);
    ArrayCopy(&numArray5, &numArray6);
    while (true)
    {
        int num9 = 0;
        float num10 = 0;
        for (int j = 0; j < m_numRows; j++)
        {
            if (num10 < numArray5[j])
            {
                num10 = numArray5[j];
                num9 = j;
            }
        }
        if (num9 == 0)
        {
            break;
        }
        int num12 = num9;
        int num13 = num12 - 1;
        int num14 = numArray[num13];
        float num15 = (*pHeaderWidth)[num14];
        numArray4[num12] += num15;
        if (numArray4[num12] > rowWidthLimit)
        {
            break;
        }
        numArray[num13]--;
        numArray3[num12]++;
        numArray4[num13] -= num15;
        numArray3[num13]--;
        numArray5[num13] = (rowWidthLimit - numArray4[num13]) / ((float) numArray3[num13]);
        numArray5[num12] = (rowWidthLimit - numArray4[num12]) / ((float) numArray3[num12]);
        num10 = 0;
        for (int k = 0; k < m_numRows; k++)
        {
            if (num10 < numArray5[k])
            {
                num10 = numArray5[k];
            }
        }
        if (num10 < num)
        {
            num = num10;

            ArrayCopy(&numArray, pSolution);
            ArrayCopy(&numArray5, &numArray6);
        }
    }
    index = 0;
    for (int m = 0; m < length; m++)
    {
        (*pHeaderWidth)[m] += numArray6[index];
        if ((index < num3) && ((*pSolution)[index] == m))
        {
            index++;
        }
    }
}

 
 

 
 

 
 

 


}; // namespace AVUI
