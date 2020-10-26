#include <AVUICommon.h>
#include <AVUIPropertyPath.h>
#include <AVUIDependencyProperty.h>
#include <AVUIDependencyObject.h>
#include <AVUICollections.h>
#include <AVUITypeManager.h>
#include <AVUIPropertyPathWorker.h>
#include <AVUIStringUtil.h>

REGISTER_ELEMENT_BEGIN(AVUI::PropertyPath)
    REGISTER_DEFAULT_FACTORY()
    REGISTER_TYPE_CONVERTER(PropertyPathTypeConverter)

REGISTER_ELEMENT_END()

namespace AVUI {

PropertyPath::PropertyPath()
{

}

PropertyPath::PropertyPath(const String& path) : m_path(path)
{
    PrepareSourceValueInfo();
}

PropertyPath::PropertyPath(Object* pParam) : m_path(L"(0)")
{
    m_parameters.AddItem(pParam);
    PrepareSourceValueInfo();
}



int PropertyPath::ComputeUnresolvedAttachedPropertiesInPath()
{
    int num = 0;
    for (int i = get_Length() - 1; i >= 0; i--)
    {
        String* pName = m_arySVI[i].pName;
        if (IsPropertyReference(pName) && (pName->IndexOf(L'.') >= 0))
        {
            num++;
        }
    }
    return num;
}

void PropertyPath::DowncastAccessor(Object* pAccessor, const DependencyProperty** ppDp, const Property** ppProperty)
{
    *ppDp = NULL;
    *ppProperty = NULL;

    if(pAccessor != NULL && pAccessor->IsType(GET_TYPE(DependencyProperty)))
    {
        *ppDp = (DependencyProperty*) pAccessor;
    }
    else
    {
        *ppProperty = object_cast<Property>(pAccessor);
    }
}

Object* PropertyPath::GetAccessor(int k)
{
    return get_SingleWorker()->GetAccessor(k);
}

Object* PropertyPath::GetItem(int k)
{
    return get_SingleWorker()->GetItem(k);
}

TRefCountedPtr<String> PropertyPath::GetPropertyName(Object* pAccessor)
{
    if(pAccessor == NULL)
    {
        return NULL;
    }

    if(pAccessor->IsType(GET_TYPE(Property)))
    {
        return object_allocate<String>(((Property*)pAccessor)->get_PropertyName().ToString());
    }

    return NULL;
}

bool PropertyPath::IsValidAccessor(Object* pAccessor)
{
    return pAccessor != NULL && pAccessor->IsType(GET_TYPE(Property));
}

TRefCountedPtr<Object> PropertyPath::ResolvePropertyName(int level, Object* pItem, const Type* pOwnerType, Object* pContext)
{
    TRefCountedPtr<String> pName = m_arySVI[level].pName;

    if (IsPropertyReference(pName))
    {
        pName = pName->Substring(1, pName->get_Cch() - 2);
    }
    return ResolvePropertyName(pName, pItem, pOwnerType, pContext, false);
}

int PropertyPath::ResolveIndexerParams(int level, Object* pContext)
{
    String* pName = m_arySVI[level].pName;

    return Int::Parse(pName);
}


TRefCountedPtr<Object> PropertyPath::ResolvePropertyName(String* pName, Object* pItem, const Type* pOwnerType, Object* pContext, bool throwOnError)
{
    TRefCountedPtr<String> pStr;
    int length = pName->LastIndexOf(L'.');
    if (length == -1)
    {
        int num2;
        if (IsParameterIndex(pName, false, &num2))
        {
            if (0 <= num2 && num2 < get_PathParameters()->get_Count())
            {
                Object* pAccessor = (*(get_PathParameters()))[num2];
                if (!IsValidAccessor(pAccessor))
                {
                    LibraryCriticalError();
                }
                return pAccessor;
            }
            LibraryCriticalError();
            return NULL;
        }
        pStr = pName;
    }
    else
    {
        pStr = pName->Substring(length + 1);
        pStr->Trim();

        TRefCountedPtr<String> pStr2 = pName->Substring(0, length);
        pStr2->Trim();
        pOwnerType = GetTypeFromName(pStr2, pContext);

        if (pOwnerType == NULL && throwOnError)
        {
            LibraryCriticalError();
        }
    }
    if (pOwnerType == NULL)
    {
        return NULL;
    }
    Object* pProperty = (Object*) pOwnerType->get_Property(*pStr);

    if (pProperty == NULL && throwOnError)
    {
        LibraryCriticalError();
    }
    return pProperty;
}


TRefCountedPtr<Object> PropertyPath::GetTypedParamValue(String* pParam, const Type* pType)
{
    TRefCountedPtr<Object> pObj2;

    if (pType == &(GET_TYPE(String)))
    {
        return pParam;
    }
    const TypeConverter* pConverter = pType->get_TypeConverter();
    if (pConverter != NULL && pConverter->CanConvertFrom(GET_TYPE(String)))
    {
        pObj2 = pConverter->ConvertFrom(pParam);
    }

    return pObj2;
}

const Type* PropertyPath::GetTypeFromName(String* pName, Object* pContext)
{
    return TypeManager::FindType(*pName);
}


TRefCountedPtr<Object> PropertyPath::GetValue()
{
    return get_SingleWorker()->RawValue();
}

bool PropertyPath::IsParameterIndex(String* pName, bool stripParens, int* pIndex)
{
    TRefCountedPtr<String> pParam = pName;
    if(stripParens)
    {
        if(!IsPropertyReference(pName))
        {
            *pIndex = -1;
            return false;
        }
        pParam = pName->Substring(1, pName->get_Cch() - 2);
    }

    for(int idx = 0; idx < pParam->get_Cch(); idx++)
    {
        WCHAR wch = (*pParam)[idx];
        if(wch < '0' || wch > '9')
        {
            *pIndex = -1;
            return false;
        }
    }

    StringUtil::Scan(pParam->get_Ptr(), L"%d", pIndex);

    return true;
}

bool PropertyPath::IsPropertyReference(String* pName)
{
    if (pName != NULL && pName->get_Cch() > 1 && (*pName)[0] == L'(')
    {
        return (*pName)[pName->get_Cch() - 1] == L')';
    }
    return false;
}


void PropertyPath::PrepareSourceValueInfo()
{
    PathParser parser;

    parser.Parse(m_path, &m_arySVI);
}


PropertyPathWorker* PropertyPath::get_SingleWorker()
{
    if (m_pSingleWorker == NULL)
    {
        m_pSingleWorker = object_allocate<PropertyPathWorker>(this);
    }
    return m_pSingleWorker;
}

unsigned int PropertyPath::get_Status()
{
    return get_SingleWorker()->get_Status();
}

TRefCountedPtr<Object> PropertyPath::SetContext(Object* pRootItem)
{
    return get_SingleWorker()->SetContext(pRootItem);
}




bool PropertyPathTypeConverter::CanConvertFrom(const Type& type) const
{
    return (type == GET_TYPE(String));
}

ObjectPtr PropertyPathTypeConverter::ConvertFrom(const Object* pObjectFrom) const
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

