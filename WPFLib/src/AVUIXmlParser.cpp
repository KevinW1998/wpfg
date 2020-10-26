#include <AVUICommon.h>
#include <AVUIXmlParser.h>
#include <AVUIXamlParser.h>

namespace AVUI
{

XmlParser::XmlParser(IXMLParserDataProvider* pDataSource, XamlParser* pXMLSink) : m_pDataSource(pDataSource), m_pXMLSink(pXMLSink),
                                                                                            m_cbBuffered(0), m_idxBuffer(0),
                                                                                            m_wchCurrent(0), m_isAtEOF(false)
    
{
    m_rgBufferedChars.AddEmptyItems(mc_cbBufferMax);
    m_currentLine = 1;
}

XmlParser::~XmlParser()
{
}

void XmlParser::Parse()
{
    m_pXMLSink->StartDocument();

    ReadBuffer();

    if(m_cbBuffered >= 3)
    {
        if(m_rgBufferedChars[0] == 0xEF &&
           m_rgBufferedChars[1] == 0xBB &&
           m_rgBufferedChars[2] == 0xBF)
        {
            m_idxBuffer += 3;
        }
    }

    AdvanceToNextCharacter();
    SkipWhitespace();

    if(!IsAtEOF() && m_wchCurrent != '<')
    {
        LibraryCriticalError();
    }

    while(!IsAtEOF())
    {
        ParseNextSection();
    }

    m_pXMLSink->EndDocument();
}

// TODO -- Unicode and UTF-8 support?
void XmlParser::AdvanceToNextCharacter()
{
    if(IsAtEOF())
    {
        return;
    }

    if(m_cbBuffered == m_idxBuffer)
    {
        ReadBuffer();
        m_idxBuffer = 0;
    }

    m_wchCurrent = m_rgBufferedChars[m_idxBuffer];
    m_idxBuffer++;
}


void XmlParser::ParseNextSection()
{
    if(m_wchCurrent == L'<')
    {
        AdvanceToNextCharacter();
        ParseNextTag();
    }
    else
    {
        // data data
        m_rgTagData.Clear();
        while(m_wchCurrent != L'<')
        {
            m_rgTagData.AddItem(m_wchCurrent);

            AdvanceToNextCharacter();
            if(IsAtEOF())
            {
                LibraryCriticalError();
            }
        }

        m_pXMLSink->Characters(m_rgTagData.get_Ptr(), m_rgTagData.get_Count());
        m_rgTagData.Clear();
    }

}

void XmlParser::ParseComment()
{
    if(m_wchCurrent != L'!')
    {
        LibraryCriticalError();
    }

    AdvanceToNextCharacter();

    if(m_wchCurrent != L'-')
    {
        LibraryCriticalError();
    }

    AdvanceToNextCharacter();

    if(m_wchCurrent != L'-')
    {
        LibraryCriticalError();
    }

    AdvanceToNextCharacter();

    if(!IsWhitespace(m_wchCurrent))
    {
        LibraryCriticalError();
    }


    int endIdx = 0;

    while(!IsAtEOF() && endIdx != 3)
    {
        if(m_wchCurrent == '\n')
        {
            m_pXMLSink->set_CurrentLine(++m_currentLine);
        }

        if(m_wchCurrent == L'-' && endIdx < 2)
        {
            endIdx++;
        }
        else if(m_wchCurrent == L'>' && endIdx == 2)
        {
            endIdx++;
        }
        else
        {
            endIdx = 0;
        }

        AdvanceToNextCharacter();
    }
}

void XmlParser::ParseNextTag()
{
    bool isClosingTag = false;

    SkipWhitespace();

    if(m_wchCurrent == L'/')
    {
        isClosingTag = true;
    }

    if(m_wchCurrent == L'!')
    {
        ParseComment();
        SkipWhitespace();
        return;
    }

    ReadNextName();

    if(isClosingTag)
    {
        if(m_wchCurrent != L'>')
        {
            LibraryCriticalError();
        }
        m_pXMLSink->EndElement();
    }
    else
    {
        m_pXMLSink->StartElement(m_rgTagData.get_Ptr(), m_rgTagData.get_Count());

        if(m_wchCurrent != L'/' && m_wchCurrent != L'>')
        {
            ParseAttributes();
        }

        if(m_wchCurrent == L'/')
        {
            AdvanceToNextCharacter();
            SkipWhitespace();
            m_pXMLSink->EndElement();
        }

        if(m_wchCurrent != L'>')
        {
            LibraryCriticalError();
        }

    }

    AdvanceToNextCharacter();
    SkipWhitespace();
}

void XmlParser::ReadNextName()
{
    m_rgTagData.Clear();
    while(!IsWhitespace(m_wchCurrent) && !IsAtEOF() && m_wchCurrent != L'=' && m_wchCurrent != L'>'
          && (m_wchCurrent != L'/' || m_rgTagData.get_Count() == 0) )
    {
        m_rgTagData.AddItem(m_wchCurrent);

        AdvanceToNextCharacter();
    }

    SkipWhitespace();
}

void XmlParser::SkipWhitespace()
{
    while(!IsAtEOF() && IsWhitespace(m_wchCurrent))
    {
        if(m_wchCurrent == '\n')
        {
            m_pXMLSink->set_CurrentLine(++m_currentLine);
        }
        AdvanceToNextCharacter();
    }
}

bool XmlParser::IsWhitespace(WCHAR wch)
{
    if(wch == L'\t' || wch == L' ' || wch == '\r' || wch == '\n')
    {
        return true;
    }

    return false;
}


void XmlParser::ReadBuffer()
{
    m_pDataSource->ReadData(m_rgBufferedChars.get_Ptr(), mc_cbBufferMax, &m_cbBuffered);

    if(m_cbBuffered != mc_cbBufferMax)
    {
        m_isAtEOF = true;
    }
}

void XmlParser::ParseAttributes()
{
    SkipWhitespace();

    m_pXMLSink->BeginAttributes();

    while(m_wchCurrent != L'/' && m_wchCurrent != L'>')
    {
        ParseAttribute();
        SkipWhitespace();
    }

    m_pXMLSink->EndAttributes();
}

void XmlParser::ParseAttribute()
{
    ReadNextName();

    SkipWhitespace();
    if(m_wchCurrent != L'=')
    {
        LibraryCriticalError();
    }
    AdvanceToNextCharacter();
    SkipWhitespace();

    WCHAR quoteChar = m_wchCurrent;
    if(quoteChar != L'\'' && quoteChar != L'\"')
    {
        LibraryCriticalError();
    }

    AdvanceToNextCharacter();

    int idxNameEnd = m_rgTagData.get_Count();

    while(m_wchCurrent != quoteChar)
    {
        m_rgTagData.AddItem(m_wchCurrent);

        AdvanceToNextCharacter();

        if(IsAtEOF())
        {
            LibraryCriticalErrorMessage("Unterminated quote at line %d", m_currentLine);
        }
    }    

    m_pXMLSink->AddAttribute(m_rgTagData.get_Ptr(), idxNameEnd, m_rgTagData.get_Ptr() + idxNameEnd, m_rgTagData.get_Count() - idxNameEnd);

    AdvanceToNextCharacter();
}



}; // namespace AVUI
