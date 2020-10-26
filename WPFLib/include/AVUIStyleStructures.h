#pragma once

#include <AVUIItemStructMap.h>
#include <AVUIItemStructList.h>
#include <AVUIUIString.h>

namespace AVUI {

class EventHandlersStore;

struct TriggerCondition
{
    TriggerCondition() : SourceChildIndex(0), pProperty(NULL), LogicalOp(LogicalOp::Equals) { }
    bool Match(Object* pState) { return Match(pState, Value); }
    bool Match(Object* pState, Object* pValue) { if(LogicalOp == LogicalOp::Equals) return Object::Equals(pState, pValue); return !Object::Equals(pState, Value); }
    LogicalOp::Enum LogicalOp;
    const DependencyProperty* pProperty;
    StringPtr SourceName;
    int SourceChildIndex;
    ObjectPtr Value;
};

typedef ArrayInstance<TriggerCondition> TriggerConditionArray;
typedef TRefCountedPtr<TriggerConditionArray> TriggerConditionArrayPtr;

struct ContainerDependent
{
    ContainerDependent() : FromVisualTrigger(false), pProperty(NULL) { }
    bool FromVisualTrigger;
    const DependencyProperty* pProperty;
};

struct ChildValueLookup
{
    ChildValueLookup() : LookupType(ValueLookupType::Simple), pProperty(NULL) { }
    ValueLookupType::Enum LookupType;
    TriggerConditionArrayPtr Conditions;
    const DependencyProperty* pProperty;
    ObjectPtr Value;
};

struct PropertyValue
{
    PropertyValue() : pProperty(NULL), ValueType(PropertyValueType::Set) { }
    TriggerConditionArrayPtr Conditions;
    StringPtr pChildName;
    const DependencyProperty* pProperty;
    ObjectPtr Value;
    PropertyValueType::Enum ValueType;
};

struct ChildPropertyDependent
{
    ChildPropertyDependent() : ChildIndex(0), pProperty(NULL) { }
    int ChildIndex;
    ObjectPtr Name;
    const DependencyProperty* pProperty;
};

struct TriggerSourceRecord
{
    ItemStructList<ChildPropertyDependent> ChildPropertyDependents;
};


struct ChildRecord
{
public:
    ItemStructMap<ItemStructList<ChildValueLookup> > ValueLookupListFromProperty;
};



}; // namespace AVUI
