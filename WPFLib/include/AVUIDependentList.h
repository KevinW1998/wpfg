#pragma once

namespace AVUI {

class Expression;
class DependencyObject;
class DependencyPropertyChangedEventArgs;

class DependentList : public Object
{
public:
    DependentList() { }
    ~DependentList() { };

    void InvalidateDependents(DependencyObject* pSource, const DependencyPropertyChangedEventArgs& eventArgs);
    bool get_IsEmpty() const { return m_dependentStorage.get_Count() == 0; };

    void Add(DependencyObject* pDO,  const DependencyProperty& dp, Expression* pExpression);
    void Remove(DependencyObject* pDO, const DependencyProperty& dp, Expression* pExpression);

private:

    struct Dependent
    {
        Dependent() : pDependencyProperty(NULL) { }
        Dependent(DependencyObject* pDO, Expression* pExpr, const DependencyProperty* pDP) : pDependencyObject(pDO), pExpression(pExpr), pDependencyProperty(pDP) { }

        TWeakPtr<DependencyObject> pDependencyObject;
        TWeakPtr<Expression> pExpression;
        const DependencyProperty* pDependencyProperty;
    };

    ItemStructList<Dependent> m_dependentStorage;
};

typedef TRefCountedPtr<DependentList> DependentListPtr;

};
