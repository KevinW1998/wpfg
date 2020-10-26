#include <AVUICommon.h>
#include <AVUIGeometry.h>
#include <AVUIStreamGeometry.h>
#include <AVUIStreamGeometryContext.h>
#include <AVUIStringUtil.h>

REGISTER_ELEMENT_BEGIN(AVUI::Geometry)
    REGISTER_TYPE_CONVERTER(GeometryTypeConverter)

    REGISTER_PROPERTY(Transform);
REGISTER_ELEMENT_END()

namespace AVUI
{

bool GeometryTypeConverter::CanConvertFrom(const Type& type) const
{
    return type == GET_TYPE(String);
}
ObjectPtr GeometryTypeConverter::ConvertFrom(const Object* pObjectFrom) const
{
    if(pObjectFrom == NULL)
    {
        LibraryCriticalError();
        return NULL;
    }

    if(!CanConvertFrom(pObjectFrom->get_Type()))
    {
        LibraryCriticalError();
        return NULL;
    }

    return (Object*) ParseGeometry(*(const String*) pObjectFrom);
}


TRefCountedPtr<Geometry> GeometryTypeConverter::ParseGeometry(const String& pathString)
{
    FillRule::Enum fillRule = FillRule::EvenOdd;
    TRefCountedPtr<StreamGeometry> pGeometry = object_allocate<StreamGeometry>();

    ParseStringToStreamGeometryContext(pGeometry->Open(), pathString, &fillRule);

    pGeometry->set_FillRule(fillRule);

    return (Geometry*) pGeometry;
}

class AbbreviatedGeometryParser
{
public:
    AbbreviatedGeometryParser() : m_pContext(NULL), m_curIndex(0), m_figureStarted(false), m_pathLength(0), m_allowComma(false),
                                  m_allowSign(false), m_isClosed(false), m_isFilled(false), m_isSmoothJoin(false), m_isStroked(false)
    {
    }

    void ParseToGeometryContext(StreamGeometryContext* pContext, const String& pathString, int startIndex)
    {
        m_pContext = pContext;
        m_pPathString = &pathString;
        m_pathLength = pathString.get_Cch();
        m_curIndex = startIndex;


        bool flag = true;
        WCHAR ch3 = L' ';
        while (ReadToken())
        {
            WCHAR cmd = m_token;
            if (flag)
            {
                if ((cmd != L'M') && (cmd != L'm'))
                {                    
                    ThrowBadToken();
                }
                flag = false;
            }
            switch (cmd)
            {
            case 'L':
            case 'V':
            case 'H':
            case 'h':
            case 'l':
            case 'v':
                break;

            case 'M':
            case 'm':
                {
                    m_lastPoint = ReadPoint(cmd, false);
                    pContext->BeginFigure(m_lastPoint, true, false);
                    m_figureStarted = true;
                    m_lastStart = m_lastPoint;
                    ch3 = 'M';
                    while (IsNumber(true))
                    {
                        m_lastPoint = ReadPoint(cmd, false);
                        pContext->LineTo(m_lastPoint, true, false);
                        ch3 = 'L';
                    }
                    continue;
                }
            case 'Q':
            case 'T':
            case 'q':
            case 't':
                {
                    EnsureFigure();
                    do
                    {
                        switch (cmd)
                        {
                        case 't':
                        case 'T':
                            if (ch3 == 'Q')
                            {
                                m_secondLastPoint = Reflect();
                            }
                            else
                            {
                                m_secondLastPoint = m_lastPoint;
                            }
                            m_lastPoint = ReadPoint(cmd, false);
                            break;

                        default:
                            m_secondLastPoint = ReadPoint(cmd, false);
                            m_lastPoint = ReadPoint(cmd, true);
                            break;
                        }
                        pContext->QuadraticBezierTo(m_secondLastPoint, m_lastPoint, true, false);
                        ch3 = 'Q';
                    }

                    while (IsNumber(true))
                        ;

                    continue;
                }

            case 'S':
            case 'C':
            case 'c':
            case 's':
                {
                    EnsureFigure();
                    do
                    {
                        Point point;
                        switch (cmd)
                        {
                        case 's':
                        case 'S':
                            if (ch3 == L'C')
                            {
                                point = Reflect();
                            }
                            else
                            {
                                point = m_lastPoint;
                            }
                            m_secondLastPoint = ReadPoint(cmd, false);
                            break;
    
                        default:
                            point = ReadPoint(cmd, false);
                            m_secondLastPoint = ReadPoint(cmd, true);
                            break;
                        }
                        m_lastPoint = ReadPoint(cmd, true);
                        pContext->BezierTo(point, m_secondLastPoint, m_lastPoint, true, false);
                        ch3 = 'C';
                    }
    
                    while (IsNumber(true))
                        ;
    
                    continue;
                }

            case 'Z':
            case 'z':
                {
                    EnsureFigure();
                    // pContext->SetClosedState(true);
                    m_figureStarted = false;
                    ch3 = L'Z';
                    m_lastPoint = m_lastStart;

                    continue;
                }
            case 'A':
            case 'a':
                {
                    EnsureFigure();
                    do
                    {
                        float width = ReadNumber(false);
                        float height = ReadNumber(true);
                        float rotationAngle = ReadNumber(true);
                        bool isLargeArc = ReadBool();
                        bool flag2 = ReadBool();
                        m_lastPoint = ReadPoint(cmd, true);
                        pContext->ArcTo(m_lastPoint, Size(width, height), rotationAngle, isLargeArc, flag2 ? SweepDirection::Clockwise : SweepDirection::Counterclockwise, true, false);
                    }
                    while (IsNumber(true))
                        ;

                    ch3 = 'A';
                    continue;
                }
            default:
                ThrowBadToken();
            }

            EnsureFigure();

            do
            {
                switch (cmd)
                {
                case 'h':
                    m_lastPoint.set_X(m_lastPoint.get_X() + ReadNumber(false));
                    break;

                case 'l':
                    m_lastPoint = ReadPoint(cmd, false);
                    break;

                case 'v':
                    m_lastPoint.set_Y(m_lastPoint.get_Y() + ReadNumber(false));
                    break;

                case 'H':
                    m_lastPoint.set_X(ReadNumber(false));
                    break;

                case 'L':
                    m_lastPoint = ReadPoint(cmd, false);
                    break;

                case 'V':
                    m_lastPoint.set_Y(ReadNumber(false));
                    break;
                }
                pContext->LineTo(m_lastPoint, true, false);
            }
            while (IsNumber(true))
                ;

            ch3 = 'L';
        }



    }

private:

