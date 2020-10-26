#pragma once

namespace AVUI {

class PropertyChangedEventArgs;
class DataBindEngine;
class PropertyPath;
class ClrBindingWorker;
class CollectionView;
struct SourceValueInfo;

class PropertyPathStatus
{
public:

    enum Enum
    {
        Inactive,
        Active,
        PathError,
        AsyncRequestPending
    };
};
 

class PropertyPathWorker : public Object
{
    struct SourceValueState;
public:

    DECLARE_ELEMENT(PropertyPathWorker, Object);
    PropertyPathWorker(PropertyPath* pPath);
    PropertyPathWorker(PropertyPath* pPath, DataBindEngine* pEngine) { Initialize(pPath, pEngine); }
    PropertyPathWorker(PropertyPath* pPath, ClrBindingWorker* pHost, bool isDynamic, DataBindEngine* pEngine)
    {
        Initialize(pPath, pEngine);
        m_pHost = pHost;
        m_isDynamic = isDynamic;
    }

    ~PropertyPathWorker();

    void AttachToRootItem(Object* pRootItem);
    void DetachFromRootItem();

    DependencyObject* get_TreeContext();
    void set_TreeContext(DependencyObject* pTreeContext);

    Object* GetAccessor(int level);
    Object* GetItem(int level);
    SourceValueInfo* GetSourceValueInfo(int level);
    const Type* GetType(int level);
    TRefCountedPtr<Object> GetValue(Object* pItem, int level);
    int LevelForPropertyChange(Object* pItem, const Property* pProperty);
    void OnCurrentChanged(CollectionView* pCollectionView);
    void OnDependencyPropertyChanged(DependencyObject* pDO, const DependencyProperty* pDP, bool isASubPropertyChange);
    void OnNewValue(int level, Object* pValue);
    void OnPropertyChangedAtLevel(int level) { UpdateSourceValueState(level, NULL); }
    TRefCountedPtr<Object> RawValue();
    void RefreshValue();
    TRefCountedPtr<Object> SetContext(Object* pRootItem);
    void SetValue(Object* pItem, Object* pValue);
    bool UsesDependencyProperty(DependencyObject* pDO, const DependencyProperty* pDP);

 

 

 

 

 

 

 

 
    int get_Length();
    PropertyPathStatus::Enum get_Status() { return m_status; }

private:
    void OnItemPropertyChanged(Object* pSender, PropertyChangedEventArgs* pArgs);

    void CheckReadOnly(Object* pItem, Object* pInfo);

    void SetPropertyInfo(Object* pProp, const Property** ppProperty, const DependencyProperty** ppDP);
    bool ShouldConvertIndexerToProperty(Object* pItem, const Type* pNewType, String* pName);
 
    TRefCountedPtr<Object> RawValue(int k);
    void ReplaceItem(int k, Object* pNewO, Object* pParent);

 
    void GetInfo(int k, Object* pItem, SourceValueState* pSVS);

 

    void Initialize(PropertyPath* pPath, DataBindEngine* pEngine);

    void UpdateSourceValueState(int k, CollectionView* pCollectionView);
    void UpdateSourceValueState(int k, CollectionView* pCollectionView, Object* pNewValue, bool isASubPropertyChange);

 
    bool get_IsDynamic() { return m_isDynamic; }

    static Object* get_AsyncRequestPending() { static Object AsyncRequestPending; return &AsyncRequestPending; }
    static Object* get_NoParent() { static Object NoParent; return &NoParent; }

    struct SourceValueState
    {
        SourceValueState() : TypeValue(NULL) { }

        TRefCountedPtr<CollectionView> CollectionViewValue;
        TRefCountedPtr<Object> Item;
        TRefCountedPtr<Object> Info;
        const Type* TypeValue;
        int Args;
    };

    class ContextHelper : public Object
    {
    public:
        ContextHelper(PropertyPathWorker* pOwner) : m_pOwner(pOwner) { }
        ~ContextHelper()
        {
            if(m_pOwner != NULL)
            {
                m_pOwner->DetachFromRootItem();
                m_pOwner->set_TreeContext(NULL);
            }
        }

        void SetContext(Object* pRootItem)
        {
            if(m_pOwner != NULL)
            {
                m_pOwner->set_TreeContext(object_cast<DependencyObject>(pRootItem));
                m_pOwner->AttachToRootItem(pRootItem);
            }
        }

        void SetOwner(PropertyPathWorker* pOwner) { m_pOwner = pOwner; }

    private:
        PropertyPathWorker* m_pOwner;
    };

    ItemStructList<SourceValueState> m_arySVS;
    TRefCountedPtr<ContextHelper> m_pContextHelper;
    bool m_dependencySourcesChanged;
    DataBindEngine* m_pEngine;
    TWeakPtr<ClrBindingWorker> m_pHost;
    bool m_isDynamic;
    PropertyPath* m_pParent;
    TRefCountedPtr<Object> m_pRootItem;
    PropertyPathStatus::Enum m_status;
    TRefCountedPtr<Object> m_pTreeContext;
};


};


