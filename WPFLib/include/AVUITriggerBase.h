#pragma once

#include <AVUIDependencyObject.h>
#include <AVUITriggerActionCollection.h>
#include <AVUIStyleStructures.h>

namespace AVUI {

class TriggerBase : public DependencyObject
{
    friend class StyleHelper;
public:
    DECLARE_ELEMENT(TriggerBase, DependencyObject);

    TriggerBase() : m_layerRank(0) { };

    void AddToPropertyValues(String* pChildName, const DependencyProperty& dp, Object* pValue, PropertyValueType::Enum valueType);
    void EstablishLayer() { m_layerRank = m_layerRankNext++; };
    void ProcessParametersContainer(const DependencyProperty& dp); 
    void ProcessParametersset_Value(const DependencyProperty& dp, Object* pValue); 

    void Seal();


    TriggerActionCollection* get_EnterActions() { return &m_enterActions; }
    bool get_HasEnterActions() { return m_enterActions.get_Count() > 0; }
    bool get_ExecuteEnterActionsOnApply() { return true; }

    TriggerActionCollection* get_ExitActions() { return &m_exitActions; }
    bool get_HasExitActions() { return m_exitActions.get_Count() > 0; }
    bool get_ExecuteExitActionsOnApply() { return false; }

    ItemStructList<PropertyValue>& get_PropertyValues() { return m_propertyValueList; }
    int get_Layer() const { return m_layerRank; }

    virtual bool GetCurrentState(DependencyObject* pContainer) { return false; };

protected:

    void set_TriggerConditions(TriggerConditionArray* pConditions) { m_pConditions = pConditions; }
    TriggerConditionArray* get_TriggerConditions() { return m_pConditions; }

private:

    IMPLEMENT_READONLY_REFERENCE_PROPERTY(EnterActions, TriggerActionCollection, false);
    IMPLEMENT_READONLY_REFERENCE_PROPERTY(ExitActions, TriggerActionCollection, false);

    static int m_layerRankNext;
    int m_layerRank;

    ItemStructList<PropertyValue> m_propertyValueList;
    TriggerActionCollection m_enterActions;
    TriggerActionCollection m_exitActions;
    TRefCountedPtr<TriggerConditionArray> m_pConditions;
};

typedef TRefCountedPtr<TriggerBase> TriggerBasePtr;


}; // namespace AVUI
