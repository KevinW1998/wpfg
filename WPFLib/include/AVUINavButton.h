#pragma once

#include <AVUIButton.h>
#include <AVUIRoutedEventArgs.h>

namespace AVUI {

class RequestNavigateEventArgs : public RoutedEventArgs
{
public:

    DECLARE_ELEMENT(RequestNavigateEventArgs, RoutedEventArgs);

    RequestNavigateEventArgs(const RoutedEvent* pRoutedEvent, String* pUri, String* pTarget) : RoutedEventArgs(pRoutedEvent), m_pUri(pUri), m_pTarget(pTarget) { }

    const String* get_Target() const { return m_pTarget; }
    const String* get_Uri() const { return m_pUri; }

private:
    TRefCountedPtr<String> m_pTarget;
    TRefCountedPtr<String> m_pUri;
};



class NavButton : public Button
{
public:
    NavButton() { };
    ~NavButton() { };

    DECLARE_ELEMENT(NavButton, Button);

    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(NavigateUri, String, NULL, PropertyFlag::None);
    IMPLEMENT_REFERENCE_DEPENDENCY_PROPERTY(TargetName, String, NULL, PropertyFlag::None);
    IMPLEMENT_ROUTED_EVENT(RequestNavigate,  RequestNavigateEventArgs, RoutingStrategy::Bubble);

protected:

    virtual void OnClick();

};

}; // namespace AVUI
