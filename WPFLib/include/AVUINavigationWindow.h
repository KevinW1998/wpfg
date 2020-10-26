#pragma once

#include <AVUIContentControl.h>
#include <AVUIWindow.h>

namespace AVUI {

class CancelEventArgs : public EventArgs
{
public:
    CancelEventArgs() : m_cancel(false) { }
    CancelEventArgs(bool isCanceled) : m_cancel(isCanceled) { }

    DECLARE_ELEMENT(CancelEventArgs, EventArgs);

    bool get_Cancel() { return m_cancel; }
    void set_Cancel(bool isCanceled) { m_cancel = isCanceled; }
private:
    bool m_cancel;
};

class NavigatingCancelEventArgs : public CancelEventArgs
{
public:
    NavigatingCancelEventArgs(String* pUri, Object* pContent) : m_pUri(pUri), m_pContent(pContent) { };

    DECLARE_ELEMENT(NavigatingCancelEventArgs, CancelEventArgs);

    String* get_Uri() { return m_pUri; }
    Object* get_Content() { return m_pContent; }

private:

    TRefCountedPtr<String> m_pUri;
    TRefCountedPtr<Object> m_pContent;
};


DEFINE_EVENTHANDLER(Cancel);
DEFINE_EVENTHANDLER(NavigatingCancel);

class NavigationWindow : public Window
{
public:

    DECLARE_ELEMENT(NavigationWindow, Window);

    IMPLEMENT_EVENT(Navigating, NavigatingCancelEventArgs);
    IMPLEMENT_EVENT(Navigated, EventArgs);

    void Navigate(const String& navTo);
    void Navigate(Object* pContent);
};


}; // namespace AVUI
