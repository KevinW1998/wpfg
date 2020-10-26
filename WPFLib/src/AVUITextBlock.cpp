#include <AVUICommon.h>
#include <AVUITextBlock.h>
#include <AVUIFont.h>
#include <AVUIFormattedText.h>
#include <AVUIRun.h>

REGISTER_ELEMENT_BEGIN(AVUI::TextBlock)
    REGISTER_DEFAULT_FACTORY()

    REGISTER_INTERFACE(IAddChild);
    REGISTER_PROPERTY(Text);
    REGISTER_PROPERTY(Foreground);
    REGISTER_PROPERTY(FontFamily);
    REGISTER_PROPERTY(FontSize);
    REGISTER_PROPERTY(TextWrapping);
    REGISTER_PROPERTY(TextTrimming);
    REGISTER_PROPERTY(TextAlignment);
REGISTER_ELEMENT_END()

REGISTER_INTERFACE_TYPE(AVUI::IAddChild);

namespace AVUI {

TextBlock::~TextBlock()
{
    ClearFormattedText();
}

Size TextBlock::MeasureOverride(const Size& constraintSize)
{
    String* pString = get_Text();
    String* pFontFamily = get_FontFamily();
    float fontSize = get_FontSize();
    TextTrimming::Enum textTrimming = get_TextTrimming();

    ClearFormattedText();

    if(pString == NULL || pFontFamily == NULL || fontSize <= 0.0f)
    {
        return Size(0, 0);
    }

    Font font(pFontFamily->get_Ptr(), fontSize);

    int cchMax = pString->get_Cch();
    int cchCur = 0;
    bool isWrapped = get_TextWrapping() == TextWrapping::Wrap;

    float maxX = 0.0f;
    float totalY = 0.0f;

    while(cchCur < cchMax)
    {
        TRefCountedPtr<FormattedText> pFormattedText = font.CreateFormattedText(pString->get_Ptr() + cchCur, cchMax - cchCur, constraintSize.get_Width(), isWrapped);

        m_formattedText.AddItem(pFormattedText);
        cchCur += pFormattedText->get_Cch();

        float collapsedWidth = pFormattedText->get_Width();
        if(collapsedWidth > constraintSize.get_Width() && textTrimming != TextTrimming::None)
        {
            collapsedWidth = constraintSize.get_Width();
        }
        maxX = Float::Max(maxX, collapsedWidth);
        totalY += pFormattedText->get_Height();
    }

    return Size(maxX, totalY);
}

Size TextBlock::ArrangeOverride(const Size& finalSize)
{    
    bool isTrimmed = get_TextTrimming() == TextTrimming::CharacterEllipsis;

    for(int idx = 0; idx < m_formattedText.get_Count(); idx++)
    {
        if(idx != m_formattedText.get_Count() -1 && m_formattedText[idx]->get_Width() < finalSize.get_Width())
        {
            continue;
        }

        if(isTrimmed)
        {
            m_formattedText[idx]->Trim(finalSize.get_Width());
        }
        else
        {
            m_formattedText[idx]->Trim(Float::get_PositiveInfinity());
        }
    }

    return finalSize;
}

void TextBlock::ClearFormattedText()
{
    m_formattedText.Clear();
}

 
void TextBlock::OnRender(DrawingContext& drawingContext)
{
    TextAlignment::Enum textAlignment = get_TextAlignment();
    float yPos = 0.0f;

    Brush* pBrush = get_Foreground();


    for(int idx = 0; idx < m_formattedText.get_Count(); idx++)
    {
        if(yPos > get_RenderSize().get_Height())
        {
            break;
        }

        float xPos = 0.0f;

        if(textAlignment == TextAlignment::Center)
        {
            xPos = (get_RenderSize().get_Width() - m_formattedText[idx]->get_Width()) / 2.0f;
        } 
        else if (textAlignment == TextAlignment::Right)
        {
            xPos = get_RenderSize().get_Width() - m_formattedText[idx]->get_Width();
        }

        drawingContext.DrawFormattedText(pBrush, m_formattedText[idx], Point(xPos, yPos));
        yPos += m_formattedText[idx]->get_Height();
    }
}

void TextBlock::AddChild(Object* pValue)
{
    if(pValue == NULL)
    {
        return;
    }

    if(pValue->IsType(GET_TYPE(Run)))
    {
        set_Text(((Run*)pValue)->get_Text());
    }
}

void TextBlock::AddText(String* pText)
{

}



}; // namespace AVUI
