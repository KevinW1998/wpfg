#pragma once

#include <AVUIItemStructList.h>
#include <AVUITypeConverter.h>
#include <AVUIObjectCollection.h>

namespace AVUI {

class PropertyPathWorker;
struct SourceValueInfo
{
public:
    SourceValueInfo(SourceValueType::Enum type, DrillIn::Enum drillIn, const TRefCountedPtr<String>& name) : 
                    Type(type), DrillIn(drillIn), pName(name) { }
    SourceValueInfo() { }

    SourceValueType::Enum Type;
    DrillIn::Enum DrillIn;
    TRefCountedPtr<String> pName;

};

struct ResolvedSVI
{
    SourceValueType::Enum Type;
    int index;
    const Object* pAccessor;
    Object* pItem;
};

class PropertyPath : public Object
{
public:
    DECLARE_ELEMENT(PropertyPath, Object);

    PropertyPath();
    PropertyPath(const String& propertyPath);
    PropertyPath(Object* pParam);

    int ComputeUnresolvedAttachedPropertiesInPath();
    void DowncastAccessor(Object* pAccessor, const DependencyProperty** ppDp, const Property** ppProperty);
    Object* GetAccessor(int k);
    Object* GetItem(int k);

    static TRefCountedPtr<String> GetPropertyName(Object* pAccessor);
    TRefCountedPtr<Object> GetValue();
    static bool IsParameterIndex(String* pName, bool stripParens, int* pIndex);
    static bool IsPropertyReference(String* pName);
    static bool IsValidAccessor(Object* pAccessor);

 
    TRefCountedPtr<Object> ResolvePropertyName(int level, Object* pItem, const Type* pOwnerType, Object* pContext);
    int ResolveIndexerParams(int level, Object* pContext);

 
 
    // Retain this
    SourceValueInfo* GetSourceValueInfo(int level) { return &(m_arySVI[level]); }


    Object* get_LastAccessor() { return GetAccessor(get_Length() - 1); }
    Object* get_LastItem() { return GetItem(get_Length() - 1); }
    int get_Length() { return m_arySVI.get_Count(); }
    String* get_Path() { return &m_path; }
    PropertyPathWorker* get_SingleWorker();
    unsigned int get_Status(); // PropertyPathStatus
    TRefCountedPtr<Object> SetContext(Object* pRootItem);

    ItemStructList<Object*> * get_PathParameters() { return &m_parameters; }

private:
    TRefCountedPtr<Object> GetTypedParamValue(String* pParam, const Type* pType);
    const Type* GetTypeFromName(String* pName, Object* pContext);
    TRefCountedPtr<Object> ResolvePropertyName(String* pName, Object* pItem, const Type* pOwnerType, Object* pContext, bool throwOnError);

 

 

 

 

    void PrepareSourceValueInfo();

    ItemStructList<SourceValueInfo> m_arySVI;
    ItemStructList<Object*> m_parameters;

    String m_path;

    TRefCountedPtr<PropertyPathWorker> m_pSingleWorker;
};

typedef TRefCountedPtr<PropertyPath> PropertyPathPtr;

class PathParser
{
public:
    PathParser() { };

    void Parse(const String& str, ItemStructList<SourceValueInfo>* pParsedPath);

private:

    void AddIndexer(ItemStructList<SourceValueInfo>* pParsedPath);
    void AddProperty(ItemStructList<SourceValueInfo>* pParsedPath);
    void StartNewLevel();
    bool IsSpecialChar(WCHAR wch);

    enum State
    {
        InitState,
        DoneState,
        DrillInState,
        PropState
    };

    int m_n;
    int m_index;
    DrillIn::Enum m_drillIn;
    String m_path;
    State m_state;
};

class PropertyPathTypeConverter : public TypeConverter
{
    virtual bool CanConvertFrom(const Type& type) const;
    virtual ObjectPtr ConvertFrom(const Object* pObjectFrom) const;
    virtual bool CanConvertTo(const Type& type) const { return false; };
    virtual ObjectPtr ConvertTo(const Object* pObjectFrom, const Type& type) const { LibraryCriticalError(); return NULL; }
};

}; // namespace AVUI
