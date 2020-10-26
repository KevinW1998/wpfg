#pragma once

#include <AVUIItemStructList.h>
#include <AVUIObjectCollection.h>

namespace AVUI {


template <class T>
class ValueTypeCollection : public ItemStructList<T>, public ICollection
{
public:
    virtual bool get_IsEmpty() { return ItemStructList<T>::get_Count() == 0; }
    virtual int get_Count() { return ItemStructList<T>::get_Count(); }
    virtual void Clear() { ItemStructList<T>::Clear(); }
    virtual void Add(Object* pObject) { ItemStructList<T>::AddItem(UnboxValue<T>(pObject)); }
    virtual bool Contains(Object* pObject) { return ItemStructList<T>::Contains(UnboxValue<T>(pObject)); }
    virtual void Move(int oldIndex, int newIndex) { return ItemStructList<T>::Move(oldIndex, newIndex); }
    virtual bool Remove(Object* pObject) { return ItemStructList<T>::Remove(UnboxValue<T>(pObject)); }
    virtual void RemoveRange(int idx, int count) { for(int removeIdx = 0; removeIdx < count; removeIdx++) ItemStructList<T>::RemoveAt(removeIdx); }
    virtual void RemoveAt(int idx) { ItemStructList<T>::RemoveAt(idx); };
    virtual TRefCountedPtr<Object> GetItemAt(int idx) { return BoxValue<T>( (*this)[idx]); }
    virtual void SetItemAt(int idx, Object* pValue) { (*this)[idx] = UnboxValue<T>(pValue); }
    virtual int IndexOf(Object* pObject) { if(pObject->get_Type() != AVUI::get_Type((T*) 0)) return -1; return ItemStructList<T>::IndexOf(UnboxValue<T>(pObject)); }

};


}; // namespace AVUI
