
namespace AVUI {

typedef TDelegate<Object> DispatcherOperationCallback;

class DispatcherPriority
{
public:
    enum Enum
    {
        Invalid = -1,
        Inactive = 0,
        SystemIdle = 1,
        ApplicationIdle = 2,
        ContextIdle = 3,
        Background = 4,
        Input = 5,
        Loaded = 6,
        Render = 7,
        DataBind = 8,
        Normal = 9,
        Send = 10
    };
};


class UIElement;
class LayoutManager;
class InputManager;
class TimeManager;
class DispatcherTimer;
class DataBindEngine;
class Dispatcher
{
    friend class Application;
public:
    static Dispatcher* get_CurrentDispatcher();   

    Dispatcher();
    ~Dispatcher();

    LayoutManager* get_LayoutManager() { return m_pLayoutManager; }
    InputManager* get_InputManager() { return m_pInputManager; }
    TimeManager* get_TimeManager() { return m_pTimeManager; }
    DataBindEngine* get_DataBindEngine() { return m_pDataBindEngine; }

    void AddTimer(DispatcherTimer* pTimer);
    void RemoveTimer(DispatcherTimer* pTimer);

    void BeginInvoke(DispatcherPriority::Enum priority, DispatcherOperationCallback* pCallback, Object* pArg = NULL);
private:

    void Update();

    struct DispatcherOperation
    {
        friend class Dispatcher;
    public:
        DispatcherOperation() { }
        DispatcherOperation(DispatcherPriority::Enum priority, DispatcherOperationCallback* pCallback, Object* pArg) :
                            m_priority(priority), m_pCallback(pCallback), m_pArg(pArg) { }

    private:
        DispatcherPriority::Enum m_priority;
        TRefCountedPtr<DispatcherOperationCallback> m_pCallback;
        TRefCountedPtr<Object> m_pArg;
    };

    ItemStructList<DispatcherOperation> m_dispatcherOperations;


    LayoutManager* m_pLayoutManager;
    InputManager* m_pInputManager;
    TimeManager* m_pTimeManager;
    DataBindEngine* m_pDataBindEngine;

    ItemStructList<TRefCountedPtr<DispatcherTimer> > m_dispatcherTimers;
    ItemStructList<TRefCountedPtr<DispatcherTimer> > m_tickedTimers;
};

};
