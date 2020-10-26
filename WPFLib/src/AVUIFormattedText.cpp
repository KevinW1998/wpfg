#include <AVUICommon.h>
#include <AVUIFormattedText.h>
#include <AVUIIClientRendererInterface.h>

namespace AVUI {

FormattedText::~FormattedText()
{
    if(m_pClientFormattedText != NULL)
    {
        m_pClientFormattedText->Destroy();
    }
}

float FormattedText::get_Width() const
{
    if(m_pClientFormattedText != NULL)
    {
        return m_pClientFormattedText->get_Width();
    }

    return 0.0f;
}

float FormattedText::get_Height() const
{
    if(m_pClientFormattedText != NULL)
    {
        return m_pClientFormattedText->get_Height();
    }

    return 0.0f;
}

int FormattedText::get_Cch() const
{
    if(m_pClientFormattedText != NULL)
    {
        return m_pClientFormattedText->get_Cch();
    }

    return 0;
}


void FormattedText::Trim(float trimWidth)
{
    if(m_pClientFormattedText != NULL)
    {
        m_pClientFormattedText->Trim(trimWidth);
    }
}

float FormattedText::GetXLocationBeforeIndex(int index) const
{
    return m_pClientFormattedText->GetXLocationBeforeIndex(index);
}


}; // namespace AVUI
