#include <AVUICommon.h>
#include <AVUIControl.h>
#include <AVUITemplate.h>
#include <AVUIControlTemplate.h>
#include <AVUIUIElement.h>
#include <AVUIXamlParser.h>
#include <AVUISetter.h>
#include <AVUITrigger.h>
#include <AVUIStyleHelper.h>
#include <AVUIResourceDictionary.h>


REGISTER_ELEMENT_BEGIN(AVUI::FrameworkTemplate)
    REGISTER_PROPERTY(Resources)
    REGISTER_DEFAULT_FACTORY()
REGISTER_ELEMENT_END()

namespace AVUI {


UncommonField<ItemStructList<DependencyObjectPtr> > FrameworkTemplate::NamedTemplateChildList;


void FrameworkTemplate::RecordStartElement(const WCHAR* pwchElement, int cchElement)
{
    WriteStreamTag(StreamTagStartElement);
    WriteStreamString(pwchElement, cchElement);

    m_stackDepth++;
}

bool FrameworkTemplate::RecordEndElement()
{
    WriteStreamTag(StreamTagEndElement);

    m_stackDepth--;

    return m_stackDepth != 0;
}

void FrameworkTemplate::RecordCharacters(const WCHAR* pwchCharacters, int cchElement)
{
    WriteStreamTag(StreamTagCharacters);
    WriteStreamString(pwchCharacters, cchElement);
}

void FrameworkTemplate::RecordBeginAttributes()
{
    WriteStreamTag(StreamTagBeginAttributes);
}

void FrameworkTemplate::RecordEndAttributes()
{
    WriteStreamTag(StreamTagEndAttributes);
}


void FrameworkTemplate::RecordAttribute(const WCHAR* pwchName, int cchName, const WCHAR* pwchValue, int cchValue)
{
    WriteStreamTag(StreamTagAttribute);
    WriteStreamString(pwchName, cchName);
    WriteStreamString(pwchValue, cchValue);
}

void FrameworkTemplate::WriteStreamTag(StreamTag streamTag)
{
    char streamChar = (char) streamTag;
    WriteDataToStream(&streamChar, sizeof(streamChar));
}

void FrameworkTemplate::WriteStreamString(const WCHAR* pwch, int cch)
{
    unsigned short cchShort = (unsigned short) cch;
    WriteDataToStream(&cchShort, sizeof(cchShort));
    WriteDataToStream(pwch, cch * sizeof(WCHAR));
}

void FrameworkTemplate::WriteDataToStream(const void* pvData, int cbData)
{
    const unsigned char* pbData = reinterpret_cast<const unsigned char*>(pvData);
    for(int i = 0; i < cbData; i++)
    {
        m_templateStream.AddItem(pbData[i]);
    }
}

void FrameworkTemplate::BeginReadStream()
{
    m_currentStreamPos = 0;
}

bool FrameworkTemplate::ReadNextStreamTag(StreamTag* pStreamTag)
{
    if(m_currentStreamPos == m_templateStream.get_Count())
    {
        return false;
    }

    unsigned char chTag;

    ReadDataFromStream(&chTag, sizeof(chTag));    

    *pStreamTag = (StreamTag) chTag;

    return true;
}

void FrameworkTemplate::ReadDataFromStream(void* pvData, int cbData)
{
    unsigned char* pbData = reinterpret_cast<unsigned char*>(pvData);

    for(int i = 0; i < cbData; i++)
    {
        pbData[i] = m_templateStream[m_currentStreamPos++];
    }
}


bool FrameworkTemplate::ReadNextStreamString(const WCHAR** ppwchData, int* pcchData)
{
    unsigned short cchShort;

    ReadDataFromStream(&cchShort, sizeof(cchShort));
    *pcchData = cchShort;

    *ppwchData = (const WCHAR*) &(m_templateStream.get_Ptr()[m_currentStreamPos]);
    m_currentStreamPos += cchShort * sizeof(WCHAR);

    return true;
}


ObjectPtr FrameworkTemplate::CreateInstance(DependencyObject* pTemplatedParent)
{
    Seal();

    XamlParser xamlParser;

    StreamTag tagCurrent;

    m_pChildInstances = NULL;
    BeginReadStream();

    xamlParser.StartDocument();

    xamlParser.set_TemplatedParent(pTemplatedParent);
    xamlParser.set_TemplateInstantiating(this);

    while(ReadNextStreamTag(&tagCurrent))
    {
        switch(tagCurrent)
        {
        case StreamTagStartElement:
            {
                const WCHAR* pwchElementName;
                int cchElementName;

                ReadNextStreamString(&pwchElementName, &cchElementName);

                xamlParser.StartElement(pwchElementName, cchElementName);

                break;
            }
        case StreamTagEndElement:
            {
                xamlParser.EndElement();
                break;
            }

        case StreamTagCharacters:
            {
                const WCHAR* pwchData;
                int cchData;

                ReadNextStreamString(&pwchData, &cchData);

                xamlParser.Characters(pwchData, cchData);
                break;
            }

        case StreamTagAttribute:
            {
                const WCHAR* pwchName;
                int cchName;
                const WCHAR* pwchValue;
                int cchValue;

                ReadNextStreamString(&pwchName, &cchName);
                ReadNextStreamString(&pwchValue, &cchValue);

                xamlParser.AddAttribute(pwchName, cchName, pwchValue, cchValue);

                break;
            }
        case StreamTagBeginAttributes:
            {
                xamlParser.BeginAttributes();
                break;
            }
        case StreamTagEndAttributes:
            {
                xamlParser.EndAttributes();
                break;   
            }

        default:
            {
                LibraryCriticalError();
                return NULL;
            }
        }
    }

    xamlParser.EndDocument();

    UpdateChildren(pTemplatedParent);

    return xamlParser.m_pRootElement;
}

void FrameworkTemplate::RecordTemplateChild(const String& childName, const Type* pChildType)
{
    StringPtr pString(new String(childName), false);

    ChildNameToIndex.AddItem(ChildNameLookup(pString, pChildType));
}

void FrameworkTemplate::RecordTemplateChildInstance(DependencyObject* pDOTemplateCreator, DependencyObject* pDO)
{
    if(m_pChildInstances == NULL)
    {
        TRefCountedPtr<ItemStructList<DependencyObjectPtr > > pChildList(new ItemStructList<DependencyObjectPtr>, false);

        m_pChildInstances = pChildList;
    }

    pDO->set_TemplateChildIndex(m_pChildInstances->get_Count() + 1);

    m_pChildInstances->AddItem(pDO);

    if(IsType(GET_TYPE(ControlTemplate)))
    {
        StyleHelper::InvalidatePropertiesOnTemplateNode(pDOTemplateCreator, pDO, pDO->get_TemplateChildIndex(), ((ControlTemplate*)this)->ChildRecordFromChildIndex, false);
    }

}

void FrameworkTemplate::UpdateChildren(DependencyObject* pTemplatedParent)
{
    NamedTemplateChildList.SetValue(pTemplatedParent, m_pChildInstances);
    m_pChildInstances = NULL;
}

DependencyObject* FrameworkTemplate::GetTemplateChildByIndex(DependencyObject* pDO, int childIndex)
{
    if(childIndex == 0)
    {
        return pDO;
    }

    if(pDO == NULL)
    {
        LibraryCriticalError();
    }

    ItemStructList<TRefCountedPtr<DependencyObject> >* pChildren = NamedTemplateChildList.GetValue(pDO);
    childIndex -= 1;
    if(pChildren == NULL)
    {
        return NULL;
    }
    if(pChildren->get_Count() <= childIndex)
    {
        return NULL;
    }

    return (*pChildren)[childIndex];
}

void FrameworkTemplate::Seal()
{
    if(m_isSealed)
    {
        return;
    }

    m_isSealed = true;
}

const Type* FrameworkTemplate::GetTypeFromChildName(const String& childName)
{
    for(int i = 0; i < ChildNameToIndex.get_Count(); i++)
    {
        if(*(ChildNameToIndex[i].ChildName) == childName)
        {
            return ChildNameToIndex[i].pType;
        }
    }

    return NULL;
}

int FrameworkTemplate::GetChildIndexByName(const String* pChildName)
{
    if(pChildName == NULL)
    {
        return 0;
    }

    for(int i = 0; i < ChildNameToIndex.get_Count(); i++)
    {
        if(*(ChildNameToIndex[i].ChildName) == *pChildName)
        {
            return (int)(i + 1);
        }
    }

    return -1;
}

Object* FrameworkTemplate::FindName(const String& childName, DependencyObject* pContext)
{
    int index = GetChildIndexByName(&childName);
    if(index == -1)
    {
        return NULL;
    }

    return FrameworkTemplate::GetTemplateChildByIndex(pContext, index);
}

void FrameworkTemplate::ClearTemplateChildList(DependencyObject* pDO)
{
    NamedTemplateChildList.ClearValue(pDO);
}

ResourceDictionary* FrameworkTemplate::get_Resources()
{
    if(m_pResources == NULL)
    {
        m_pResources = object_allocate<ResourceDictionary>();
    }
    return m_pResources;
}


}; // namespace AVUI
