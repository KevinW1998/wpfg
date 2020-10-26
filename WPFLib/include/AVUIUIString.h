#pragma once

namespace AVUI {

class String : public Object
{
public:
    DECLARE_ELEMENT(String, Object);

    String() : m_wzData(NULL), m_cch(0) { };
    String(const char* szString, int cch = -1);
    String(const WCHAR* wzString, int cch = -1);
    String(const String& strOther);

    bool operator==(const String& rhs) const;
    bool operator!=(const String& rhs) const;
    String& operator=(const String& rhs);

    ~String();

    bool Equals(const Object* pOther) const;
    bool Equals(const WCHAR* pwchOther, bool isInsensitive = false, int cchOther = -1) const;
    bool Equals(const char* pchOther, bool isInsensitive = false, int cchOther = -1) const;
    bool BeginsWith(const WCHAR* pwch, int cch) const;
    bool EndsWith(const WCHAR* pwch, int cch) const;

    void ConvertToAnsi(char* pszBuffer, unsigned int cchMaxBuffer);
    bool IsEqualToAnsi(char* pszAnsiStr);

    void Trim();
    void InsertAt(int idx, WCHAR wch);
    void DeleteAt(int idx);

    int IndexOf(WCHAR wch, int startIndex = 0) const;
    int LastIndexOf(WCHAR wch) const;

    const WCHAR* get_Ptr() const { return m_wzData; }
    int get_Cch() const { return m_cch; }
    WCHAR operator [] (int i) const { if(i >= m_cch) LibraryCriticalError(); return m_wzData[i]; }
    static bool IsWhiteSpace(WCHAR wch) { if(wch != L' ' && (wch < L'\t' || wch > L'\r') && wch != 0x00A0 && wch != 0x0085) return false; return true; }
    static bool IsNullOrEmpty(String* pString) { return pString == NULL || pString->m_cch == 0; }

    TRefCountedPtr<String> Substring(int index, int cch) const;
    TRefCountedPtr<String> Substring(int index) const;
    TRefCountedPtr<String> ToLower() const;
    bool Contains(WCHAR wch) const { return IndexOf(wch) != -1; }

private:

    WCHAR* m_wzData;
    int m_cch;
};


class StringTypeConverter : public TypeConverter
{
public:
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const { return false; };
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { LibraryCriticalError(); return NULL; }
};


typedef TRefCountedPtr<String> StringPtr;

};


