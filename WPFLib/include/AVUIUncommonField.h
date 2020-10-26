#pragma once

namespace AVUI {

class DependencyObject;


class UncommonFieldHelper
{
public:
    static void ClearValueHelper(DependencyObject* pDO, int globalIndex);
    static Object* GetValueHelper(DependencyObject* pDO, int globalIndex);
    static bool SetValueHelper(DependencyObject* pDO, Object* pValue, Object* pDefaultValue, int globalIndex);

};

template <class T>
class UncommonField
{
public:
    UncommonField(T* pDefaultValue = NULL)
    {
        m_pDefaultValue = pDefaultValue;
        m_hasBeenSet = false;
        m_globalIndex = DependencyProperty::get_UniqueGlobalID();
    }

    void ClearValue(DependencyObject* pDO)
    {
        if(pDO == NULL)
        {
            LibraryCriticalError();
        }

        UncommonFieldHelper::ClearValueHelper(pDO, m_globalIndex);
    }

    T* GetValue(DependencyObject* pDO)
    {
        if(pDO == NULL)
        {
            LibraryCriticalError();
        }

        if(m_hasBeenSet)
        {
            Object* pObject = UncommonFieldHelper::GetValueHelper(pDO, m_globalIndex);
            if(pObject != DependencyProperty::get_UnsetValue())
            {
                return (T*) pObject;
            }
        }

        return m_pDefaultValue;
    }

    void SetValue(DependencyObject* pDO, T* pValue)
    {
        if(pDO == NULL)
        {
            LibraryCriticalError();
        }

        if(UncommonFieldHelper::SetValueHelper(pDO, (Object*) pValue, (Object*) m_pDefaultValue, m_globalIndex))
        {
            m_hasBeenSet = true;
        }
    }
private:

    TRefCountedPtr<T> m_pDefaultValue;
    bool m_hasBeenSet;
    unsigned int m_globalIndex;
};


}; // namespace AVUI
