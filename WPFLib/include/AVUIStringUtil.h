#pragma once

namespace AVUI {

class StringUtil
{
public:

    static int Scan(const WCHAR* wzSrc, const WCHAR* wzFormat, void* pData1 = NULL, void* pData2 = NULL, void* pData3 = NULL, void* pData4 = NULL);
    static int ScanSubsection(const WCHAR* wzSrc, int srcLen, const WCHAR* wzFormat, void* pData1 = NULL, void* pData2 = NULL, void* pData3 = NULL, void* pData4 = NULL);
    static int Print(WCHAR* wzDest, int cchMax, WCHAR* wzFmt, ...);
    static int Compare(const WCHAR* wzOne, const WCHAR* wzTwo);
    static int CompareLen(const WCHAR* wzOne, const WCHAR* wzTwo, int cchCompare);
    static int CompareCaseInsensitive(const WCHAR* wzOne, const WCHAR* wzTwo);
    static int CompareCaseInsensitiveLen(const WCHAR* wzOne, const WCHAR* wzTwo, int cchCompare);
    static int GetCch(const WCHAR* wz);
    static int GetAnsiCch(const char* sz);
    static void CopySubstring(WCHAR* wzDest, int cchDest, const WCHAR* wzSrc, int cchToCopy);
    static int ToLower(int upperCase);
};


};