    bool More()
    {
        return (m_curIndex < m_pathLength);
    }

    bool IsNumber(bool allowComma)
    {
        bool flag = SkipWhiteSpace(allowComma);
        if (More())
        {
            m_token = (*m_pPathString)[m_curIndex];
            if (m_token == L'.' || 
                m_token == L'-' || 
                m_token == L'+' || 
                (m_token >= '0' && m_token <= '9') || 
                m_token == 'I' || 
                m_token == 'N')
            {
                return true;
            }
        }
        if (flag)
        {
            ThrowBadToken();
        }
        return false;
    }

    bool ReadToken()
    {
        SkipWhiteSpace(false);
        if (More())
        {
            m_token = (*m_pPathString)[m_curIndex++];
            return true;
        }
        return false;
    }

    void ThrowBadToken()
    {
        LibraryCriticalError();
    }

    Point ReadPoint(WCHAR cmd, bool allowcomma)
    {
        float x = ReadNumber(allowcomma);
        float y = ReadNumber(true);
        if (cmd >= L'a')
        {
            x += m_lastPoint.get_X();
            y += m_lastPoint.get_Y();
        }
        return Point(x, y);
    }

    void EnsureFigure()
    {
        if (!m_figureStarted)
        {
            m_pContext->BeginFigure(m_lastStart, true, false);
            m_figureStarted = true;
        }
    }

