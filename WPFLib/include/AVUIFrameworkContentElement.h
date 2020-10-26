#pragma once

#include <AVUIContentElement.h>

namespace AVUI {


class FrameworkContentElement : public ContentElement
{
    friend class FrameworkObject;
public:

    DECLARE_ELEMENT(FrameworkContentElement, ContentElement);

    virtual void BeginInit() { };
    virtual void EndInit() { };

    Object* FindName(const String& Name) { return NULL; };
    Object* FindName(const String& Name, DependencyObject** ppScopeOwner) { return NULL; };

    DependencyObject* get_Parent() { return NULL; }

    void RaiseEvent(RoutedEventArgs* pArgs) { };

    IMPLEMENT_ROUTED_EVENT(Loaded, RoutedEventArgs, RoutingStrategy::Broadcast);
    IMPLEMENT_ROUTED_EVENT(Unloaded, RoutedEventArgs, RoutingStrategy::Broadcast);
    IMPLEMENT_EVENT(ResourcesChanged, EventArgs);

protected:

    virtual DependencyObject* GetUIParentCore() { return get_Parent(); }


private:

    void OnLoaded(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }
    void OnUnloaded(RoutedEventArgs* pArgs) { RaiseEvent(pArgs); }

    void ChangeLogicalParent(DependencyObject* pNewParent) { };

};


}; // namespace AVUI
