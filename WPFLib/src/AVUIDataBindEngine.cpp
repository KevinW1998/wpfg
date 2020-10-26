#include <AVUICommon.h>
#include <AVUIDataBindEngine.h>
#include <AVUIDispatcher.h>
#include <AVUIValueConverter.h>

REGISTER_INTERFACE_TYPE(AVUI::IValueConverter);

namespace AVUI {

TRefCountedPtr<DataBindEngine> DataBindEngine::s_pEngine;

DataBindEngine::DataBindEngine()
{

}

DataBindEngine::~DataBindEngine()
{
}

DataBindEngine* DataBindEngine::get_CurrentDataBindEngine()
{
    return get_Dispatcher()->get_DataBindEngine();
}

Dispatcher* DataBindEngine::get_Dispatcher()
{
    return Dispatcher::get_CurrentDispatcher();
}

void DataBindEngine::AddTask(IDataBindEngineClient* pEngineClient, TaskOps::Enum op)
{
    m_pendingTasks.AddItem(Task(pEngineClient, op));
}

void DataBindEngine::CancelTasks(IDataBindEngineClient* pEngineClient)
{
    for(int idx = 0; idx < m_pendingTasks.get_Count(); idx++)
    {
        if(m_pendingTasks[idx].pEngineClient == pEngineClient)
        {
            m_pendingTasks.RemoveAt(idx);
            idx--;
        }
    }
}

Object* DataBindEngine::GetDefaultValueConverter(const Type* pSourceType, const Type* pTargetType, bool targetToSource)
{
    for(int i = 0; i < m_cachedConverters.get_Count(); i++)
    {
        if(m_cachedConverters[i].pSourceType == pSourceType &&
           m_cachedConverters[i].pTargetType == pTargetType &&
           m_cachedConverters[i].TargetToSource == targetToSource)
        {
            return m_cachedConverters[i].pValueConverter;
        }
    }
    
    TRefCountedPtr<Object> pConverter = DefaultValueConverter::Create(pSourceType, pTargetType, targetToSource, this);
    if(pConverter != NULL)
    {
        m_cachedConverters.AddItem(ValueConverterEntry(pSourceType, pTargetType, targetToSource, pConverter));
    }

    return pConverter;
}


void DataBindEngine::Run(bool isLastChance)
{
    int count = m_pendingTasks.get_Count();
    ItemStructList<Task> retryTasks;
    
    for(int i = 0; i < count; i++)
    {
        Task* pTask = &(m_pendingTasks[i]);

        switch (pTask->Op)
        {
        case TaskOps::TransferValue:
            pTask->pEngineClient->ClientTransferValue();
            break;
    
        case TaskOps::UpdateValue:
            pTask->pEngineClient->ClientUpdateValue();
            break;
    
        case TaskOps::AttachToContext:
            if (!pTask->pEngineClient->ClientAttachToContext(isLastChance) && !isLastChance)
            {
                retryTasks.AddItem(*pTask);
            }
            break;
    
        case TaskOps::RaiseTargetUpdatedEvent:
            pTask->pEngineClient->ClientOnTargetUpdated();
            break;
        }
    }
    m_pendingTasks = retryTasks;
}



}; // namespace AVUI


