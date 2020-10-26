#include <AVUICommon.h>
#include <AVUIStyleHelper.h>
#include <AVUIUIElement.h>
#include <AVUITrigger.h>
#include <AVUIEventTrigger.h>
#include <AVUIControl.h>
#include <AVUIControlTemplate.h>
#include <AVUIContentPresenter.h>
#include <AVUIMultiTrigger.h>

namespace AVUI {

TRefCountedPtr<RoutedEventHandler> StyleHelper::ms_pEventTriggerHandlerOnContainer;


bool StyleHelper::ShouldGetValueFromStyle(const DependencyProperty& dp)
{
    if(dp != UIElement::get_StyleProperty())
    {
        return true;
    }

    return false;
}

Object* StyleHelper::GetChildValue(DependencyObject* pContainer, int childIndex, UIElement* pChild, const DependencyProperty& dp, ItemStructList<ChildRecord>& childRecordFromChildIndex, EffectiveValueEntry* pEntry, ValueLookupType::Enum* pSourceType)
{
    Object* pObject = DependencyProperty::get_UnsetValue();
    *pSourceType = ValueLookupType::Simple;

    if(childIndex < 0 || childIndex >= (int)childRecordFromChildIndex.get_Count())
    {
        return pObject;
    }

    ChildRecord& record = childRecordFromChildIndex[childIndex];

    if(record.ValueLookupListFromProperty.get_Count() == 0)
    {
        return pObject;
    }

    int index = record.ValueLookupListFromProperty.Search(dp.get_PropertyID());
    if(index < 0)
    {
        return pObject;
    }

    return GetChildValueHelper(record.ValueLookupListFromProperty[index], dp, pContainer, pChild, childIndex, true, pEntry, pSourceType);
}

Object* StyleHelper::GetChildValueHelper(ItemStructList<ChildValueLookup>& valueLookupList, const DependencyProperty& dp, DependencyObject* pContainer, UIElement* pChild, int childIndex, bool isStyleLookup, EffectiveValueEntry* pEntry, ValueLookupType::Enum* pSourceType)
{
    Object* pObject = DependencyProperty::get_UnsetValue();

    *pSourceType = ValueLookupType::Simple;

    for(int index = valueLookupList.get_Count() - 1; index >= 0 && pObject == DependencyProperty::get_UnsetValue(); index--)
    {
        ChildValueLookup& lookup = valueLookupList[index];

        if(lookup.LookupType == ValueLookupType::Simple)
        {
            pObject = lookup.Value;
        }
        else if(lookup.LookupType == ValueLookupType::Trigger)
        {
            bool isTriggerTrue = true;

            if(lookup.Conditions != NULL)
            {
                TriggerConditionArray& conditions = *(lookup.Conditions);

                for(int conditionIndex = 0; conditionIndex < conditions.get_Length() && isTriggerTrue; conditionIndex++)
                {
                    TriggerCondition& condition = conditions[conditionIndex];

                    DependencyObject* pDOQuery = NULL;
                    if(condition.SourceChildIndex == 0)
                    {
                        pDOQuery = pContainer;
                    }
                    else
                    {
                        pDOQuery = ControlTemplate::GetTemplateChildByIndex(pContainer, condition.SourceChildIndex);
                    }

                    Object* pConditionValue = pDOQuery->GetValue(*(condition.pProperty));

                    if(condition.LogicalOp == LogicalOp::Equals)
                    {
                        isTriggerTrue = Object::Equals(condition.Value, pConditionValue);
                    }
                    else
                    {
                        isTriggerTrue = !(Object::Equals(condition.Value, pConditionValue));
                    }
                }
            }
            if(isTriggerTrue)
            {
                pObject = lookup.Value;
            }
        }
    }

    if(pObject != DependencyProperty::get_UnsetValue())
    {
        pEntry->set_Value(pObject);
    }

    return pObject;
}

void StyleHelper::AddContainerDependent(const DependencyProperty& dp, bool isFromVisualTrigger, ItemStructList<ContainerDependent>& containerDependents)
{
    for(int i = 0; i < containerDependents.get_Count(); i++)
    {
        ContainerDependent& dependent = containerDependents[i];
        if(dp == *(dependent.pProperty))
        {
            dependent.FromVisualTrigger |= isFromVisualTrigger;
            return;
        }
    }

    ContainerDependent newDependent;

    newDependent.pProperty = &dp;
    newDependent.FromVisualTrigger = isFromVisualTrigger;
    containerDependents.AddItem(newDependent);
}


void StyleHelper::AddPropertyTriggerWithAction(TriggerBase* pTrigger, const DependencyProperty& dp, ItemStructMap<ObjectPtr>& triggersWithActions)
{
    int index = triggersWithActions.Search(dp.get_PropertyID());
    if(index < 0)
    {
        index = triggersWithActions.EnsureEntry(dp.get_PropertyID());
        triggersWithActions[index] = pTrigger;
    }
    else
    {
        if(triggersWithActions[index]->IsType(GET_TYPE(TriggerBase)))
        {
            ObjectPtr currentValue = triggersWithActions[index];
            TriggerCollection* pTriggerCollection = new TriggerCollection();

            pTriggerCollection->Add(object_cast<TriggerBase>(currentValue));
            pTriggerCollection->Add(pTrigger);

            triggersWithActions[index] = pTriggerCollection;

            pTriggerCollection->Release();
        }
        else
        {
            TriggerCollection& collection = *(object_cast<TriggerCollection>(triggersWithActions[index]));
            collection.Add(pTrigger);
        }

    }
}

void StyleHelper::AddPropertyDependent(int childIndex, const DependencyProperty& dp, ItemStructList<ChildPropertyDependent>& propertyDependents)
{
    ChildPropertyDependent dependent;
    dependent.ChildIndex = childIndex;
    dependent.pProperty = &dp;
    propertyDependents.AddItem(dependent);
}

 
void StyleHelper::UpdateTables(PropertyValue& propertyValue, int childIndex, ItemStructList<ChildRecord>& childRecordFromChildIndex, ItemStructList<ItemStructMap<TriggerSourceRecord> >& triggerSourceRecordFromChildIndex, bool* pHasInstanceValues)
{
    Object* pObject = propertyValue.Value;

    childRecordFromChildIndex.EnsureIndex(childIndex);
    ChildRecord& childRecord = childRecordFromChildIndex[childIndex];
    int index = childRecord.ValueLookupListFromProperty.EnsureEntry(propertyValue.pProperty->get_PropertyID());

    ChildValueLookup lookup;

    lookup.LookupType = (ValueLookupType::Enum) propertyValue.ValueType;
    lookup.Conditions = propertyValue.Conditions;
    lookup.pProperty = propertyValue.pProperty;
    lookup.Value = pObject;
    childRecord.ValueLookupListFromProperty[index].AddItem(lookup);
    childRecordFromChildIndex[childIndex] = childRecord;

    if(propertyValue.ValueType == PropertyValueType::Set)
    {
        *pHasInstanceValues = true;
        return;
    }

    if(propertyValue.ValueType == PropertyValueType::Trigger)
    {
        if(propertyValue.Conditions != NULL)
        {
            TriggerConditionArray& conditions = *(lookup.Conditions);

            for(int conditionIndex = 0; conditionIndex < conditions.get_Length(); conditionIndex++)
            {                
                TriggerCondition& condition = conditions[conditionIndex];
                int sourceChildIndex = condition.SourceChildIndex;
                triggerSourceRecordFromChildIndex.EnsureIndex(sourceChildIndex);
                ItemStructMap<TriggerSourceRecord>& map = triggerSourceRecordFromChildIndex[sourceChildIndex];

                int mapIndex = map.EnsureEntry(condition.pProperty->get_PropertyID());
                StyleHelper::AddPropertyDependent(childIndex, *(propertyValue.pProperty), map[mapIndex].ChildPropertyDependents);
            }
        }
    }
}

void StyleHelper::OnTriggerSourcePropertyInvalidated(DependencyObject* pContainer, ControlTemplate* pTemplate, const DependencyProperty& dp, const DependencyPropertyChangedEventArgs& e, ItemStructList<ItemStructMap<TriggerSourceRecord> >& triggerSourceRecordFromChildIndex, ItemStructMap<ObjectPtr>& propertyTriggersWithActions, int sourceChildIndex)
{
      if ((0 <= sourceChildIndex) && (sourceChildIndex < (int)triggerSourceRecordFromChildIndex.get_Count()))
      {
            ItemStructMap<TriggerSourceRecord>& map = triggerSourceRecordFromChildIndex[sourceChildIndex];
            int index = map.Search(dp.get_PropertyID());
            if (index >= 0)
            {
                  TriggerSourceRecord& record = map[index];
                  StyleHelper::InvalidateDependents(pContainer, pTemplate, dp, record.ChildPropertyDependents);
            }
      }

      int actionsIndex = propertyTriggersWithActions.Search(dp.get_PropertyID());
      if(actionsIndex >= 0)
      {
          Object* pObject = propertyTriggersWithActions[actionsIndex];
          if (pObject != DependencyProperty::get_UnsetValue() && pObject != NULL)
          {
              if(pObject->IsType(GET_TYPE(TriggerBase)))
              {
                  TriggerBase* pTriggerBase = object_cast<TriggerBase>(pObject);
                  StyleHelper::InvokePropertyTriggerActions(pTriggerBase, pContainer, pTemplate, dp, e, sourceChildIndex);
              }
              else
              {
                  TriggerCollection& triggerCollection = *(object_cast<TriggerCollection>(pObject));

                  for (int index = 0; index < triggerCollection.get_Count(); index++)
                  {
                      StyleHelper::InvokePropertyTriggerActions(triggerCollection[index], pContainer, pTemplate, dp, e, sourceChildIndex);
                  }
              }
          }
      }
}

void StyleHelper::InvalidateDependents(DependencyObject* pContainer, ControlTemplate* pTemplate, const DependencyProperty& dp, ItemStructList<ChildPropertyDependent>& dependents)
{
    for (int i = 0; i < dependents.get_Count(); i++)
    {
        const DependencyProperty& property = *(dependents[i].pProperty);
        DependencyObject* pDOInvalidate = NULL;

        if(dependents[i].ChildIndex == 0)
        {
            pDOInvalidate = pContainer;
        }
        else
        {
            pDOInvalidate = pTemplate->GetTemplateChildByIndex(pContainer, dependents[i].ChildIndex);
        }

        if ((pDOInvalidate != NULL) && (pDOInvalidate->ReadLocalValue(property) == DependencyProperty::get_UnsetValue()))
        {
            pDOInvalidate->InvalidateProperty(property);
        }
    }
}

void StyleHelper::InvokePropertyTriggerActions(TriggerBase* pTriggerBase, DependencyObject* pTriggerContainer, ControlTemplate* pTemplate, const DependencyProperty& changedProperty, const DependencyPropertyChangedEventArgs& e, int sourceChildIndex)
{
    bool oldState = false;
    bool newState = false;


    if(pTriggerBase->IsType(GET_TYPE(Trigger)))
    {
        EvaluateOldNewStates((Trigger*) pTriggerBase, pTriggerContainer, changedProperty, e, sourceChildIndex, &oldState, &newState);
    }
    else
    {
        EvaluateOldNewStates((MultiTrigger*) pTriggerBase, pTriggerContainer, changedProperty, e, sourceChildIndex, &oldState, &newState);
    }

    InvokeEnterOrExitActions(pTriggerBase, oldState, newState, pTriggerContainer, pTemplate);
}


void StyleHelper::EvaluateOldNewStates(Trigger* pTrigger, DependencyObject* pTriggerContainer, const DependencyProperty& changedProperty, const DependencyPropertyChangedEventArgs& e, int sourceChildIndex, bool* pOldState, bool* pNewState)
{
    int index = 0;

    if(pTrigger->get_SourceName() != NULL)
    {
        // TBD - Source name property? Not sure if we support this.
    }

    if(sourceChildIndex == index)
    {
        *pOldState = Object::Equals(e.pOldValue, pTrigger->get_Value());
        *pNewState = Object::Equals(e.pNewValue, pTrigger->get_Value());
    }
    else
    {
        *pOldState = *pNewState = false;
    }

}

void StyleHelper::EvaluateOldNewStates(MultiTrigger* pTrigger, DependencyObject* pTriggerContainer, const DependencyProperty& changedProperty, const DependencyPropertyChangedEventArgs& e, int sourceChildIndex, bool* pOldState, bool* pNewState)
{
    int index = 0;

    TriggerConditionArray* pConditions = pTrigger->get_TriggerConditions();

    *pOldState = false;
    *pNewState = false;

    for(int i = 0; i < pConditions->get_Length(); i++)
    {
        TriggerCondition* pCondition = &((*pConditions)[i]);

        if(pCondition->pProperty == &changedProperty && index == sourceChildIndex)
        {
            *pOldState = pCondition->Match(e.pOldValue);
            *pNewState = pCondition->Match(e.pNewValue);

            if(*pOldState == *pNewState)
            {
                return;
            }
        }
        else
        {
            Object* pState = pTriggerContainer->GetValue(*(pCondition->pProperty));
            if(!pCondition->Match(pState))
            {
                *pOldState = false;
                *pNewState = false;
                return;
            }
        }
    }
}

void StyleHelper::InvokeEnterOrExitActions(TriggerBase* pTriggerBase, bool oldState, bool newState, DependencyObject* pTriggerContainer, ControlTemplate* pTemplate)
{
      TriggerActionCollection* pCollection = NULL;
      if (!oldState && newState)
      {
            pCollection = pTriggerBase->get_EnterActions();
      }
      else if (oldState && !newState)
      {
            pCollection = pTriggerBase->get_ExitActions();
      }

      if(pCollection != NULL)
      {
          InvokeActions(*pCollection, pTriggerBase, pTriggerContainer, pTemplate);
      }
}

 
void StyleHelper::InvokeActions(TriggerActionCollection& triggerActions, TriggerBase* pTriggerBase, DependencyObject* pTriggerContainer, ControlTemplate* pTemplate)
{
    for(int i = 0; i < triggerActions.get_Count(); i++)
    {
        triggerActions[i]->Invoke(pTriggerContainer, pTemplate);
    }
}


void StyleHelper::InvalidateContainerDependents(DependencyObject* pContainer, ItemStructList<ContainerDependent>* pOldDependents, ItemStructList<ContainerDependent>* pNewDependents)
{
    if(pOldDependents != NULL)
    {
        for(int i = 0; i < pOldDependents->get_Count(); i++)
        {
            const DependencyProperty& property = *((*pOldDependents)[i].pProperty);
            if(pContainer->ReadLocalValue(property) == DependencyProperty::get_UnsetValue())
            {
                pContainer->InvalidateProperty(property);
            }
        }
    }

    if(pNewDependents != NULL)
    {
        for(int i = 0; i < pNewDependents->get_Count(); i++)
        {
            const DependencyProperty& property = *((*pNewDependents)[i].pProperty);
            if(pContainer->ReadLocalValue(property) == DependencyProperty::get_UnsetValue())
            {
                pContainer->InvalidateProperty(property);
            }
        }
    }
}

void StyleHelper::DoStyleInvalidations(UIElement* pUIElement, Style* pOldStyle, Style* pNewStyle)
{
    ItemStructList<ContainerDependent>* pOldDependents = NULL;
    ItemStructList<ContainerDependent>* pNewDependents = NULL;

    if(pOldStyle != NULL)
    {
        pOldDependents = &(pOldStyle->ContainerDependents);
    }

    if(pNewStyle != NULL)
    {
        pNewDependents = &(pNewStyle->ContainerDependents);
    }

    InvalidateContainerDependents(pUIElement, pOldDependents, pNewDependents);
}

void StyleHelper::UpdateStyleCache(UIElement* pUIElement, Style* pOldStyle, Style* pNewStyle, Style** ppStyleCache)
{
    if(pNewStyle != NULL)
    {
        pNewStyle->Seal();
    }
    *ppStyleCache = pNewStyle;

    DoStyleInvalidations(pUIElement, pOldStyle, pNewStyle);
    ExecuteOnApplyEnterExitActions(pUIElement, pNewStyle);
}

void StyleHelper::ExecuteOnApplyEnterExitActions(UIElement* pUIElement, Style* pStyle)
{
    if (pStyle != NULL && pStyle->PropertyTriggersWithActions.get_Count() != 0)
    {
        TriggerCollection* pTriggers = pStyle->get_Triggers();
        ExecuteOnApplyEnterExitActionsLoop(pUIElement, pTriggers, pStyle, NULL);
    }
}

void StyleHelper::ExecuteOnApplyEnterExitActionsLoop(DependencyObject* pTriggerContainer, TriggerCollection* pTriggers, Style* pStyle, ControlTemplate* pTemplate)
{
    for (int i = 0; i < pTriggers->get_Count(); i++)
    {
        TriggerBase* pTriggerBase = (*pTriggers)[i];
        if (((pTriggerBase->get_HasEnterActions() || pTriggerBase->get_HasExitActions()) && (pTriggerBase->get_ExecuteEnterActionsOnApply() || pTriggerBase->get_ExecuteExitActionsOnApply())) && NoSourceNameInTrigger(pTriggerBase))
        {
            bool currentState = pTriggerBase->GetCurrentState(pTriggerContainer);
            if (currentState && pTriggerBase->get_ExecuteEnterActionsOnApply())
            {
                InvokeActions(*(pTriggerBase->get_EnterActions()), pTriggerBase, pTriggerContainer, pTemplate);
            }
            else if (!currentState && pTriggerBase->get_ExecuteExitActionsOnApply())
            {
                InvokeActions(*(pTriggerBase->get_ExitActions()), pTriggerBase, pTriggerContainer, pTemplate);
            }
        }
    }
}

void StyleHelper::ExecuteOnApplyEnterExitActions(UIElement* pElement, ControlTemplate* pTemplate)
{
    if (pTemplate != NULL && pTemplate->PropertyTriggersWithActions.get_Count() != 0)
    {
        TriggerCollection* pTriggers = pTemplate->get_Triggers();
        ExecuteOnApplyEnterExitActionsLoop(pElement, pTriggers, NULL, pTemplate);
    }
}

 


bool StyleHelper::NoSourceNameInTrigger(TriggerBase* pTriggerBase)
{
    Trigger* pTrigger = object_cast<Trigger>(pTriggerBase);
    if (pTrigger != NULL)
    {
        return (pTrigger->get_SourceName() == NULL);
    }
    /*
    MultiTrigger trigger2 = triggerBase as MultiTrigger;
    if (trigger2 != null)
    {
        for (int i = 0; i < trigger2.Conditions.Count; i++)
        {
            if (trigger2.Conditions[i].SourceName != null)
            {
                return false;
            }
        }
    }
    */

    return true;
}

 

 



void StyleHelper::ProcessEventTrigger(EventTrigger* pTrigger, ItemStructList<Style::TriggerActionRecord> *pTriggerActionRecords, EventHandlersStore* pEventHandlersStore)
{
    if(pTrigger == NULL)
    {
        return;
    }

    ItemStructList<TRefCountedPtr<TriggerAction> >* pTriggerActions = EnsureTriggerActionList(pTriggerActionRecords, pTrigger->get_RoutedEvent());

    for (int idx = 0; idx < pTrigger->get_Actions()->get_Count(); idx++)
    {
          TriggerAction* pAction = (*(pTrigger->get_Actions()))[idx];
          pTriggerActions->AddItem(pAction);
    }

    StyleHelper::AddDelegateToFireTrigger(pTrigger->get_RoutedEvent(), pEventHandlersStore);
}

ItemStructList<TRefCountedPtr<TriggerAction> >* StyleHelper::EnsureTriggerActionList(ItemStructList<Style::TriggerActionRecord> *pTriggerActionRecords, const RoutedEvent* pEvent)
{
    for(int idx = 0; idx < pTriggerActionRecords->get_Count(); idx++)
    {
        if((*pTriggerActionRecords)[idx].pEvent == pEvent)
        {
            return (*pTriggerActionRecords)[idx].pActions;
        }
    }

    pTriggerActionRecords->AddItem(Style::TriggerActionRecord(object_allocate<ItemStructList<TRefCountedPtr<TriggerAction> > >(), pEvent));

    return (*pTriggerActionRecords)[pTriggerActionRecords->get_Count() - 1].pActions;
}

void StyleHelper::AddDelegateToFireTrigger(const RoutedEvent* pRoutedEvent, EventHandlersStore* pEventHandlersStore)
{
    if(ms_pEventTriggerHandlerOnContainer == NULL)
    {
        ms_pEventTriggerHandlerOnContainer = RoutedEventHandler::Create(StyleHelper::ExecuteEventTriggerActionsOnContainer);
    }
    pEventHandlersStore->AddRoutedEventHandler(pRoutedEvent, ms_pEventTriggerHandlerOnContainer);
}

void StyleHelper::ExecuteEventTriggerActionsOnContainer(Object* pSender, RoutedEventArgs* pArgs)
{
    UIElement* pUIElement = object_cast<UIElement>(pSender);

    if(pUIElement == NULL)
    {
        return;
    }

    Style* pStyle = pUIElement->get_Style();

    if(pStyle == NULL)
    {
        return;
    }

    StyleHelper::InvokeEventTriggerActions(pUIElement, pStyle, pArgs->get_RoutedEvent());
}


void StyleHelper::InvokeEventTriggerActions(UIElement* pUIElement, Style* pStyle, const RoutedEvent* pEvent)
{
    ItemStructList<TRefCountedPtr<TriggerAction> >* pActions = pStyle->GetActionsForEvent(pEvent);

    if (pActions != NULL)
    {
        for(int idx = 0; idx < pActions->get_Count(); idx++)
        {
            Control* pControl = object_cast<Control>(pUIElement);
            ControlTemplate* pTemplate = pControl != NULL ? pControl->get_Template() : NULL;

            (*pActions)[idx]->Invoke(pUIElement, pTemplate);
        }
    }
}

bool StyleHelper::GetValueFromTemplatedParent(DependencyObject* pContainer, int childIndex, UIElement* pChild, const DependencyProperty& dp, ItemStructList<ChildRecord>& childRecordFromChildIndex, EffectiveValueEntry* pEntry)
{
    ValueLookupType::Enum sourceType = ValueLookupType::Simple;

    if (GetChildValue(pContainer, childIndex, pChild, dp, childRecordFromChildIndex, pEntry, &sourceType) != DependencyProperty::get_UnsetValue())
    {
        if (sourceType == ValueLookupType::Trigger || sourceType == ValueLookupType::PropertyTriggerResource || sourceType == ValueLookupType::DataTrigger || sourceType == ValueLookupType::DataTriggerResource)
        {
            pEntry->set_BaseValueSource(BaseValueSource::ParentTemplateTrigger);
        }
        else
        {
            pEntry->set_BaseValueSource(BaseValueSource::ParentTemplate);
        }
        return true;
    }

    /*
    // TODO -- Not sure when this happens
    if (child.StoresParentTemplateValues)
    {
        HybridDictionary dictionary = ParentTemplateValuesField.GetValue(child.DO);
        if (dictionary.Contains(dp))
        {
            entry.BaseValueSourceInternal = BaseValueSourceInternal.ParentTemplate;
            object obj2 = dictionary[dp];
            entry.Value = obj2;
            if (obj2 is MarkupExtension)
            {
                GetInstanceValue(TemplateDataField, container, child.FE, child.FCE, childIndex, dp, -1, ref entry);
            }
            return true;
        }
    }
    */
    return false;
}

 
bool StyleHelper::GetValueFromStyleOrTemplate(UIElement* pUIElement, const DependencyProperty& dp, EffectiveValueEntry* pEntry)
{
    ValueLookupType::Enum sourceType = ValueLookupType::Simple;
    ObjectPtr pUnsetValue = DependencyProperty::get_UnsetValue();
    EffectiveValueEntry entry2 = *pEntry;
    Style* pStyle = pUIElement->get_Style();

    if (pStyle != NULL && ShouldGetValueFromStyle(dp))
    {
        ObjectPtr pObj3 = GetChildValue(pUIElement, 0, pUIElement, dp, pStyle->ChildRecordFromChildIndex, &entry2, &sourceType);
        if (pObj3 != DependencyProperty::get_UnsetValue())
        {
            switch (sourceType)
            {
                case ValueLookupType::Trigger:
                case ValueLookupType::PropertyTriggerResource:
                case ValueLookupType::DataTrigger:
                case ValueLookupType::DataTriggerResource:
                    *pEntry = entry2;
                    pEntry->set_BaseValueSource(BaseValueSource::StyleTrigger);
                    return true;
            }
            pUnsetValue = pObj3;
        }
    }
    if (ShouldGetValueFromTemplate(dp))
    {
        Control* pControl = object_cast<Control>(pUIElement);
        ControlTemplate* pTemplate = (pControl != NULL) ? pControl->get_Template() : NULL;
        if (pTemplate != NULL && GetChildValue(pUIElement, 0, pUIElement, dp, pTemplate->ChildRecordFromChildIndex, pEntry, &sourceType) != DependencyProperty::get_UnsetValue())
        {
            pEntry->set_BaseValueSource(BaseValueSource::TemplateTrigger);
            return true;
        }
    }
    if (pUnsetValue != DependencyProperty::get_UnsetValue())
    {
        *pEntry = entry2;
        pEntry->set_BaseValueSource(BaseValueSource::Style);
        return true;
    }

    /*
    if (ShouldGetValueFromThemeStyle(dp))
    {
        Style themeStyle = fo.ThemeStyle;
        if ((themeStyle != null) && (GetChildValue(ThemeStyleDataField, fo.DO, 0, fo, dp, ref themeStyle.ChildRecordFromChildIndex, ref entry, out sourceType, null) != DependencyProperty.UnsetValue))
        {
            if (((sourceType == ValueLookupType.Trigger) || (sourceType == ValueLookupType.PropertyTriggerResource)) || ((sourceType == ValueLookupType.DataTrigger) || (sourceType == ValueLookupType.DataTriggerResource)))
            {
                entry.BaseValueSourceInternal = BaseValueSourceInternal.ThemeStyleTrigger;
            }
            else
            {
                entry.BaseValueSourceInternal = BaseValueSourceInternal.ThemeStyle;
            }
            return true;
        }
    }
    */
    
    return false;
}

 

bool StyleHelper::ShouldGetValueFromTemplate(const DependencyProperty& dp)
{
    if (dp != UIElement::get_StyleProperty() && dp != Control::get_TemplateProperty() && dp != ContentPresenter::get_TemplateProperty())
    {
        return true;
    }
    return false;
}

 

 
TRefCountedPtr<Expression> StyleHelper::GetExpression(DependencyObject* pTarget, const DependencyProperty* pDP)
{
    UIElement* pElement = object_cast<UIElement>(pTarget);

    bool flag = pElement != NULL ? pElement->get_IsInitialized() : true;
    if (!flag)
    {
        pElement->set_IsInitialized(true);
    }

    TRefCountedPtr<Expression> pExpression = GetExpressionCore(pTarget, pDP, pDP->GetMetadata(pTarget->get_Type()));

    if (!flag)
    {
        pElement->set_IsInitialized(false);
    }
    return pExpression;
}

TRefCountedPtr<Expression> StyleHelper::GetExpressionCore(DependencyObject* pTarget, const DependencyProperty* pDP, DependencyPropertyMetadata* pMetadata)
{
    UIElement* pElement = object_cast<UIElement>(pTarget);

    if(pElement != NULL)
    {
        return pElement->GetExpressionCore(pDP, pMetadata);
    }
    return NULL;

}

void StyleHelper::InvalidatePropertiesOnTemplateNode(DependencyObject* pContainer, DependencyObject* pChild, int childIndex, ItemStructList<ChildRecord>& childRecordFromChildIndex, bool isDetach)
{
   if ((0 <= childIndex) && (childIndex < childRecordFromChildIndex.get_Count()))
    {
        ChildRecord& record = childRecordFromChildIndex[childIndex];
        int count = record.ValueLookupListFromProperty.get_Count();
        if (count > 0)
        {
            for (int i = 0; i < count; i++)
            {
                const DependencyProperty* pDP = record.ValueLookupListFromProperty[i][0].pProperty;
                if (!isDetach)
                {
                    ApplyTemplatedParentValue(pContainer, pChild, childIndex, childRecordFromChildIndex, pDP);
                }
                else if (pDP != &(UIElement::get_StyleProperty()))
                {
                    bool flag = true;
                    /*
                    if (dp.IsPotentiallyInherited)
                    {
                        PropertyMetadata metadata = dp.GetMetadata(child.DO.DependencyObjectType);
                        if ((metadata != null) && metadata.IsInherited)
                        {
                            flag = false;
                        }
                    }
                    */

                    if (flag)
                    {
                        pChild->InvalidateProperty(*pDP);
                    }
                }
            }
        }
    }
}

void StyleHelper::ApplyTemplatedParentValue(DependencyObject* pContainer, DependencyObject* pChild, int childIndex, ItemStructList<ChildRecord>& childRecordFromChildIndex, const DependencyProperty* pDP)
{
    EffectiveValueEntry entry(*pDP);
    entry.set_Value(DependencyProperty::get_UnsetValue());

    if (GetValueFromTemplatedParent(pContainer, childIndex, object_cast<UIElement>(pChild), *pDP, childRecordFromChildIndex, &entry))
    {
        pChild->UpdateEffectiveValue(pChild->LookupEntry(pDP->get_PropertyID()), *pDP, pDP->GetMetadata(pChild->get_Type()), EffectiveValueEntry(), &entry, false, OperationType::Unknown);
    }
}

void StyleHelper::CleanupStaticAllocations()
{
    ms_pEventTriggerHandlerOnContainer = NULL;
}


}; // namespace AVUI
