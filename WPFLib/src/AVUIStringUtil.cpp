#include <AVUICommon.h>
#include <AVUIUIString.h>
#include <AVUIStringUtil.h>
#include <wchar.h>
#include <string.h>
#include <stdarg.h>

namespace AVUI {

int StringUtil::Scan(const WCHAR* wzSrc, const WCHAR* wzFormat, void* pData1, void* pData2, void* pData3, void* pData4)
{
#if defined(_WIN32) && !defined(_WIN32_WCE)
    return swscanf_s(wzSrc, wzFormat, pData1, pData2, pData3, pData4);
#else
    return swscanf(wzSrc, wzFormat, pData1, pData2, pData3, pData4);
#endif

}

int StringUtil::ScanSubsection(const WCHAR* wzSrc, int srcLen, const WCHAR* wzFormat, void* pData1, void* pData2, void* pData3, void* pData4)
{
#if defined(_WIN32) && !defined(_WIN32_WCE)
    return _snwscanf_s(wzSrc, srcLen, wzFormat, pData1, pData2, pData3, pData4);
#else
    String str(wzSrc, srcLen);

    return swscanf(str.get_Ptr(), wzFormat, pData1, pData2, pData3, pData4);
#endif
}

int StringUtil::Print(WCHAR* wzDest, int cchMax, WCHAR* wzFmt, ...)
{
    va_list vl;
    va_start(vl, wzFmt);

#if defined(_WIN32) && !defined(_WIN32_WCE)
    return vswprintf_s(wzDest, cchMax, wzFmt, vl);
#else
    return vswprintf(wzDest, wzFmt, vl);
#endif
}

int StringUtil::Compare(const WCHAR* wzOne, const WCHAR* wzTwo)
{
    return wcscmp(wzOne, wzTwo);
}

int StringUtil::CompareLen(const WCHAR* wzOne, const WCHAR* wzTwo, int cchCompare)
{
    return wcsncmp(wzOne, wzTwo, cchCompare);
}

int StringUtil::CompareCaseInsensitive(const WCHAR* wzOne, const WCHAR* wzTwo)
{
#if defined(_WIN32)
    return _wcsicmp(wzOne, wzTwo);
#else
    return wcscasecmp(wzOne, wzTwo);
#endif
}

int StringUtil::CompareCaseInsensitiveLen(const WCHAR* wzOne, const WCHAR* wzTwo, int cchCompare)
{
#if defined(_WIN32)
    return _wcsnicmp(wzOne, wzTwo, cchCompare);
#else
    return wcsncasecmp(wzOne, wzTwo);
#endif
}

int StringUtil::GetCch(const WCHAR* wz)
{
    return (int) wcslen(wz);
}

int StringUtil::GetAnsiCch(const char* sz)
{
    return (int) strlen(sz);
}

void StringUtil::CopySubstring(WCHAR* wzDest, int cchDest, const WCHAR* wzSrc, int cchToCopy)
{
    if(cchToCopy > cchDest)
    {
        LibraryCriticalError();
    }

#if defined(_WIN32) && !defined(_WIN32_WCE)
    wcsncpy_s(wzDest, cchDest, wzSrc, cchToCopy);
#else
    wcsncpy(wzDest, wzSrc, cchToCopy);
#endif
}

int StringUtil::ToLower(int upperCase)
{
    if(upperCase >= L'A' && upperCase <= 'Z')
    {
        return upperCase - L'A' + L'a';
    }

    return upperCase;
}


}; // namespace AVUI

