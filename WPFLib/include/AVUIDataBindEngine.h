#pragma once

#include <AVUIObjectRef.h>

namespace AVUI {

class Dispatcher;
class DataBindEngine;

class IDataBindEngineClient
{
public:
    virtual bool ClientAttachToContext(bool lastChance) = 0;
    virtual void ClientOnTargetUpdated() = 0;
    virtual void ClientTransferValue() = 0;
    virtual void ClientUpdateValue() = 0;

    // Properties
    virtual DependencyObject* get_ClientTargetElement() = 0;
};

class TaskOps
{
public:
    enum Enum
    {
        TransferValue,
        UpdateValue,
        AttachToContext,
        RaiseTargetUpdatedEvent
    };
};


class DataBindEngine
{
public:
    DataBindEngine();
    ~DataBindEngine();

    static DataBindEngine* get_CurrentDataBindEngine();
    static Dispatcher* get_Dispatcher();
    void AddTask(IDataBindEngineClient* pEngineClient, TaskOps::Enum op);
    void CancelTasks(IDataBindEngineClient* pEngineClient);
    Object* GetDefaultValueConverter(const Type* pSourceType, const Type* pTargetType, bool targetToSource);
    void Run(bool isLastChance);

private:

    struct Task
    {
        Task() : pEngineClient(NULL), Op(TaskOps::AttachToContext) { }
        Task(IDataBindEngineClient* pClientIn, TaskOps::Enum opIn) : pEngineClient(pClientIn), Op(opIn) { }

        IDataBindEngineClient* pEngineClient;
        TaskOps::Enum Op;
    };

    struct ValueConverterEntry
    {
        ValueConverterEntry() : pSourceType(NULL), pTargetType(NULL), TargetToSource(false) { }
        ValueConverterEntry(const Type* pSourceTypeIn, const Type* pTargetTypeIn, bool targetToSourceIn, Object* pValueConverterIn) : 
                           pSourceType(pSourceTypeIn), pTargetType(pTargetTypeIn), TargetToSource(targetToSourceIn), pValueConverter(pValueConverterIn) { }

        const Type* pSourceType;
        const Type* pTargetType;
        bool TargetToSource;
        TRefCountedPtr<Object> pValueConverter;
    };

    ItemStructList<ValueConverterEntry> m_cachedConverters;
    ItemStructList<Task> m_pendingTasks;
    static TRefCountedPtr<DataBindEngine> s_pEngine;
};


};

