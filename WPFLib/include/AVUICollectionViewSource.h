#pragma once

#include <AVUIDependencyObject.h>

namespace AVUI {

class FilterEventArgs;
class CollectionView;

class CollectionViewSource : public DependencyObject
{
    friend class ItemCollection;
    friend class PropertyPathWorker;
public:

    DECLARE_ELEMENT(CollectionViewSource, DependencyObject);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(View, CollectionView, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(Source, Object, NULL, DependencyPropertyMetadata(PropertyFlag::None, CollectionViewSource::OnSourceChanged));
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(CollectionViewType, Type, NULL, DependencyPropertyMetadata(PropertyFlag::None, CollectionViewSource::OnCollectionViewTypeChanged));

    IMPLEMENT_EVENT(Filter, FilterEventArgs);

    CollectionView* get_CollectionView();

    const DependencyProperty* get_PropertyForInheritanceContext() { return NULL; }

private:

    static TRefCountedPtr<CollectionView> GetDefaultCollectionView(Object* pSource);
    static TRefCountedPtr<CollectionView> GetDefaultCollectionView(Object* pSource, DependencyObject* pDO);

 


    static void OnSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);
    static void OnCollectionViewTypeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e);

};

}; // namespace AVUI
