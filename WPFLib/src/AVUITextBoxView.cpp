#include <AVUICommon.h>
#include <AVUITextBoxView.h>
#include <AVUIFont.h>
#include <AVUICaretElement.h>
#include <AVUIFormattedText.h>
#include <AVUICursors.h>

REGISTER_ELEMENT_BEGIN(AVUI::TextBoxView)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_PROPERTY(Text);
    REGISTER_PROPERTY(Foreground);
    REGISTER_PROPERTY(FontFamily);
    REGISTER_PROPERTY(FontSize);
REGISTER_ELEMENT_END()


namespace AVUI {

TextBoxView::TextBoxView() :  m_isCaretVisible(false), m_caretIndex(0), m_pTextBox(NULL) 
{ 
};

TextBoxView::~TextBoxView()
{
}

Size TextBoxView::MeasureOverride(const Size& constraintSize)
{
    String* pString = get_Text();
    String* pFontFamily = get_FontFamily();
    float fontSize = get_FontSize();

    m_pFormattedText = NULL;

    if(pFontFamily == NULL || fontSize <= 0.0f)
    {
        return Size(0, 0);
    }

    if(pString == NULL)
    {
        return Size(0, get_FontSize());
    }

    Font font(pFontFamily->get_Ptr(), fontSize);

    m_pFormattedText = font.CreateFormattedText(pString->get_Ptr(), pString->get_Cch(), constraintSize.get_Width(), false);

    return Size(m_pFormattedText->get_Width(), m_pFormattedText->get_Height());
}

Size TextBoxView::ArrangeOverride(const Size& finalSize)
{    
    return finalSize;
}

 
void TextBoxView::OnRender(DrawingContext& drawingContext)
{
    if(m_pFormattedText != NULL)
    {
        drawingContext.DrawFormattedText(get_Foreground(), m_pFormattedText, Point());
    }
    set_CaretIndex(m_caretIndex);
}

void TextBoxView::ShowCaret()
{
    m_isCaretVisible = true;
    if(m_pCaretElement == NULL)
    {
        m_pCaretElement = CaretElementPtr(new CaretElement(), false);
        AddVisualChild(m_pCaretElement);
    }
    m_pCaretElement->set_Height(get_FontSize());
    m_pCaretElement->set_Brush(get_Foreground());
    m_pCaretElement->ShowCaret();
    m_pCaretElement->BringIntoView();
}

void TextBoxView::HideCaret()
{
    m_isCaretVisible = false;
    if(m_pCaretElement != NULL)
    {
        RemoveVisualChild(m_pCaretElement);
        m_pCaretElement = NULL;
    }
}


unsigned int TextBoxView::get_VisualChildCount()
{
    if(m_isCaretVisible)
    {
        return 1;
    }
    return 0;
}

Visual* TextBoxView::get_VisualChild(unsigned int idx)
{
    return m_pCaretElement;
}

void TextBoxView::set_CaretIndex(int idx)
{
    m_caretIndex = idx;

    if(m_pFormattedText == NULL)
    {
        return;
    }

    if(m_pCaretElement != NULL)
    {
        m_pCaretElement->set_VisualOffset(Vector(m_pFormattedText->GetXLocationBeforeIndex(idx), 0.0f));
        if(m_isCaretVisible)
        {
            m_pCaretElement->BringIntoView();
        }
    }
}

int TextBoxView::get_CaretIndexFromPoint(const Point& point)
{
    if(m_pFormattedText == NULL)
    {
        return 0;
    }

    int i;

    for(i = 0; i < m_pFormattedText->get_Cch(); i++)
    {
        if(m_pFormattedText->GetXLocationBeforeIndex(i) > point.get_X())
        {
            if(i != 0 && Float::Abs(point.get_X() - m_pFormattedText->GetXLocationBeforeIndex(i - 1)) < Float::Abs(point.get_X() - m_pFormattedText->GetXLocationBeforeIndex(i)))
            {
                return i - 1;
            }
            return i;
        }
    }

    return i;
}

}; // namespace AVUI