    Point Reflect()
    {
        return Point((2.0f * m_lastPoint.get_X()) - m_secondLastPoint.get_X(), (2.0f * m_lastPoint.get_Y()) - m_secondLastPoint.get_Y());
    }

 
    float ReadNumber(bool allowComma)
    {
        float num4;
        if (!IsNumber(allowComma))
        {
            ThrowBadToken();
        }
        bool flag = true;
        int startIndex = m_curIndex;
        if (More() && ((*m_pPathString)[m_curIndex] == L'-' || (*m_pPathString)[m_curIndex] == L'+'))
        {
            m_curIndex++;
        }
        if (More() && (*m_pPathString)[m_curIndex] == L'I')
        {
            m_curIndex = Int::Min(m_curIndex + 8, m_pathLength);
            flag = false;
        }
        else if (More() && (*m_pPathString)[m_curIndex] == L'N')
        {
            m_curIndex = Int::Min(m_curIndex + 3, m_pathLength);
            flag = false;
        }
        else
        {
            SkipDigits(false);
            if (More() && (*m_pPathString)[m_curIndex] == L'.')
            {
                flag = false;
                m_curIndex++;
                SkipDigits(false);
            }
            if (More() && ((*m_pPathString)[m_curIndex] == L'E' || (*m_pPathString)[m_curIndex] == 'e'))
            {
                flag = false;
                m_curIndex++;
                SkipDigits(true);
            }
        }
        if (flag && (m_curIndex <= (startIndex + 8)))
        {
            int num3 = 1;
            if ((*m_pPathString)[startIndex] == L'+')
            {
                startIndex++;
            }
            else if ((*m_pPathString)[startIndex] == L'-')
            {
                startIndex++;
                num3 = -1;
            }
            int num2 = 0;
            while (startIndex < m_curIndex)
            {
                num2 = (num2 * 10) + ((*m_pPathString)[startIndex] - '0');
                startIndex++;
            }
            return(float) (num2 * num3);
        }

        String subStr(m_pPathString->get_Ptr() + startIndex, m_curIndex - startIndex);

        if(!StringUtil::Scan(subStr.get_Ptr(), L"%f", &num4))
        {
            LibraryCriticalError();
        }
        return num4;
    }


    void SkipDigits(bool signAllowed)
    {
        if ((signAllowed && More()) && ((*m_pPathString)[m_curIndex] == L'-' || (*m_pPathString)[m_curIndex] == L'+'))
        {
            m_curIndex++;
        }
        while (More() && ((*m_pPathString)[m_curIndex] >= '0' && (*m_pPathString)[m_curIndex] <= '9'))
        {
            m_curIndex++;
        }
    }


    bool ReadBool()
    {
        SkipWhiteSpace(true);
        if (More())
        {
            m_token = (*m_pPathString)[m_curIndex++];
            if (m_token == L'0')
            {
                return false;
            }
            if (m_token == L'1')
            {
                return true;
            }
        }
        ThrowBadToken();
        return false;
    }

 

    bool SkipWhiteSpace(bool allowComma)
    {
        bool flag = false;
        while (More())
        {
            WCHAR c = (*m_pPathString)[m_curIndex];
            switch (c)
            {
                case L'\t':
                case L'\n':
                case L'\r':
                case L' ':
                    break;
    
                case ',':
                    if (allowComma)
                    {
                        flag = true;
                        allowComma = false;
                    }
                    else
                    {
                        ThrowBadToken();
                    }
                    break;
    
                default:
                    if ((c > L' ') && (c <= L'z'))
                    {
                        return flag;
                    }
                    if (!String::IsWhiteSpace(c))
                    {
                        return flag;
                    }
                    break;
            }
            m_curIndex++;
        }
        return flag;
    }


    StreamGeometryContext* m_pContext;
    int m_curIndex;
    bool m_figureStarted;
    Point m_lastPoint;
    Point m_lastStart;
    int m_pathLength;
    const String* m_pPathString;
    Point m_secondLastPoint;
    WCHAR m_token;
    bool m_allowComma;
    bool m_allowSign;
    bool m_isClosed;
    bool m_isFilled;
    bool m_isSmoothJoin;
    bool m_isStroked;
 
};


 
void GeometryTypeConverter::ParseStringToStreamGeometryContext(StreamGeometryContext* pContext, const String& pathString, FillRule::Enum* pFillRule)
{
    int index = 0;
    while ((index < pathString.get_Cch()) && String::IsWhiteSpace(pathString[index]))
    {
        index++;
    }
    if ((index < pathString.get_Cch()) && (pathString[index] == L'F'))
    {
        index++;
        while ((index < pathString.get_Cch()) && String::IsWhiteSpace(pathString[index]))
        {
            index++;
        }
        if ((index == pathString.get_Cch()) || ((pathString[index] != L'0') && (pathString[index] != L'1')))
        {
            LibraryCriticalError();
        }
        *pFillRule = (pathString[index] == L'0') ? FillRule::EvenOdd : FillRule::NonZero;
        index++;
    }

    AbbreviatedGeometryParser().ParseToGeometryContext(pContext, pathString, index);
}
 


} // namespace AVUI

