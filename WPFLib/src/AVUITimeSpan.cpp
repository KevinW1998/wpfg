#include <AVUICommon.h>
#include <AVUITimeSpan.h>
#include <AVUITimeline.h>

REGISTER_VALUE_TYPE_BEGIN(AVUI::TimeSpan)
    REGISTER_TYPE_CONVERTER(TimeSpanTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(AVUI::Nullable<TimeSpan>)
    REGISTER_TYPE_CONVERTER(NullableTimeSpanTypeConverter)
REGISTER_VALUE_TYPE_END()

REGISTER_VALUE_TYPE_BEGIN(AVUI::Duration)
    REGISTER_TYPE_CONVERTER(DurationTypeConverter)
REGISTER_VALUE_TYPE_END()

namespace AVUI {


ObjectPtr DurationTypeConverter::ConvertFromString(const String* pString) const
{
    if(pString->Equals("Automatic"))
    {
        return BoxValue(Duration::get_Automatic());
    }

    if(pString->Equals("Forever"))
    {
        return BoxValue(Duration::get_Forever());
    }

    TimeSpan timeSpan = UnboxValue<TimeSpan>(GET_TYPE(TimeSpan).get_TypeConverter()->ConvertFrom(pString));

    return BoxValue(Duration(timeSpan));
};

ObjectPtr DurationTypeConverter::ConvertToString(const Duration& value) const
{
    return NULL;
};


class TimeSpanStringParser
{
public:

    bool TryParse(const String& str, int* pValueOut)
    {
        *pValueOut = 0;

        int parsedValue = 0;
        m_pStr = &str;
        m_len = str.get_Cch();
        m_pos = -1;

        NextChar();
        SkipBlanks();

        bool isNegative = false;
        if(m_ch == L'-')
        {
            isNegative = true;
            NextChar();
        }

        if(NextNonDigit() == L':')
        {
            if(!ParseTime(&parsedValue))
            {
                return false;
            }
        }
        else
        {
            if(!ParseInt(0xa2e3ff , &parsedValue))
            {
                return false;
            }
            parsedValue = parsedValue * 864000000;
            if(m_ch == L'.')
            {
                int parsedTime;
                NextChar();
                if(!ParseTime(&parsedTime))
                {
                    return false;
                }
                parsedValue += parsedTime;
            }
        }

        if(isNegative)
        {
            parsedValue=-parsedValue;
            if(parsedValue > 0)
            {
                // Overflow
                return false;
            }
        }
        else if(parsedValue < 0)
        {
            // Overflow
            return false;
        }
        SkipBlanks();
        if(m_pos < m_len)
        {
            // Format error.
            return false;
        }
        *pValueOut = parsedValue;

        return true;
    }

private:

    void NextChar()
    {
        if(m_pos < m_len)
        {
            m_pos++;
        }
        m_ch = (m_pos < m_len) ? (*m_pStr)[m_pos] : L'\0';
    }

    void SkipBlanks()
    {
        while(m_ch == L' ' || m_ch == L'\t')
        {
            NextChar();
        }
    }

    WCHAR NextNonDigit()
    {
        for(int i = m_pos; i < m_len; i++)
        {
            WCHAR wch = (*m_pStr)[i];
            if(wch < '0' || wch > '9')
            {
                return wch;
            }
        }
        return L'\0';
    }

    bool ParseInt(int max, int* pOutValue)
    {
        *pOutValue = 0;
        int posOriginal = m_pos;

        while(m_ch >= L'0' && m_ch <= L'9')
        {
            if(*pOutValue < 0)
            {
                // overflow
                return false;
            }
            *pOutValue = *pOutValue * 10 + (m_ch - 0x30);
            if(*pOutValue < 0)
            {
                // Overflow
                return false;
            }
            NextChar();
        }

        if(posOriginal == m_pos)
        {
            // Format error
            return false;
        }
        if(*pOutValue > max)
        {
            // Overflow
            return false;
        }

        return true;
    }

    bool ParseTime(int* pOutTime)
    {
        int parsedValue = 0;
        *pOutTime = 0;

        if(!ParseInt(23, &parsedValue))
        {
            // Overflow
            return false;
        }

        *pOutTime = parsedValue * 3600000;
        if(m_ch != L':')
        {
            // Format error
            return false;
        }
        NextChar();
        if(!ParseInt(59, &parsedValue))
        {
            // Overflow
            return false;
        }
        *pOutTime += parsedValue * 60000;
        if(m_ch == L':')
        {
            NextChar();

            if(m_ch != L'.')
            {
                if(!ParseInt(59, &parsedValue))
                {
                    // Overflow
                    return false;
                }
                *pOutTime += parsedValue * 1000;
            }
        }

        if(m_ch == L'.')
        {
            NextChar();
            parsedValue = 1000;

            while(m_ch >= L'0' && m_ch <= L'9')
            {
                parsedValue /= 10;
                *pOutTime += (m_ch - L'0') * parsedValue;
                NextChar();
            }
        }

        return true;
    }

    WCHAR m_ch;
    int m_len;
    int m_pos;
    const String* m_pStr;

};


ObjectPtr TimeSpanTypeConverter::ConvertFromString(const String* pString) const
{
    int m_ticksValue;
    TimeSpanStringParser stringParser;

    if(!stringParser.TryParse(*pString, &m_ticksValue))
    {
        LibraryCriticalError();
    }

    return BoxValue(TimeSpan(m_ticksValue));
};

ObjectPtr TimeSpanTypeConverter::ConvertToString(const TimeSpan& value) const
{
    return NULL;
};


ObjectPtr RepeatBehaviorTypeConverter::ConvertFromString(const String* pString) const
{
    if(pString->Equals("Forever"))
    {
        return BoxValue(RepeatBehavior::get_Forever());
    }

    if(pString->get_Cch() > 1 && pString->EndsWith(L"x", 1))
    {
        String strRepeatCount(pString->get_Ptr(), pString->get_Cch() - 1);

        float repeatCount = UnboxValue<float>(GET_TYPE(float).get_TypeConverter()->ConvertFrom(&strRepeatCount));
        return BoxValue(RepeatBehavior(repeatCount));
    }
    else
    {
        TimeSpan timeSpan = UnboxValue<TimeSpan>(GET_TYPE(TimeSpan).get_TypeConverter()->ConvertFrom(pString));

        return BoxValue(RepeatBehavior(timeSpan));
    }
};

ObjectPtr RepeatBehaviorTypeConverter::ConvertToString(const RepeatBehavior& value) const
{
    return NULL;
};


}; // namespace AVUI
