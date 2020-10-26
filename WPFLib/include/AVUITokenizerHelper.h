#pragma once

#include <AVUIUIString.h>

namespace AVUI
{

class TokenizerHelper
{
public:

    TokenizerHelper(const String* pString);
    TokenizerHelper(const String* pString, WCHAR quoteChar, WCHAR separator);

    StringPtr GetCurrentToken();
    static WCHAR GetNumericListSeparator();
    void LastTokenRequired();
    bool NextToken() { return NextToken(false); }
    bool NextToken(bool allowQuotedToken) { return NextToken(allowQuotedToken,m_argSeparator); }
    bool NextToken(bool allowQuotedToken, WCHAR separator);
    StringPtr NextTokenRequired();
    StringPtr NextTokenRequired(bool allowQuotedToken);
    void ScanToNextToken(WCHAR separator);

    bool get_FoundSeparator() const { return m_foundSeparator; }

private:
    void Initialize(const String* pString, WCHAR quoteChar, WCHAR separator);

    WCHAR m_argSeparator;
    int m_charIndex;
    int m_currentTokenIndex;
    int m_currentTokenLength;
    bool m_foundSeparator;
    WCHAR m_quoteChar;
    StringPtr m_pString;
    int m_cchString;
};



};
