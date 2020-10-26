#include <AVUICommon.h>
#include <AVUIClrBindingWorker.h>
#include <AVUIBinding.h>
#include <AVUIBindingExpression.h>
#include <AVUIPropertyPath.h>
#include <AVUIPropertyPathWorker.h>
#include <AVUICollectionView.h>

namespace AVUI {


ClrBindingWorker::ClrBindingWorker(BindingExpression* pB, DataBindEngine* pEngine) : BindingWorker(pB)
{
    TRefCountedPtr<PropertyPath> pPath = get_ParentBinding()->get_Path();

    /* // TODO -- XML
    if (get_ParentBinding()->get_XPath() != NULL)
    {
        path = PrepareXmlBinding(pPath);
    }
    */

    if (pPath == NULL)
    {
        pPath = object_allocate<PropertyPath>(L"");
    }
    if (get_ParentBinding()->get_Path() == NULL)
    {
        get_ParentBinding()->UsePath(pPath);
    }
    m_pPathWorker = object_allocate<PropertyPathWorker>(pPath, this, get_IsDynamic(), pEngine);
    m_pPathWorker->set_TreeContext(get_TargetElement());
}


bool ClrBindingWorker::AsyncGet(Object* pItem, int level)
{
    /* // TODO Async
    if (get_ParentBinding()->get_IsAsync())
    {
        RequestAsyncGetValue(pItem, level);

        return true;
    }
    */
    return false;
}

void ClrBindingWorker::AttachDataItem()
{
    Object* pDataItem;
    if (get_XmlWorker() == NULL)
    {
        pDataItem = get_DataItem();
    }
    else
    {   
        /* TODO -- XML
        this.XmlWorker.AttachDataItem();
        dataItem = this.XmlWorker.RawValue();
        */
    }
    get_PW()->AttachToRootItem(pDataItem);

    if (get_PW()->get_Length() == 0)
    {
        get_ParentBindingExpression()->SetupDefaultValueConverter(&(pDataItem->get_Type()));
    }
}

void ClrBindingWorker::CancelPendingTasks()
{
    get_ParentBindingExpression()->CancelPendingTasks();
}

bool ClrBindingWorker::get_CanUpdate()
{
    int level = get_PW()->get_Length() - 1;
    return level >= 0 && get_PW()->GetItem(level) != NULL && get_PW()->GetAccessor(level) != NULL;
}
const Type* ClrBindingWorker::get_SourcePropetyType()
{
    return get_PW()->GetType(get_PW()->get_Length() - 1);
}

bool ClrBindingWorker::get_TransfersDefaultValue()
{
    return get_ParentBinding()->get_TransfersDefaultValue();
}

DependencyObject* ClrBindingWorker::CheckTarget()
{
    return get_TargetElement();
}

void ClrBindingWorker::DetachDataItem()
{
    get_PW()->DetachFromRootItem();
    if (get_XmlWorker() != NULL)
    {
        // TODO XML
        //this.XmlWorker.DetachDataItem();
    }
}

bool ClrBindingWorker::IsValidValue(Object* pValue)
{
    return get_TargetProperty()->IsValidValue(pValue);
}

void ClrBindingWorker::NewValueAvailable(bool dependencySourcesChanged, bool initialValue, bool isASubPropertyChange)
{
    SetStatus(get_PW()->get_Status());
    if (dependencySourcesChanged)
    {
        ReplaceDependencySources();
    }
    if (!initialValue && get_Status() != BindingStatus::AsyncRequestPending)
    {
        get_ParentBindingExpression()->ScheduleTransfer(isASubPropertyChange);
    }
}

void ClrBindingWorker::OnCurrentChanged(CollectionView* pCollectionView, EventArgs* pArgs)
{
    /* // TODO XML
    if (this.XmlWorker != null)
    {
        this.XmlWorker.OnCurrentChanged(collectionView, args);
    }
    */

    get_PW()->OnCurrentChanged(pCollectionView);
}


void ClrBindingWorker::OnSourceInvalidation(DependencyObject* pDO, const DependencyProperty* pDP, bool isASubPropertyChange)
{
    get_PW()->OnDependencyPropertyChanged(pDO, pDP, isASubPropertyChange);
}

void ClrBindingWorker::OnSourcePropertyChanged(Object* pObject, const Property* pProperty)
{
    int num;
    if (!get_IgnoreSourcePropertyChange() && ((num = get_PW()->LevelForPropertyChange(pObject, pProperty)) >= 0))
    {
        // TODO -- Async
        //if (base.Dispatcher.Thread == Thread.CurrentThread)
        {
            get_PW()->OnPropertyChangedAtLevel(num);
        }
        /*
        else
        {
            base.SetTransferIsPending(true);
            base.Dispatcher.BeginInvoke(DispatcherPriority.DataBind, new DispatcherOperationCallback(this.ScheduleTransferOperation), new object[] { o, propName });
        }
        */
    }
}

TRefCountedPtr<Object> ClrBindingWorker::RawValue()
{
    TRefCountedPtr<Object> pObj2 = get_PW()->RawValue();
    SetStatus(get_PW()->get_Status());
    return pObj2;
}
 
void ClrBindingWorker::RefreshValue()
{
    get_PW()->RefreshValue();
}

void ClrBindingWorker::ReplaceCurrentItem(CollectionView* pOldCollectionView, CollectionView* pNewCollectionView)
{
    if (pOldCollectionView != NULL)
    {
        pOldCollectionView->remove_CurrentChanged(EventHandler::Create(get_ParentBindingExpression(), &BindingExpression::OnCollectionViewCurrentChanged));

        if (get_IsReflective())
        {
            pOldCollectionView->remove_CurrentChanging(CurrentChangingEventHandler::Create(get_ParentBindingExpression(), &BindingExpression::OnCollectionViewCurrentChanging));
        }
    }
    if (pNewCollectionView != NULL)
    {
        pNewCollectionView->add_CurrentChanged(EventHandler::Create(get_ParentBindingExpression(), &BindingExpression::OnCollectionViewCurrentChanged));

        if (get_IsReflective())
        {
            pNewCollectionView->add_CurrentChanging(CurrentChangingEventHandler::Create(get_ParentBindingExpression(), &BindingExpression::OnCollectionViewCurrentChanging));
        }
    }
}


void ClrBindingWorker::ReplaceDependencySources()
{
    if (!get_ParentBindingExpression()->get_IsDetaching())
    {
        ItemStructList<WeakDependencySource> newWorkerSources;
        int n = 0;
        if (get_IsDynamic())
        {
            for (int i = 0; i < get_PW()->get_Length(); i++)
            {
                const DependencyProperty* pAccessor = object_cast<DependencyProperty>(get_PW()->GetAccessor(i));
                if (pAccessor != NULL)
                {
                    DependencyObject* pItem = object_cast<DependencyObject>(get_PW()->GetItem(i));
                    if (pItem != NULL)
                    {
                        newWorkerSources.AddItem(WeakDependencySource(pItem, pAccessor));
                        n++;
                    }
                }
            }
        }
        get_ParentBindingExpression()->ChangeWorkerSources(&newWorkerSources, n);
    }
}

void ClrBindingWorker::SetStatus(unsigned int status)
{
    switch (status)
    {
    case PropertyPathStatus::Inactive:
        set_Status(BindingStatus::Inactive);
        return;

    case PropertyPathStatus::Active:
        set_Status(BindingStatus::Active);
        return;

    case PropertyPathStatus::PathError:
        set_Status(BindingStatus::PathError);
        return;

    case PropertyPathStatus::AsyncRequestPending:
        set_Status(BindingStatus::AsyncRequestPending);
        return;
    }
}

 
void ClrBindingWorker::SetupDefaultValueConverter(const Type* pType)
{
    get_ParentBindingExpression()->SetupDefaultValueConverter(pType);
}

 
void ClrBindingWorker::UpdateValue(Object* pValue)
{
    int level = get_PW()->get_Length() - 1;
    Object* pItem = get_PW()->GetItem(level);
    if (pItem != NULL && pItem != BindingExpression::get_NullDataItem())
    {
        /* TODO -- Async
        if (base.ParentBinding.IsAsync && !(this.PW.GetAccessor(level) is DependencyProperty))
        {
            this.RequestAsyncSetValue(item, value);
        }
        else
        */
        {
            get_PW()->SetValue(pItem, pValue);
        }
    }
}

 

 
bool ClrBindingWorker::UsesDependencyProperty(DependencyObject* pDO, const DependencyProperty* pDP)
{
    return get_PW()->UsesDependencyProperty(pDO, pDP);
}



}; // namespace AVUI

