#pragma once

namespace AVUI {

class RelativeSource;    
class DependencyObject;
class Visual;

class ObjectRef : public Object
{
public:

    DECLARE_ELEMENT(ObjectRef, Object);

    virtual TRefCountedPtr<Object> GetDataObject(DependencyObject* pDO) { return GetObject(pDO); }
    virtual TRefCountedPtr<Object> GetDataObject(DependencyObject* pDO, bool resolveNamesInTemplate) { return GetDataObject(pDO); }
    virtual TRefCountedPtr<Object> GetObject(DependencyObject* pDO) { return NULL; }
    bool TreeContextIsRequired(DependencyObject* pTarget) { return ProtectedTreeContextIsRequired(pTarget); }
    bool get_UsesMentor() { return get_ProtectedUsesMentor(); }

protected:

    ObjectRef() { }
    virtual bool ProtectedTreeContextIsRequired(DependencyObject* pTarget) { return false; }
    virtual bool get_ProtectedUsesMentor() { return true; }
};

class ElementObjectRef : public ObjectRef
{
public:
    DECLARE_ELEMENT(ElementObjectRef, ObjectRef);

    ElementObjectRef(String* pName);
    virtual TRefCountedPtr<Object> GetDataObject(DependencyObject* pDO, bool resolveNamesInTemplate) { return GetDataObjectImpl(pDO, resolveNamesInTemplate); }
    virtual TRefCountedPtr<Object> GetObject(DependencyObject* pDO) { return GetDataObjectImpl(pDO, false); }

private:
    TRefCountedPtr<Object> GetDataObjectImpl(DependencyObject* pDO, bool resolveNamesInTemplate);

    TRefCountedPtr<String> m_pName;
};

class ExplicitObjectRef : public ObjectRef
{
public:
    DECLARE_ELEMENT(ExplicitObjectRef, ObjectRef);

    // Methods
    ExplicitObjectRef(Object* pObject);
    virtual TRefCountedPtr<Object> GetObject(DependencyObject* pDO);

protected:
    virtual bool get_ProtectedUsesMentor() { return false; }

private:

    TWeakPtr<Object> m_pElement;
    TRefCountedPtr<Object> m_pObject;
};


class RelativeObjectRef : public ObjectRef
{
public:
    DECLARE_ELEMENT(RelativeObjectRef, ObjectRef);

    RelativeObjectRef(RelativeSource* pRelativeSource);
    virtual TRefCountedPtr<Object> GetDataObject(DependencyObject* pDO, bool resolveNamesInTemplate);
    virtual TRefCountedPtr<Object> GetObject(DependencyObject* pDO);

    bool get_ReturnsDataContext();

protected:
    virtual bool ProtectedTreeContextIsRequired(DependencyObject* pTarget);
    virtual bool get_ProtectedUsesMentor();

private:
    TRefCountedPtr<DependencyObject> FindAncestorOfType(const Type* pType, int level, DependencyObject* pDO);
    TRefCountedPtr<Object> GetDataObjectImpl(DependencyObject* pDO);
    TRefCountedPtr<Object> GetPreviousData(DependencyObject* pDO);
    int IndexOf(Visual* pParent, Visual* pChild, Visual** ppPrevChild);

    TRefCountedPtr<RelativeSource> m_pRelativeSource;
};

};
