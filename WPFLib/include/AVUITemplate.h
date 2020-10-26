#pragma once

#include <AVUIDependencyObject.h>
#include <AVUIItemStructList.h>

namespace AVUI {

class String;
class ResourceDictionary;

class FrameworkTemplate : public Object
{
    friend class StyleHelper;
    friend class XamlParser;
    friend class UIElement;
public:
    FrameworkTemplate() : m_stackDepth(0), m_isSealed(false) { };

    DECLARE_ELEMENT(FrameworkTemplate, Object);
    IMPLEMENT_REFERENCE_PROPERTY(Resources, ResourceDictionary, false);

    ObjectPtr CreateInstance(DependencyObject* pTemplatedParent);

    Object* FindName(const String& childName, DependencyObject* pContext);

    void set_Resources(ResourceDictionary* pResources) { m_pResources = pResources; }
    ResourceDictionary* get_Resources();

protected:

    virtual void Seal();

    const Type* GetTypeFromChildName(const String& childName);
    int GetChildIndexByName(const String* pChildName);
    static DependencyObject* GetTemplateChildByIndex(DependencyObject* pDO, int childIndex);

    bool get_IsSealed() { return m_isSealed; }

    void CheckSealed() { if(get_IsSealed()) LibraryCriticalErrorMessage("Sealed templates cannot be modified."); }

private:
    static void ClearTemplateChildList(DependencyObject* pTemplatedParent);
    void UpdateChildren(DependencyObject* pTemplatedParent);

    void RecordStartElement(const WCHAR* pwchElement, int cchElement);
    bool RecordEndElement();
    void RecordCharacters(const WCHAR* pwchCharacters, int cchElement);

    void RecordBeginAttributes();
    void RecordAttribute(const WCHAR* pwchName, int cchName, const WCHAR* pwchValue, int cchValue);
    void RecordEndAttributes();

    void RecordTemplateChild(const String& childName, const Type* pChildType);
    void RecordTemplateChildInstance(DependencyObject* pDOTemplateCreator, DependencyObject* pDO);

    ItemStructList<unsigned char> m_templateStream;

    bool m_isSealed;

    static UncommonField<ItemStructList<TRefCountedPtr<DependencyObject> > > NamedTemplateChildList;

    struct ChildNameLookup
    {
        ChildNameLookup() : pType(NULL) { }
        ChildNameLookup(String* pString, const Type* pTypeIn) : pType(pTypeIn), ChildName(pString) { }

        TRefCountedPtr<String> ChildName;
        const Type* pType;
    };

    ItemStructList<ChildNameLookup> ChildNameToIndex;
    TRefCountedPtr<ItemStructList<TRefCountedPtr<DependencyObject> > > m_pChildInstances;

    enum StreamTag
    {
        StreamTagStartElement,
        StreamTagEndElement,
        StreamTagCharacters,
        StreamTagBeginAttributes,
        StreamTagAttribute,
        StreamTagEndAttributes,
    };

    void WriteStreamTag(StreamTag streamTag);
    void WriteStreamString(const WCHAR* pwch, int cch);

    bool ReadNextStreamTag(StreamTag* pStreamTag);
    bool ReadNextStreamString(const WCHAR** ppwchData, int* pcchData);


    void WriteDataToStream(const void* pvData, int cbData);
    void ReadDataFromStream(void* pvData, int cbData);

    void BeginReadStream();

    int m_stackDepth;
    int m_currentStreamPos;
    TRefCountedPtr<ResourceDictionary> m_pResources;
};



}; // namespace AVUI
