#include <AVUICommon.h>
#include <AVUICollectionViewSource.h>
#include <AVUICollectionView.h>

REGISTER_ELEMENT_BEGIN(AVUI::CollectionViewSource)

    REGISTER_PROPERTY(View)
    REGISTER_PROPERTY(Source)
    REGISTER_PROPERTY(CollectionViewType)
REGISTER_ELEMENT_END()

namespace AVUI {

// This is quite complicated in its original implementation -- We shortcut this and just provide a basic uncached
// collectionview.
TRefCountedPtr<CollectionView> CollectionViewSource::GetDefaultCollectionView(Object* pSource)
{
    if(pSource == NULL)
    {
        return NULL;
    }

    if(interface_cast<IEnumerable>(pSource) == NULL)
    {
        return NULL;
    }

    return object_allocate<CollectionView>(pSource);
    /*
    if (!IsValidSourceForView(source))
    {
        return NULL;
    }
    TRefCountedPtr<ViewRecord> pRecord = DataBindEngine::get_Current()->GetViewRecord(pSource, get_DefaultSource(), NULL);
    if (pRecord == NULL)
    {
        return NULL;
    }
    return pRecord->get_View();
    */
}

TRefCountedPtr<CollectionView> CollectionViewSource::GetDefaultCollectionView(Object* pSource, DependencyObject* pDO)
{
    return GetDefaultCollectionView(pSource);

    /*
    CollectionView defaultCollectionView = GetDefaultCollectionView(source);
    if ((defaultCollectionView != null) && (defaultCollectionView.Culture == null))
    {
        XmlLanguage language = (d != null) ? ((XmlLanguage) d.GetValue(FrameworkElement.LanguageProperty)) : null;
        if (language == null)
        {
            return defaultCollectionView;
        }
        try
        {
            defaultCollectionView.Culture = language.GetSpecificCulture();
        }
        catch (InvalidOperationException)
        {
        }
    }
    return defaultCollectionView;
    */
}




void CollectionViewSource::OnSourceChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
/*
    CollectionViewSource source = (CollectionViewSource) d;
    source.OnSourceChanged(e.OldValue, e.NewValue);
    source.EnsureView();
*/
}

void CollectionViewSource::OnCollectionViewTypeChanged(Object* pDO, const DependencyPropertyChangedEventArgs& e)
{
/*
    CollectionViewSource source = (CollectionViewSource) d;
    Type oldValue = (Type) e.OldValue;
    Type newValue = (Type) e.NewValue;
    if (!source._isInitializing)
    {
        throw new InvalidOperationException(SR.Get(SRID.CollectionViewTypeIsInitOnly, new object[0]));
    }
    source.OnCollectionViewTypeChanged(oldValue, newValue);
    source.EnsureView();
*/
}


CollectionView* CollectionViewSource::get_CollectionView()
{
    CollectionView* pView = get_View();

    /* -- TODO - DataSourceProvider
    if (pView != null && !m_isViewInitialized)
    {
        Object* pSource = get_Source();
        DataSourceProvider* pProvider = object_cast<DataSourceProvider>(pSource);
        if (pProvider != NULL)
        {
            pSource = pProvider->get_Data();
        }
        if (pSource != null)
        {
            ViewRecord record = DataBindEngine.CurrentDataBindEngine.GetViewRecord(source, this, this.CollectionViewType);
            if (record != null)
            {
                record.InitializeView();
                this._isViewInitialized = true;
            }
        }
    }
    */
    return pView;
}
 

 




}; // namespace AVUI
