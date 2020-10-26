#include <AVUICommon.h>
#include <AVUIUIString.h>
#include <AVUIStringUtil.h>

REGISTER_ELEMENT_BEGIN(AVUI::String)
    REGISTER_TYPE_CONVERTER(StringTypeConverter)
REGISTER_ELEMENT_END()

namespace AVUI {

String::String(const char* szString, int cch)
{
    if(cch == -1)
    {
        cch = StringUtil::GetAnsiCch(szString);
    }
    m_cch = cch;

    m_wzData = (WCHAR*) Object::AllocateMemory((cch + 1) * sizeof(WCHAR));

    for(int i = 0; i < cch; i++)
    {
        m_wzData[i] = szString[i];
    }
    m_wzData[cch] = L'\0';
}

String::String(const WCHAR* wzString, int cch)
{
    if(cch == -1)
    {
        cch = StringUtil::GetCch(wzString);
    }
    m_cch = cch;

    m_wzData = (WCHAR*) Object::AllocateMemory((cch + 1) * sizeof(WCHAR));
    StringUtil::CopySubstring(m_wzData, cch + 1, wzString, cch);
    m_wzData[cch] = L'\0';
}

String::String(const String& strOther)
{
    m_cch = strOther.m_cch;
    m_wzData = NULL;

    if(m_cch > 0)
    {
        m_wzData = (WCHAR*) Object::AllocateMemory((m_cch + 1) * sizeof(WCHAR));
        StringUtil::CopySubstring(m_wzData, m_cch + 1, strOther.m_wzData, m_cch);
        m_wzData[m_cch] = '\0';
    }
}

String& String::operator=(const String& rhs)
{
    if(m_wzData != NULL)
    {
        Object::FreeMemory(m_wzData);
        m_wzData = NULL;
    }

    m_cch = rhs.m_cch;
    m_wzData = NULL;

    if(m_cch > 0)
    {
        m_wzData = (WCHAR*) Object::AllocateMemory((m_cch + 1) * sizeof(WCHAR));
        StringUtil::CopySubstring(m_wzData, m_cch + 1, rhs.m_wzData, m_cch);
        m_wzData[m_cch] = '\0';
    }

    return (*this);
}


int String::IndexOf(WCHAR wch, int startIndex) const
{
    for(int i = startIndex; i < m_cch; i++)
    {
        if(m_wzData[i] == wch)
        {
            return i;
        }
    }
    return -1;
}

int String::LastIndexOf(WCHAR wch) const
{
    for(int i = m_cch; i >= 0; i--)
    {
        if(m_wzData[i] == wch)
        {
            return i;
        }
    }
    return -1;
}



String::~String()
{
    if(m_wzData != NULL)
    {
        Object::FreeMemory(m_wzData);
        m_wzData = NULL;
    }
}

bool String::BeginsWith(const WCHAR* pwchOther, int cch) const
{
    if(cch > m_cch)
    {
        return false;
    }

    for(int i = 0; i < cch; i++)
    {
        if(m_wzData[i] != pwchOther[i])
        {
            return false;
        }
    }
    return true;
}

bool String::EndsWith(const WCHAR* pwchOther, int cch) const
{
    if(cch > m_cch)
    {
        return false;
    }

    for(int i = 0; i < cch; i++)
    {
        if(m_wzData[m_cch - cch + i] != pwchOther[i])
        {
            return false;
        }
    }
    return true;
}

bool String::Equals(const Object* pOther) const
{
    if(pOther->IsType(GET_TYPE(String)))
    {
        return (*this) == *(String*)pOther;
    }

    return false;
}

bool String::Equals(const WCHAR* pwchOther, bool isInsensitive, int cchOther) const
{
    if(cchOther == -1)
    {
        cchOther = StringUtil::GetCch(pwchOther);
    }

    if(cchOther != m_cch)
    {
        return false;
    }

    if(isInsensitive)
    {
        return StringUtil::CompareCaseInsensitiveLen(pwchOther, m_wzData, cchOther) == 0;
    }
    return StringUtil::CompareLen(pwchOther, m_wzData, cchOther) == 0;
}

bool String::Equals(const char* pchOther, bool isInsensitive, int cchOther) const
{
    if(cchOther == -1)
    {
        cchOther = StringUtil::GetAnsiCch(pchOther);
    }

    if(cchOther != m_cch)
    {
        return false;
    }

    String convertedString(pchOther, cchOther);

    return Equals(convertedString.get_Ptr(), isInsensitive, cchOther);

}

void String::Trim()
{
    if(m_cch == 0)
    {
        return;
    }

    int indexFirstChar = 0;
    int indexLastChar = m_cch;

    for(indexFirstChar = 0; indexFirstChar < m_cch; indexFirstChar++)
    {
        if(!IsWhiteSpace(m_wzData[indexFirstChar]))
        {
            break;
        }
    }

    // Copy null terminator
    if(indexFirstChar > 0)
    {
        for(int i = indexFirstChar; i <= m_cch; i++)
        {
            m_wzData[i - indexFirstChar] = m_wzData[i];
        }
    }
    m_cch = m_cch - indexFirstChar;

    for(int i = m_cch - 1; i >= 0; i--)
    {
        if(!IsWhiteSpace(m_wzData[i]))
        {
            m_wzData[i + 1] = L'\0';
            m_cch = i + 1;
            break;
        }
    }
}



void String::InsertAt(int idx, WCHAR wch)
{
    if(idx > m_cch)
    {
        LibraryCriticalError();
    }

    m_cch += 1;

    WCHAR* pwchNewData = (WCHAR*) Object::AllocateMemory((m_cch + 1) * sizeof(WCHAR));;

    if(m_wzData != NULL)
    {
        StringUtil::CopySubstring(pwchNewData, m_cch + 1, m_wzData, idx);
        StringUtil::CopySubstring(pwchNewData + idx + 1, m_cch - idx, m_wzData + idx, m_cch - idx);

        Object::FreeMemory(m_wzData);
    }
    else
    {
        pwchNewData[0] = L'\0';
    }

    m_wzData = pwchNewData;
    m_wzData[idx] = wch;
    if(idx == m_cch - 1)
    {
        m_wzData[m_cch] = L'\0';
    }
}

void String::DeleteAt(int idx)
{
    if(idx >= m_cch)
    {
        LibraryCriticalError();
    }

    for(idx; idx < m_cch; idx++)
    {
        m_wzData[idx] = m_wzData[idx + 1];
    }
    m_cch -= 1;
}

TRefCountedPtr<String> String::Substring(int index, int cch) const
{
    return object_allocate<String>((m_wzData + index), cch);
}

TRefCountedPtr<String> String::Substring(int index) const
{
    return Substring(index, m_cch - index);
}

TRefCountedPtr<String> String::ToLower() const
{
    TRefCountedPtr<String> pReturn = object_allocate<String>(m_wzData, m_cch);

    for(int idx = 0; idx < m_cch; idx++)
    {
        pReturn->m_wzData[idx] = StringUtil::ToLower(pReturn->m_wzData[idx]);
    }

    return pReturn;
}



bool String::operator==(const String& rhs) const
{
    return Equals(rhs.m_wzData, false, rhs.m_cch);
}

bool String::operator!=(const String& rhs) const
{
    return !Equals(rhs.m_wzData, false, rhs.m_cch);
}

void String::ConvertToAnsi(char* pszBuffer, unsigned int cchMaxBuffer)
{
    if(cchMaxBuffer == 0)
    {
        return;
    }

    unsigned int cchToWrite = Int::Min(cchMaxBuffer - 1, m_cch);

    for(unsigned int i = 0; i < cchToWrite; i++)
    {
        pszBuffer[i] = (char)m_wzData[i];
    }

    pszBuffer[cchToWrite] = '\0';
}

bool String::IsEqualToAnsi(char* pszAnsiStr)
{
    if(pszAnsiStr == NULL)
    {
        return false;
    }

    WCHAR* pwzWideStr = m_wzData;

    while(*pszAnsiStr == (char) *pwzWideStr)
    {
        if( (*pszAnsiStr) == '\0' || (*pwzWideStr) == '\0')
        {
            break;
        }

        pszAnsiStr++;
        pwzWideStr++;
    }

    return (*pszAnsiStr) == (*pwzWideStr);
}

bool StringTypeConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}

ObjectPtr StringTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    if(!pObjectFrom)
    {
        LibraryCriticalError();
        return NULL;
    }

    if(!CanConvertFrom(pObjectFrom->get_Type()))
    {
        LibraryCriticalError();
        return NULL;
    }

    StringPtr pString(new String(), false);

    *pString = *(String*)pObjectFrom;

    return static_cast<Object*>(pString);
}


}; // namespace AVUI
