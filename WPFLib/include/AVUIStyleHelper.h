#pragma once

#include <AVUIStyle.h>
#include <AVUIControlTemplate.h>

namespace AVUI {

class DependencyPropertyMetadata;
class DependencyProperty;
class DependencyPropertyChangedEventArgs;
class Trigger;
class MultiTrigger;
class EventTrigger;

class StyleHelper
{
public:
    static bool ShouldGetValueFromStyle(const DependencyProperty& dp);
    static bool ShouldGetValueFromTemplate(const DependencyProperty& dp);
    static bool GetValueFromStyleOrTemplate(UIElement* pUIElement, const DependencyProperty& dp, EffectiveValueEntry* pEntry);
    static bool GetValueFromTemplatedParent(DependencyObject* pContainer, int childIndex, UIElement* pChild, const DependencyProperty& dp, ItemStructList<ChildRecord>& childRecordFromChildIndex, EffectiveValueEntry* pEntry);

    static Object* GetChildValue(DependencyObject* pContainer, int childIndex, UIElement* pChild, const DependencyProperty& dp, ItemStructList<ChildRecord>& childRecordFromChildIndex, EffectiveValueEntry* pEntry, ValueLookupType::Enum* pSourceType);
    static Object* GetChildValueHelper(ItemStructList<ChildValueLookup>& valueLookupList, const DependencyProperty& dp, DependencyObject* pContainer, UIElement* pChild, int childIndex, bool isStyleLookup, EffectiveValueEntry* pEntry, ValueLookupType::Enum* pSourceType);

    static void AddContainerDependent(const DependencyProperty& dp, bool isFromVisualTrigger, ItemStructList<ContainerDependent>& containerDependents);
    static void AddPropertyDependent(int childIndex, const DependencyProperty& dp, ItemStructList<ChildPropertyDependent>& propertyDependents);
    static void AddPropertyTriggerWithAction(TriggerBase* pTrigger, const DependencyProperty& dp, ItemStructMap<ObjectPtr>& triggersWithActions);

    static void UpdateTables(PropertyValue& propertyValue, int childIndex, ItemStructList<ChildRecord>& ChildRecordFromChildIndex, ItemStructList<ItemStructMap<TriggerSourceRecord> >& TriggerSourceRecordFromChildIndex, bool* pHasInstanceValues);

    static void OnTriggerSourcePropertyInvalidated(DependencyObject* pContainer, ControlTemplate* pTemplate, const DependencyProperty& dp, const DependencyPropertyChangedEventArgs& e, ItemStructList<ItemStructMap<TriggerSourceRecord> >& triggerSourceRecordFromChildIndex, ItemStructMap<ObjectPtr>& propertyTriggersWithActions, int templateChildIndex);

    static void InvalidateDependents(DependencyObject* pContainer, ControlTemplate* pTemplate, const DependencyProperty& dp, ItemStructList<ChildPropertyDependent>& dependents);
    static void InvokePropertyTriggerActions(TriggerBase* pTriggerBase, DependencyObject* pTriggerContainer, ControlTemplate* pTemplate, const DependencyProperty& changedProperty, const DependencyPropertyChangedEventArgs& e, int sourceChildIndex);

    static void EvaluateOldNewStates(Trigger* pTrigger, DependencyObject* pTriggerContainer, const DependencyProperty& changedProperty, const DependencyPropertyChangedEventArgs& e, int sourceChildIndex, bool* pOldState, bool* pNewState);
    static void EvaluateOldNewStates(MultiTrigger* pTrigger, DependencyObject* pTriggerContainer, const DependencyProperty& changedProperty, const DependencyPropertyChangedEventArgs& e, int sourceChildIndex, bool* pOldState, bool* pNewState);

    static void InvokeEnterOrExitActions(TriggerBase* pTriggerBase, bool oldState, bool newState, DependencyObject* pTriggerContainer, ControlTemplate* pTemplate);
    static void InvokeActions(TriggerActionCollection& triggerActions, TriggerBase* pTriggerBase, DependencyObject* pTriggerContainer, ControlTemplate* pTemplate);

    static void UpdateStyleCache(UIElement* pUIElement, Style* pOldStyle, Style* pNewStyle, Style** ppStyleCache);
    static void DoStyleInvalidations(UIElement* pUIElement, Style* pOldStyle, Style* pNewStyle);
    static void InvalidateContainerDependents(DependencyObject* pContainer, ItemStructList<ContainerDependent>* pOldDependents, ItemStructList<ContainerDependent>* pNewDependents);
    static void ProcessEventTrigger(EventTrigger* pTrigger,  ItemStructList<Style::TriggerActionRecord> *pTriggerActions, EventHandlersStore* pEventHandlersStore);
    static void AddDelegateToFireTrigger(const RoutedEvent* pEvent, EventHandlersStore* pEventHandlersStore);
    static ItemStructList<TRefCountedPtr<TriggerAction> >* EnsureTriggerActionList(ItemStructList<Style::TriggerActionRecord> *pTriggerActionRecords, const RoutedEvent* pEvent);
    static void InvokeEventTriggerActions(UIElement* pUIElement, Style* pStyle, const RoutedEvent* pEvent);
    static void ExecuteEventTriggerActionsOnContainer(Object* pSender, RoutedEventArgs* pArgs);

    static void InvalidatePropertiesOnTemplateNode(DependencyObject* pContainer, DependencyObject* pChild, int childIndex, ItemStructList<ChildRecord>& childRecordFromChildIndex, bool isDetach);
    static void ApplyTemplatedParentValue(DependencyObject* pContainer, DependencyObject* pChild, int childIndex, ItemStructList<ChildRecord>& childRecordFromChildIndex, const DependencyProperty* pDP);

    static TRefCountedPtr<Expression> GetExpression(DependencyObject* pTarget, const DependencyProperty* pDP);
    static TRefCountedPtr<Expression> GetExpressionCore(DependencyObject* pTarget, const DependencyProperty* pDP, DependencyPropertyMetadata* pMetadata);
    static void ExecuteOnApplyEnterExitActions(UIElement* pUIElement, Style* pStyle);
    static void ExecuteOnApplyEnterExitActions(UIElement* pElement, ControlTemplate* pTemplate);
    static void ExecuteOnApplyEnterExitActionsLoop(DependencyObject* pTriggerContainer, TriggerCollection* pTriggers, Style* pStyle, ControlTemplate* pTemplate);
    static bool NoSourceNameInTrigger(TriggerBase* pTriggerBase);


    static void CleanupStaticAllocations();

private:

    static TRefCountedPtr<RoutedEventHandler> ms_pEventTriggerHandlerOnContainer;
 
};

}; // namespace AVUI
