#pragma once

#include <AVUIPage.h>

namespace AVUI {

class DispatcherTimer;
class NavigatingCancelEventArgs;
class NavigationWindow;

class TransitioningPage : public Page
{
public:
    TransitioningPage() { };
    ~TransitioningPage() { };

    DECLARE_ELEMENT(TransitioningPage, Page);

    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(TransitionInDelay, TimeSpan, TimeSpan::FromMilliseconds(500), PropertyFlag::None);
    IMPLEMENT_VALUE_DEPENDENCY_PROPERTY(TransitionOutDelay, TimeSpan, TimeSpan::FromMilliseconds(500), PropertyFlag::None);

    IMPLEMENT_ROUTED_EVENT(TransitionIn, RoutedEventArgs, RoutingStrategy::Direct);
    IMPLEMENT_ROUTED_EVENT(TransitionOut, RoutedEventArgs, RoutingStrategy::Direct);

};


}; // namespace AVUI
 