    ObjectPtr pPropertyPath(new PropertyPath(*(String*)pObjectFrom), false);

    return pPropertyPath;
}


void PathParser::Parse(const String& str, ItemStructList<SourceValueInfo>* pParsedPath)
{
    m_path = str;
    m_n = m_path.get_Cch();
    if(m_n == 0)
    {
        return;
    }
    m_index = 0;
    m_drillIn = DrillIn::IfNeeded;
    m_state = InitState;

    while(m_state != DoneState)
    {
        WCHAR wch = m_index < m_n ? m_path[m_index] : L'\0';
        if(String::IsWhiteSpace(wch))
        {
            m_index++;
            continue;
        }

        switch(m_state)
        {
        case InitState:
            {
                m_state = PropState;
                break;
            }
        case DrillInState:
            {
                switch (wch)
                {
                case L'.':
                    m_drillIn = DrillIn::Never;
                    m_index++;
                    break;

                case L'/':
                    m_drillIn = DrillIn::Always;
                    m_index++;
                    break;

                case L'[':
                case L'\0':
                    break;
                default:
                    LibraryCriticalError();
                    return;
                }
                m_state = PropState;
                break;
            }
        case PropState:
            {
                bool isIndexer = false;
                switch (wch)
                {
                      case '[':
                      {
                            isIndexer = true;
                      }
                }
                if (isIndexer)
                {
                      AddIndexer(pParsedPath);
                }
                else
                {
                      AddProperty(pParsedPath);
                }
            }
        default:
            break;
        }
    }

}


void PathParser::AddProperty(ItemStructList<SourceValueInfo>* pParsedPath)
{
    int startIndex = m_index;
    int match = 0;

    while(m_index < m_n && (match > 0 || !IsSpecialChar(m_path[m_index])))
    {
        if(m_path[m_index] == L'(')
        {
            match++;
        }
        if(m_path[m_index] == ')')
        {
            match--;
        }
        m_index++;
    }
    if(match > 0)
    {
        // Unbalanced parens
        LibraryCriticalError();
    }
    else
    {
        bool isAllWhitespace = true;
        for(int i = startIndex; i < m_index; i++)
        {
            if(!String::IsWhiteSpace(m_path[i]))
            {
                isAllWhitespace = false;
                break;
            }
        }

        if(isAllWhitespace)
        {
            pParsedPath->AddItem(SourceValueInfo(SourceValueType::Direct, m_drillIn, NULL));
        }
        else
        {
            StringPtr subSegment;

            if(m_path[startIndex] == L'(')
            {
                subSegment = object_allocate<String>(m_path.get_Ptr() + startIndex + 1, m_index - startIndex - 2);
            }
            else
            {
                subSegment = object_allocate<String>(m_path.get_Ptr() + startIndex, m_index - startIndex);
            }

            pParsedPath->AddItem(SourceValueInfo(SourceValueType::Property, m_drillIn, subSegment));
        }

        StartNewLevel();
    }
}

bool PathParser::IsSpecialChar(WCHAR wch)
{
    if(wch == L'.' || wch == L'/' || wch == L'[' || wch == L']')
    {
        return true;
    }
    return false;
}

void PathParser::StartNewLevel()
{
    m_state = m_index < m_n ? DrillInState : DoneState;
    m_drillIn = DrillIn::Never;
}

void PathParser::AddIndexer(ItemStructList<SourceValueInfo>* pParsedPath)
{
    int startIndex = ++m_index;
    bool hitEndBracket = false;

    while(!hitEndBracket)
    {
        if(m_index >= m_n)
        {
            // Parse error
            LibraryCriticalError();
        }
        hitEndBracket = m_path[m_index] == L']';
        m_index++;
    }

    StringPtr pIndexString(new String(m_path.get_Ptr() + startIndex, m_index - startIndex - 1), false);

    pParsedPath->AddItem(SourceValueInfo(SourceValueType::Indexer, m_drillIn, pIndexString));
    StartNewLevel();
}


}; // namespace AVUI
