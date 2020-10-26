#pragma once

#include <AVUIBindingWorker.h>

namespace AVUI {

class XmlBindingWorker;
class PropertyPathWorker;
class DataBindEngine;

// Needs a partial rewrite to deal with XML and Async!

class ClrBindingWorker : public BindingWorker
{
public:
    ClrBindingWorker(BindingExpression* pB, DataBindEngine* pEngine);


    bool AsyncGet(Object* pItem, int level);
    virtual void AttachDataItem();

 





    void CancelPendingTasks();
    DependencyObject* CheckTarget();
    void DetachDataItem();
    bool IsValidValue(Object* pValue);
    void NewValueAvailable(bool dependencySourcesChanged, bool initialValue, bool isASubPropertyChange);
    virtual void OnCurrentChanged(CollectionView* pCollectionView, EventArgs* pArgs);
    virtual void OnSourceInvalidation(DependencyObject* pDO, const DependencyProperty* pDP, bool isASubPropertyChange);
    void OnSourcePropertyChanged(Object* pObject, const Property* pProperty);
    virtual TRefCountedPtr<Object> RawValue();
    virtual void RefreshValue();
    void ReplaceCurrentItem(CollectionView* pOldCollectionView, CollectionView* pNewCollectionView);
    void SetupDefaultValueConverter(const Type* pType);
    virtual void UpdateValue(Object* pValue);
    virtual bool UsesDependencyProperty(DependencyObject* pDO, const DependencyProperty* pDP);

 
 

    virtual bool get_CanUpdate();
    virtual const Type* get_SourcePropetyType();
    bool get_TransfersDefaultValue();
    XmlBindingWorker* get_XmlWorker() { return NULL; }
 

private:

    void ReplaceDependencySources();
    void SetStatus(unsigned int status);

 

 

    PropertyPathWorker* get_PW() { return m_pPathWorker; }

    TRefCountedPtr<PropertyPathWorker> m_pPathWorker;
    ItemStructList<const Type*> m_types;
    ItemStructList<TRefCountedPtr<Object> > m_values;

};


}; // namespace AVUI

