#pragma once

namespace AVUI {

class BoxedValueType : public Object
{
public:
    DECLARE_ELEMENT(BoxedValueType, Object);

    BoxedValueType() { };
    virtual const Type& get_ContainedType() const = 0;
    virtual void* GetContainedValueOffset() = 0;
};

template<class T>
class BoxedValueTypeInstance : public BoxedValueType
{
public:
    BoxedValueTypeInstance(const T& t) : m_value(t) { };

    virtual const Type& get_Type() const { return get_ContainedType(); };
    virtual bool IsType(const Type& rhs) const
    { 
        if(get_ContainedType() == rhs)
        {
            return true;
        }
        return BoxedValueType::IsType(rhs);
    } 


    virtual bool Equals(const Object* pObject) const
    {
        if(pObject == NULL)
        {
            return false;
        }

        if(pObject->get_Type() != get_Type())
        {
            return false;
        }

        return m_value == ((const BoxedValueTypeInstance<T> *) pObject)->m_value;
    }

    virtual const Type& get_ContainedType() const { return AVUI::get_Type((T*) 0); }
    const T& get_Value() const { return m_value; }

    virtual void* GetContainedValueOffset() { return &m_value; }

private:
    T m_value;
};

template<class T>
extern ObjectPtr BoxValue(const T& t)
{
    return ObjectPtr(new BoxedValueTypeInstance<T>(t), false);
}

template<class T>
const T& UnboxValue(const Object* pObject)
{
    // minimum possible check - this will be a hotspot.
    if(!pObject->IsType(AVUI::get_Type((T*) 0)))
    {
        LibraryCriticalError();
    }

    return ((BoxedValueTypeInstance<T> *) pObject)->get_Value();
}



}; // namespace AVUI
