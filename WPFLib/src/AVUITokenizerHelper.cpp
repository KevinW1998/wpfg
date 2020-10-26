#include <AVUICommon.h>
#include <AVUITokenizerHelper.h>

namespace AVUI
{

TokenizerHelper::TokenizerHelper(const String* pString)
{
    Initialize(pString, L'\'', L',');
}

TokenizerHelper::TokenizerHelper(const String* pString, WCHAR quoteChar, WCHAR separator)
{
    Initialize(pString, quoteChar, separator);
}

StringPtr TokenizerHelper::GetCurrentToken()
{
    if(m_currentTokenIndex < 0)
    {
        return NULL;
    }

    return m_pString->Substring(m_currentTokenIndex, m_currentTokenLength);
}

WCHAR TokenizerHelper::GetNumericListSeparator()
{
    return L',';
}

void TokenizerHelper::Initialize(const String* pString, WCHAR quoteChar, WCHAR separator)
{
    m_pString = (String*) pString;
    m_cchString = (pString == NULL) ? 0 : pString->get_Cch();
    m_currentTokenIndex = -1;
    m_quoteChar = quoteChar;
    m_argSeparator = separator;
    m_charIndex = 0;

    while(m_charIndex < m_cchString)
    {
        if(!String::IsWhiteSpace((*pString)[m_charIndex]))
        {
            return;
        }
        m_charIndex++;
    }
}

void TokenizerHelper::LastTokenRequired()
{
    if(m_charIndex != m_cchString)
    {
        LibraryCriticalError();
    }
}

bool TokenizerHelper::NextToken(bool allowQuotedToken, WCHAR separator)
{
    m_currentTokenIndex = -1;
    m_foundSeparator = false;
    if (m_charIndex >= m_cchString)
    {
        return false;
    }

    WCHAR c = (*m_pString)[m_charIndex];
    int num = 0;
    if (allowQuotedToken && (c == m_quoteChar))
    {
        num++;
        m_charIndex++;
    }
    int num2 = m_charIndex;
    int num3 = 0;
    while (m_charIndex < m_cchString)
    {
        c = (*m_pString)[m_charIndex];
        if (num > 0)
        {
            if (c != m_quoteChar)
            {
                m_charIndex++;
                num3++;
                continue;
            }
            num--;
            if (num != 0)
            {
                m_charIndex++;
                num3++;
                continue;
            }
            m_charIndex++;
            break;
        }
        if (String::IsWhiteSpace(c) || (c == separator))
        {
            if (c == separator)
            {
                m_foundSeparator = true;
            }
            break;
        }
        m_charIndex++;
        num3++;
    }

    if (num > 0)
    {
        LibraryCriticalError();
    }

    ScanToNextToken(separator);
    m_currentTokenIndex = num2;
    m_currentTokenLength = num3;
    if (m_currentTokenLength < 1)
    {
        LibraryCriticalError();
    }
    return true;
}

StringPtr TokenizerHelper::NextTokenRequired()
{
    if(!NextToken(false))
    {
        LibraryCriticalError();
    }

    return GetCurrentToken();
}

StringPtr TokenizerHelper::NextTokenRequired(bool allowQuotedToken)
{
    if(!NextToken(allowQuotedToken))
    {
        LibraryCriticalError();
    }

    return GetCurrentToken();
}

void TokenizerHelper::ScanToNextToken(WCHAR separator)
{
    if (m_charIndex < m_cchString)
    {
        WCHAR c = (*m_pString)[m_charIndex];

        if ((c != separator) && !String::IsWhiteSpace(c))
        {
            LibraryCriticalError();
        }

        int num = 0;
        while (m_charIndex < m_cchString)
        {
            c = (*m_pString)[m_charIndex];
            if (c == separator)
            {
                m_foundSeparator = true;
                num++;
                m_charIndex++;
                if (num > 1)
                {
                    LibraryCriticalError();
                }
            }
            else
            {
                if (!String::IsWhiteSpace(c))
                {
                    break;
                }
                m_charIndex++;
            }
        }
        if ((num > 0) && (m_charIndex >= m_cchString))
        {
            LibraryCriticalError();
        }
    }
}




}
