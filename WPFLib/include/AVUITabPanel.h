#include <AVUIPanel.h>

namespace AVUI {


class TabPanel : public Panel
{
public:
    TabPanel();
    ~TabPanel();


    Size MeasureOverride(const Size& constraintSize);
    Size ArrangeOverride(const Size& constraintSize);

    DECLARE_ELEMENT(TabPanel, Panel);

private:

    void CalculateHeaderDistribution(float rowWidthLimit, ItemStructList<float>* pHeaderWidth, ItemStructList<int>* pSolution);
    int GetActiveRow(ItemStructList<int>* pSolution);
    void GetHeadersSize(ItemStructList<float>* pNumArray);
    void ArrangeHorizontal(const Size& arrangeSize);
    void ArrangeVertical(const Size& arrangeSize);
    Size GetDesiredSizeWithoutMargin(UIElement* pElement);
    Dock::Enum get_TabStripPlacement();

    int m_numRows;
    float m_rowHeight;
};

}; // namespace AVUI

